#include "zf_common_headfile.h"
#include "Header.h"

// 控制标志位
uint8 go_flag = 0,stop_flag = 0,stop_time = 0;

// 速度控制参数
int16 speed = 0;  // 目标速度
uint8 start_time = 0;  // 启动计时

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

// 停车函数(统一停车接口)
void car_stop(void)
{
    // 检测到停车标志位
    if(stop_flag)
    {
        // 阶段1: 减速阶段(stop_time 在 10ms 定时器中递增)
        if(stop_time < 30)  // 10ms * 30 = 300ms
        {
            // 保持电机运行,由 motor_process() 逐步减速
            // 这里不做任何操作,让电机以当前速度运行
        }
        else  // 阶段2: 300ms后,关闭所有硬件并复原标志位
        {
            // 关闭定时器
            if(pit_10ms_timer != nullptr)
                pit_10ms_timer->stop();
            /* if(pit_100ms_timer != nullptr)
                pit_100ms_timer->stop(); */

            // 立即关闭电机(调用 motor_stop)
            motor_stop();

            // 禁用舵机PWM输出(允许手动转动)
            servo_disable();

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

            // 蜂鸣器提示
            beep_flag = 1;

            printf("Car stopped completely!\r\n");
        }
    }
}
