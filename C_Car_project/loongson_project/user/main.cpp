
#include "zf_common_headfile.h"
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
    // 1. 初始化 IPS200 显示屏
    ips200_init("/dev/fb0");
    printf("IPS200 display initialized.\r\n");

    // 2. 初始化 UVC 摄像头
    if(uvc_camera_init("/dev/video0") < 0)
    {
        printf("UVC camera initialization failed!\r\n");
        return -1;
    }
    printf("UVC camera initialized.\r\n");

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

    printf("All modules initialized successfully!\r\n");
    return 0;
}

int main(int, char**)
{
    // 执行所有模块的初始化
    if(all_init() < 0)
    {
        printf("Initialization failed, exiting...\r\n");
        return -1;
    }

    printf("System started, entering main loop...\r\n");

    while(1)
    {
        // 状态判断逻辑
        if(go_flag == 0)
        {
            // 车辆停止状态：只运行菜单系统
            show_process(NULL);
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
            image_process_time_end();             // 结束计时
        }

        // 短暂延时，避免CPU占用过高
        system_delay_ms(1);
    }
}