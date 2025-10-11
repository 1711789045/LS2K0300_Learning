#include "zf_common_headfile.h"
#include  "motor.h"
#include "pid.h"
#include "auto_menu.h"
#include "image.h"
#include "control.h"
#include "servo.h"
#include "beep.h"
#include <math.h>
//占空比最大值是10000

uint8 go_flag = 0,stop_time = 0,stop_flag = 0,block_time = 0;
uint8 motor_f = 0;
int16 encoder_data_l = 0;
int16 encoder_data_r = 0;
int16 speed_l = 0,speed_r = 0;

static PID_INCREMENT_TypeDef pid_left = {0};
static PID_INCREMENT_TypeDef pid_right = {0};

float motor_pid_kp = 8.0,motor_pid_ki = 2.0,motor_pid_kd = 4.0;
uint8 differential_mode = 0;

void motor_init(void){
	pwm_get_dev_info(MOTOR1_PWM, &motor_1_pwm_info);
    pwm_get_dev_info(MOTOR2_PWM, &motor_2_pwm_info);
}

void motor_set_pid(float kp,float ki,float kd){
	motor_pid_kp = kp;
	motor_pid_ki = ki;
	motor_pid_kd = kd;
}

void motor_setpwm(uint8 motor,int16 speed){

	if(!motor){
		if(speed >= 0){
			if(speed>SPEED_LIMIT)
				speed = SPEED_LIMIT;
			pwm_set_duty(MOTOR_L_PWM_CH4, speed);
			gpio_set_level(MOTOR_L_DIR, 1);
		}
		else{
			speed = -speed;
			if(speed>=SPEED_LIMIT)
				speed = SPEED_LIMIT;
			pwm_set_duty(MOTOR_L_PWM_CH4, speed);
			gpio_set_level(MOTOR_L_DIR, 0);
		}
	}
	else{
		if(speed >= 0){
			if(speed>SPEED_LIMIT)
				speed = SPEED_LIMIT;
			pwm_set_duty(MOTOR_R_PWM_CH2, speed);
			gpio_set_level(MOTOR_R_DIR, 1);
		}
		else{
			speed = -speed;
			if(speed>=SPEED_LIMIT)
				speed = SPEED_LIMIT;
			pwm_set_duty(MOTOR_R_PWM_CH2, speed);
			gpio_set_level(MOTOR_R_DIR, 0);
		}
	}
}

void motor_setspeed(int16 target, float current_l, float current_r,uint8 differential_mode) {
	speed_l = 0,speed_r = 0;
	if(!differential_mode){
		speed_l = pid_increment(&pid_left, target, current_l, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
    
		speed_r = pid_increment(&pid_right, target, current_r, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
	}
	if(differential_mode){
		float err = final_mid_line - IMAGE_W/2;
//		float k = func_limit_ab((func_abs(err)-10),0,100);
		float k = func_abs(angle);


		if(final_mid_line - IMAGE_W/2 >10){
			speed_r = pid_increment(&pid_right,target+ (int16)(dif_speed_reduce*k), current_r, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
			speed_l = pid_increment(&pid_left, target+  (int16)(dif_speed_plus*k), current_l, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
		}
		else if(final_mid_line - IMAGE_W/2 <-10){
			speed_r = pid_increment(&pid_right,target+ (int16)(dif_speed_plus*k), current_r, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
		
			speed_l = pid_increment(&pid_left, target+ (int16)(dif_speed_reduce*k), current_l, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
		}
		else{
			speed_r = pid_increment(&pid_right, target, current_r, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
			speed_l = pid_increment(&pid_left, target, current_l, 
                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
		}
	}


//	if(differential_mode){
//		float err = final_mid_line - IMAGE_W/2;
////		float k = func_limit_ab((func_abs(err)-10),0,100);
//		float k = CAR_B*tan(2*angle)/(2*CAR_L);


//		if(final_mid_line - IMAGE_W/2 >10){
//			speed_r = pid_increment(&pid_right,target*(1.0-dif_speed_reduce*k), current_r, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//			speed_l = pid_increment(&pid_left, target*(1.0+dif_speed_plus*k), current_l, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//		}
//		else if(final_mid_line - IMAGE_W/2 <-10){
//			speed_r = pid_increment(&pid_right,target*(1.0+dif_speed_plus*k), current_r, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//		
//			speed_l = pid_increment(&pid_left, target*(1.0-dif_speed_reduce*k), current_l, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//		}
//		else{
//			speed_r = pid_increment(&pid_right, target, current_r, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//			speed_l = pid_increment(&pid_left, target, current_l, 
//                                 SPEED_LIMIT, motor_pid_kp, motor_pid_ki, motor_pid_kd);
//		}
//	}


	ips200_show_int(0,208,speed_l,4);
	ips200_show_int(0,224,speed_r,4);
	
    motor_setpwm(MOTOR_L, speed_l);
    motor_setpwm(MOTOR_R, speed_r);
}
void motor_stop(void){
	motor_setspeed(0,encoder_data_l,encoder_data_r,0);
	if(stop_time == 30){
		beep_flag = 1;
		stop_flag = 0;
		pit_10ms_timer->stop();
		pit_100ms_timer->stop();			
		pwm_set_duty(MOTOR1_PWM, 0);   
    	pwm_set_duty(MOTOR2_PWM, 0);    
	}
}

void motor_protect(void){
	if(prospect<5 && start_time > 30){
		go_flag = 0;
		stop_flag = 1;
		stop_time = 0;
		beep_flag = 1;
	}
//	if((encoder_data_l<=100 && speed_l>=5000) || (encoder_data_r<=100  && speed_r>=5000)){
//		block_time++;
//		
//		if(block_time >= 40){
//			go_flag = 0;
//			stop_flag = 1;
//			stop_time = 0;
//			beep_flag = 3;
//		}
//	}
//	else
//		block_time = 0;
}



void motor_process(void){
	if(motor_f){
		
		if(go_flag){
			motor_setspeed(speed,encoder_data_l,encoder_data_r,differential_mode);
			motor_protect();
		}
		if(stop_flag){
			motor_stop();
		}
		motor_f = 0;
	}
}

