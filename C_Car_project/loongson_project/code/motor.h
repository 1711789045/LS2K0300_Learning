#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "zf_common_headfile.h"

#define MOTOR_L    0
#define MOTOR_R    1

#define MOTOR_L_PWM_CH4                 "/dev/zf_device_pwm_motor_1"
#define MOTOR_R_PWM_CH2                 "/dev/zf_device_pwm_motor_2"

#define MOTOR_L_DIR             "/dev/zf_driver_gpio_motor_1"
#define MOTOR_R_DIR             "/dev/zf_driver_gpio_motor_2"

// PWM 信息结构体（在 motor.cpp 中定义）
extern struct pwm_info motor_1_pwm_info;
extern struct pwm_info motor_2_pwm_info;

#define SPEED_LIMIT             6000           // 速度限制

#define CAR_B                   15
#define CAR_L                   19

extern uint8 go_flag,stop_time,stop_flag,block_time;
extern uint8 differential_mode;
extern uint8 motor_f;
extern int16 speed_l, speed_r;
extern float motor_pid_kp, motor_pid_ki, motor_pid_kd;

// 差速控制参数（需在其他地方定义）
extern float dif_speed_reduce;
extern float dif_speed_plus;
extern int16 speed;
extern uint8 start_time;

void motor_init(void);
void motor_set_pid(float kp,float ki,float kd);
void motor_setpwm(uint8 motor,int16 speed);
void motor_setspeed(int16 target, float current_l, float current_r,uint8 differential_mode) ;
void motor_lose_line_protect(void);
void motor_process(void);

#endif
