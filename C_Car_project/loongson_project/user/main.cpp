
#include "zf_common_headfile.h"
#include "config_flash.h"  // 配置库（必须最先初始化）
#include "motor.h"
#include "servo.h"
#include "encoder.h"
#include "pit.h"
#include "image.h"
#include "key.h"
#include "beep.h"
#include "auto_menu.h"
#include "control.h"

int all_init(){
    // 0. 初始化配置系统（最先执行，加载所有配置变量）
    config_init();
    printf("配置系统初始化完成\r\n");

    // 1. 初始化 IPS200 显示屏
    ips200_init("/dev/fb0");
    printf("IPS200 显示屏初始化完成\r\n");

    // 2. 初始化 UVC 摄像头
    if(uvc_camera_init("/dev/video0") < 0)
    {
        printf("UVC 摄像头初始化失败！\r\n");
        return -1;
    }
    printf("UVC 摄像头初始化完成\r\n");

    // 3. 初始化电机
    motor_init();

   /*  // 4. 初始化舵机
    servo_init(); */

    /* // 5. 初始化编码器
    encoder_init();
 */
    // 6. 初始化定时器
    pit_init();

    // 7. 初始化按键
    key_into();

    // 8. 初始化菜单系统
    menu_init();

    printf("所有模块初始化成功！\r\n");
    return 0;
}

int main(int, char**)
{
    // 执行所有模块的初始化
    if(all_init() < 0)
    {
        printf("初始化失败，程序退出...\r\n");
        return -1;
    }

    printf("系统启动完成，进入主循环...\r\n");

    while(1)
    {
        // 状态判断逻辑
        if(go_flag == 0)
        {
            // 车辆停止状态：只运行菜单系统
            show_process(NULL);
            system_delay_ms(10); // 菜单刷新间隔
        }
        else
        {
            // 车辆运行状态：运行控制系统，停止菜单

            // 检查并执行停车逻辑
            car_stop();

            // 如果已经完全停止，继续循环
            if(go_flag == 0)
                continue;

            /* // 编码器处理（标志位驱动）
            encoder_process();

            // 舵机控制处理（标志位驱动）
            servo_process();

            // 电机控制处理（标志位驱动）
            motor_process(); */

            // 图像处理（计时）
            image_process_time_start();           // 开始计时
            image_process(IMAGE_W, IMAGE_H, 0);   // 图像处理（显示模式0：仅图像）
            float process_time = image_process_time_end();  // 结束计时并获取时间

            // 只打印单行实时数据，避免频繁printf导致性能下降
            static uint32_t print_counter = 0;
            if(++print_counter >= 100)  // 每100帧打印一次详细统计
            {
                image_process_time_print();
                print_counter = 0;
            }

            // 短暂延时，避免CPU占用过高
            system_delay_ms(1);
        }

    }
}