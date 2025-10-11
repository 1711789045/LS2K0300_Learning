#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "zf_common_headfile.h"

#define MOTOR_L    0
#define MOTOR_R    1

#define MOTOR_L_PWM_CH4                 "/dev/zf_device_pwm_motor_1"
#define MOTOR_R_PWM_CH2                 "/dev/zf_device_pwm_motor_2"

#define MOTOR_L_DIR             "/dev/zf_driver_gpio_motor_1"
#define MOTOR_R_DIR             "/dev/zf_driver_gpio_motor_2"

struct pwm_info motor_1_pwm_info;
struct pwm_info motor_2_pwm_info;

#define SPEED_LIMIT             6000           //速度上限

#define CAR_B                   15
#define CAR_L                   19

extern uint8 go_flag,stop_time,stop_flag,block_time;
extern uint8 differential_mode;
extern uint8 motor_f;
extern int16 encoder_data_l;
extern int16 encoder_data_r;
void motor_init(void);
void motor_set_pid(float kp,float ki,float kd);
void motor_setpwm(uint8 motor,int16 speed);
void motor_setspeed(int16 target, float current_l, float current_r,uint8 differential_mode) ;
void motor_lose_line_protect(void);
void motor_process(void);

#endif
