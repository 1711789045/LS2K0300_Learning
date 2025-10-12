#include "zf_common_headfile.h"
#include "Header.h"

// 控制标志位
uint8 go_flag = 0,stop_flag = 0,stop_time = 0;

// 速度控制参数
int16 speed = 2000;  // 目标速度
uint8 start_time = 0;  // 启动计时

// 差速控制参数
float dif_speed_reduce = 0.3;  // 差速减速系数
float dif_speed_plus = 0.3;    // 差速加速系数

// 发车函数
void car_start(void)
{
    if(IS_OK){
    // 设置发车标志位
    go_flag = 1;
    stop_flag = 0;

    // 重置计时器
    start_time = 0;
    stop_time = 0;

    // 重置图像处理标志位
    circle_flag = 0;
    cross_flag = 0;
    circle_time = 0;

    // 启动定时器
    if(pit_10ms_timer != nullptr)
        pit_10ms_timer->start();
    if(pit_100ms_timer != nullptr)
        pit_100ms_timer->start();

    printf("Car started!\r\n");
    }
}

// 停车函数（统一停车接口）
void car_stop(void)
{
    // 检测到停车标志位
    if(stop_flag)
    {
        // 阶段1: 3秒减速阶段（stop_time 在 10ms 定时器中递增）
        if(stop_time < 30)  // 10ms * 30 = 300ms = 3秒的1/10，实际是3秒需要300次
        {
            // 保持速度闭环，目标速度设为0，让车快速停下
            // motor_stop() 函数已经在 motor.cpp 中实现了速度闭环减速
            // 这里不需要额外操作，motor_process() 会调用 motor_stop()
        }
        else  // 阶段2: 3秒后，关闭所有硬件并复原标志位
        {
            // 关闭定时器
            if(pit_10ms_timer != nullptr)
                pit_10ms_timer->stop();
            if(pit_100ms_timer != nullptr)
                pit_100ms_timer->stop();

            // 关闭电机PWM
            pwm_set_duty(MOTOR_L_PWM_CH4, 0);
            pwm_set_duty(MOTOR_R_PWM_CH2, 0);

            // 舵机回中（可选，根据需要）
            // servo_set_angle(SERVO_MID_ANGLE);

            // 复原所有标志位
            go_flag = 0;
            stop_flag = 0;
            stop_time = 0;
            start_time = 0;

            // 复原图像处理标志位
            circle_flag = 0;
            cross_flag = 0;
            circle_time = 0;
            mid_mode = 0;

            printf("Car stopped!\r\n");
        }
    }
}
