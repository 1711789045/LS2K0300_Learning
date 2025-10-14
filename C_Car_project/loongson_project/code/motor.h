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

// ==================== 电机配置参数声明 ====================
// 这些参数在 motor.cpp 中定义，可被 config_flash 等模块引用
extern float motor_pid_kp;          // 电机PID Kp参数
extern float motor_pid_ki;          // 电机PID Ki参数
extern float motor_pid_kd;          // 电机PID Kd参数

// 电机控制变量（部分在 control.cpp 中定义，需包含 control.h）
extern uint8 block_time;
extern uint8 motor_f;
extern uint8 differential_mode;
extern int16 speed_l, speed_r;

void motor_init(void);
void motor_set_pid(float kp,float ki,float kd);
void motor_setpwm(uint8 motor,int16 speed);
void motor_setspeed(int16 target, float current_l, float current_r,uint8 differential_mode) ;
void motor_stop(void);           // 电机立即停止(关闭PWM)
void motor_lose_line_protect(void);
void motor_process(void);

#endif
