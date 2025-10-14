/*********************************************************************************************************************
* LS2K0300 Opensourec Library 即（LS2K0300 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是LS2K0300 开源库的一部分
*
* LS2K0300 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          servo
* 公司名称          成都逐飞科技有限公司
* 适用平台          LS2K0300
* 店铺链接          https://seekfree.taobao.com/
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "servo.h"
#include "auto_menu.h"
#include "config_flash.h"  // 使用配置库
#include "pid.h"
#include "image.h"
#include "control.h"
#include "key.h"           // 引入按键处理
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// 从 auto_menu.cpp 引入的按键变量
extern uint8 button1, button2, button3, button4;
// IS_OK 是 auto_menu.h 中定义的宏，不需要 extern 声明

// PWM 信息结构体
static struct pwm_info servo_pwm_info;

// PID 控制器
static PID_POSITIONAL_TypeDef turn_pid = {0};

// ==================== 舵机配置参数（在本模块中定义）====================
float g_servo_mid = 69.7;       // 舵机中值（默认值）
float servo_pid_kp = 0.35;      // 舵机PID Kp参数
float servo_pid_ki = 0.0;       // 舵机PID Ki参数
float servo_pid_kd1 = 0.56;     // 舵机PID Kd1参数
float servo_pid_kd2 = 0.0;      // 舵机PID Kd2参数

// 舵机控制变量
float angle = 0;
uint8 servo_f = 0;
float last_err = 0;

// 占空比计算宏（根据角度计算PWM占空比）
// 舵机 0-180度对应 0.5ms-2.5ms 高电平
#define SERVO_MOTOR_DUTY(x) ((float)servo_pwm_info.duty_max / (1000.0 / (float)servo_pwm_info.freq) * (0.5 + (float)(x) / 90.0))

/**
 * @brief  舵机初始化
 * @param  无
 * @return 无
 * @note   不再需要手动加载配置，config_init() 会在 main.cpp 中统一调用
 */
void servo_init(void)
{
    // 配置已经在 main.cpp 中通过 config_init() 加载
    // g_servo_mid 已经被赋值，可以直接使用

    // 获取舵机PWM设备信息
    pwm_get_dev_info(SERVO_MOTOR_PWM, &servo_pwm_info);

    // 打印PWM信息（调试用）
    printf("Servo PWM freq = %d Hz\r\n", servo_pwm_info.freq);
    printf("Servo PWM duty_max = %d\r\n", servo_pwm_info.duty_max);
    printf("Loaded servo_mid = %.2f\r\n", g_servo_mid);

    // 设置舵机到中位
    servo_setangle(0);
}

/**
 * @brief  设置舵机角度
 * @param  angle: 偏转角度（相对于中位）
 * @return 无
 */
void servo_setangle(float angle)
{
    // 限制角度范围
    angle = func_limit(angle, SERVO_MOTOR_LIMIT);

    // 计算实际舵机角度
    float servo_motor_duty = g_servo_mid + angle;

    // 计算左右最大角度（基于全局中值）
    float servo_motor_l_max = g_servo_mid - SERVO_MOTOR_LIMIT;
    float servo_motor_r_max = g_servo_mid + SERVO_MOTOR_LIMIT;

    // 限制在左右最大角度范围内
    if(servo_motor_duty >= servo_motor_r_max)
        servo_motor_duty = servo_motor_r_max;
    if(servo_motor_duty <= servo_motor_l_max)
        servo_motor_duty = servo_motor_l_max;

    // 设置PWM占空比
    pwm_set_duty(SERVO_MOTOR_PWM, (uint16)SERVO_MOTOR_DUTY(servo_motor_duty));
}

/**
 * @brief  设置舵机PID参数
 * @param  kp: 比例系数
 * @param  ki: 积分系数
 * @param  kd1: 微分系数1
 * @param  kd2: 微分系数2
 * @return 无
 * @note   这些参数已移至 config_flash 统一管理，本函数保留用于兼容性
 */
void servo_set_pid(float kp, float ki, float kd1, float kd2)
{
    servo_pid_kp = kp;
    servo_pid_ki = ki;
    servo_pid_kd1 = kd1;
    servo_pid_kd2 = kd2;
}

/**
 * @brief  舵机PID控制
 * @param  mid_line: 中线位置
 * @return 无
 */
