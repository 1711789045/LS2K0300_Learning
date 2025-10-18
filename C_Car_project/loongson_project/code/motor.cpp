#include "zf_common_headfile.h"
#include "motor.h"
#include "pid.h"
#include "auto_menu.h"
#include "image.h"
#include "control.h"
#include "servo.h"
#include "beep.h"
#include "encoder.h"
#include "pit.h"
#include <math.h>

// PWM 信息结构体
struct pwm_info motor_1_pwm_info;
struct pwm_info motor_2_pwm_info;

// 电机控制变量（go_flag, stop_time, stop_flag 在 control.cpp 中定义）
uint8 block_time = 0;
uint8 motor_f = 0;
uint8 differential_mode = 0;  // 差速模式标志
int16 speed_l = 0,speed_r = 0;

static PID_INCREMENT_TypeDef pid_left = {0};
static PID_INCREMENT_TypeDef pid_right = {0};

float motor_pid_kp = 8.0,motor_pid_ki = 2.0,motor_pid_kd = 4.0;

// 阿克曼差速控制参数
float inner_wheel_coef = 0.8;  // 内轮系数（内轮减速为主）
float outer_wheel_coef = 0.2;  // 外轮系数（外轮加速为辅）
uint16 differential_enable = 1; // 差速开关（默认开启）

void motor_init(void){
	pwm_get_dev_info(MOTOR_L_PWM_CH4, &motor_1_pwm_info);
    pwm_get_dev_info(MOTOR_R_PWM_CH2, &motor_2_pwm_info);

    printf("Motor initialized.\r\n");
    printf("Motor L PWM freq = %d Hz, duty_max = %d\r\n",
           motor_1_pwm_info.freq, motor_1_pwm_info.duty_max);
    printf("Motor R PWM freq = %d Hz, duty_max = %d\r\n",
           motor_2_pwm_info.freq, motor_2_pwm_info.duty_max);
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

/* void motor_setspeed(int16 target, float current_l, float current_r,uint8 differential_mode) {
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
} */
/**
 * @brief  电机停止函数
 * @note   立即关闭电机 PWM 和方向引脚,用于紧急停止
 */
void motor_stop(void)
{
    // 立即关闭电机PWM
    pwm_set_duty(MOTOR_L_PWM_CH4, 0);
    pwm_set_duty(MOTOR_R_PWM_CH2, 0);

    // 方向引脚设为低电平(可选,防止漏电)
    gpio_set_level(MOTOR_L_DIR, 0);
    gpio_set_level(MOTOR_R_DIR, 0);

    printf("Motor stopped (PWM=0)\r\n");
}

void motor_protect(void){
	// 前瞻过小保护（可能丢线或冲出赛道）
	if(prospect<5 && start_time > 30){
		// 统一停车接口：只需设置 stop_flag = 1
		stop_flag = 1;
	}

	// 编码器堵转保护（可选，目前注释掉）
//	if((encoder_data_l<=100 && speed_l>=5000) || (encoder_data_r<=100  && speed_r>=5000)){
//		block_time++;
//
//		if(block_time >= 40){
//			// 统一停车接口：只需设置 stop_flag = 1
//			stop_flag = 1;
//			beep_flag = 3;
//		}
//	}
//	else
//		block_time = 0;
}

/**
 * @brief  阿克曼差速控制函数
 * @param  base_speed: 基础速度（目标平均速度）
 * @param  steering_angle: 前轮转向角（度）
 * @note   基于阿克曼转向模型计算左右轮差速
 *         公式：k = B*tan(α)/(2*L)
 *         α>0右转: 右轮内轮减速 V*(1-inner_coef*k), 左轮外轮加速 V*(1+outer_coef*k)
 *         α<0左转: 左轮内轮减速 V*(1-inner_coef*|k|), 右轮外轮加速 V*(1+outer_coef*|k|)
 *         内轮系数inner_wheel_coef（默认0.8），外轮系数outer_wheel_coef（默认0.2）
 *         差速主要靠内轮减速，少部分靠外轮加速
 */
void motor_ackermann_control(int16 base_speed, float steering_angle)
{
	// 限制转向角在±30度范围内
	steering_angle = func_limit_ab(steering_angle, -30.0f, 30.0f);

	// 将角度转换为弧度
	float alpha_rad = steering_angle * 3.14159265f / 180.0f;

	// 计算阿克曼差速系数 k = B*tan(α)/(2*L)
	float k = (float)CAR_B * tanf(alpha_rad) / (2.0f * (float)CAR_L);

	float v_left, v_right;

	// 判断转向方向，应用内外轮系数
	if(steering_angle > 0.5f) {
		// 右转（k > 0）：右轮是内轮，左轮是外轮
		// 右轮（内轮）：主要减速
		// 左轮（外轮）：少量加速
		v_right = (float)base_speed * (1.0f - inner_wheel_coef * k);
		v_left = (float)base_speed * (1.0f + outer_wheel_coef * k);
	}
	else if(steering_angle < -0.5f) {
		// 左转（k < 0）：左轮是内轮，右轮是外轮
		// 左轮（内轮）：主要减速
		// 右轮（外轮）：少量加速
		// 注意：k是负数，所以用-k得到正数
		v_left = (float)base_speed * (1.0f + inner_wheel_coef * k);   // k<0, 相当于减速
		v_right = (float)base_speed * (1.0f - outer_wheel_coef * k);  // k<0, 相当于加速
	}
	else {
		// 直行：左右轮同速
		v_left = (float)base_speed;
		v_right = (float)base_speed;
	}

	// 限幅并赋值
	speed_l = (int16)func_limit_ab(v_left, -SPEED_LIMIT, SPEED_LIMIT);
	speed_r = (int16)func_limit_ab(v_right, -SPEED_LIMIT, SPEED_LIMIT);

	// 设置PWM
	motor_setpwm(MOTOR_L, speed_l);
	motor_setpwm(MOTOR_R, speed_r);
}



void motor_process(void){
	if(motor_f){

		if(go_flag){
			// 判断是否启用差速控制
			if(differential_enable){
				// 启用差速：使用阿克曼差速控制
				// alpha = angle * 2（根据您的说明）
				float steering_angle = angle * 2.0f;
				motor_ackermann_control(speed, steering_angle);
			}
			else{
				// 关闭差速：匀速控制，左右轮同速
				motor_setpwm(MOTOR_L, speed);
				motor_setpwm(MOTOR_R, speed);
			}

			motor_protect();
		}
		if(stop_flag){
			motor_stop();
		}
		motor_f = 0;
	}
}