void servo_control(uint8 mid_line)
{
    // 计算误差（中线偏差）
    float err = mid_line - IMAGE_W / 2.0;
    float kp = 0, kd = 0;

    // 变增益PID控制
    // kp根据误差大小动态调整
    kp = func_limit_ab(servo_pid_kp * (err * err) / 1000 + 0.1, 0, 3);
    kd = servo_pid_kd2;

    // 当误差突变且较大时，减小微分系数
    if(func_abs(err) - func_abs(last_err) > 0 && func_abs(err) > 30)
        kd = 0.1;

    // PID计算舵机角度
    angle = pid_positional(&turn_pid, 0, err, SERVO_MOTOR_LIMIT,
                          kp, servo_pid_ki, servo_pid_kd1, kd);

    // 设置舵机角度
    servo_setangle(angle);

    // 保存本次误差
    last_err = err;
}

/**
 * @brief  舵机处理函数（在定时器中调用）
 * @param  无
 * @return 无
 */
void servo_process(void)
{
    if(servo_f)
    {
        // 更新PID参数（从菜单系统）
        // 注意：现在 servo_pid_kp 等参数已经是全局变量，可以直接使用

        // 运行状态下进行舵机控制
        if(go_flag)
        {
            servo_control(final_mid_line);
        }

        // 停止状态处理
        if(stop_flag)
        {
            if(stop_time <= 25)
            {
                // 停止后短时间内继续控制
                servo_control(final_mid_line);
            }
            else
            {
                // 超时后舵机回中位
                servo_setangle(0);
            }
        }

        servo_f = 0;
    }
}

/**
 * @brief  舵机手动调整函数
 * @param  无
 * @return 无
 * @note   必须按确认键(IS_OK)才进入调整模式
 *         在调整模式中，通过两个虚拟参数(+0.1/-0.1)调整舵机中值
 *         按button1(返回键)保存并退出
 */
void servo_manual_adjust(void)
{
    if(IS_OK)  // 必须按下确认键才进入调整模式
    {
        // 使用当前保存的中值初始化
        float current_angle = g_servo_mid;

        // 选择的参数索引：0 = angle_inc (+0.1), 1 = angle_dec (-0.1)
        uint8 param_index = 0;

        // 清屏
        ips200_full(IPS200_DEFAULT_BGCOLOR);

        printf("Entering servo manual adjust mode, current mid = %.2f\r\n", current_angle);

        // 进入手动调整主循环
        while(1)
        {
            // 更新按键状态（必须在每次循环中调用）
            button_entry(NULL);

            // 清除上次显示
            ips200_full(IPS200_DEFAULT_BGCOLOR);

            // 显示标题和当前角度
            showstr(0, 0 * DIS_Y, "SERVO ADJUST");
            showstr(0, 1 * DIS_Y, "Current:");
            showfloat(80, 1 * DIS_Y, current_angle, 4, 2);

            // 显示两个虚拟参数
            if(param_index == 0)
                showstr(0, 3 * DIS_Y, ">angle +0.1");
            else
                showstr(0, 3 * DIS_Y, " angle +0.1");

            if(param_index == 1)
                showstr(0, 4 * DIS_Y, ">angle -0.1");
            else
                showstr(0, 4 * DIS_Y, " angle -0.1");

            // 显示退出提示
            showstr(0, 5 * DIS_Y, "KEY1=SAVE&EXIT");

            // 检测返回键(button1)：保存并退出
            if(button1)
            {
                g_servo_mid = current_angle;  // 更新全局中值变量
                config_save();                 // 使用配置库保存所有配置
                printf("Configuration saved: servo_mid = %.2f\r\n", g_servo_mid);

                // 清屏并退出
                ips200_full(IPS200_DEFAULT_BGCOLOR);
                break;
            }

            // 检测上下键：切换参数选择
            if(button3)  // 向上
            {
                param_index = (param_index == 0) ? 1 : 0;
            }
            else if(button4)  // 向下
            {
                param_index = (param_index == 0) ? 1 : 0;
            }

            // 检测确认键(button2/IS_OK)：执行加/减操作
            if(button2)
            {
                if(param_index == 0)
                {
                    // angle_inc: +0.1
                    current_angle += 0.1;
                    printf("Angle increased to: %.2f\r\n", current_angle);
                }
                else if(param_index == 1)
                {
                    // angle_dec: -0.1
                    current_angle -= 0.1;
                    printf("Angle decreased to: %.2f\r\n", current_angle);
                }

                // 立即更新舵机位置
                pwm_set_duty(SERVO_MOTOR_PWM, (uint16)SERVO_MOTOR_DUTY(current_angle));
            }

            system_delay_ms(20);  // 主循环延迟，与button_entry的延迟一致
        }
    }
}

// ==================== 旧的配置函数已移除 ====================
// servo_save_config() 和 servo_load_config() 已移除
// 现在统一使用 config_flash.cpp 中的 config_save() 和 config_load()

