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

    // 初始化时禁用舵机PWM输出,保持无信号状态
    servo_disable();
    printf("Servo initialized in disabled state (no PWM)\r\n");
}

/**
 * @brief  禁用舵机PWM输出
 * @param  无
 * @return 无
 * @note   将PWM占空比设置为0,使舵机失去信号,可以手动转动
 */
void servo_disable(void)
{
    // 将占空比设为0,关闭PWM输出
    pwm_set_duty(SERVO_MOTOR_PWM, 0);
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
        // 读取IMU660RA陀螺仪数据
        imu660ra_get_gyro();

        // 更新PID参数（从菜单系统）
        // 注意：现在 servo_pid_kp 等参数已经是全局变量，可以直接使用

        // 运行状态下进行舵机控制
        if(go_flag)
        {
            servo_control(final_mid_line);
        }
        else
        {
            // 未发车状态(go_flag=0)下禁用舵机PWM,可以手动转动
            servo_disable();
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
                // 超时后禁用舵机PWM,不再输出信号
                servo_disable();
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
 *         KEY3/KEY4: 快速调整角度(+0.1/-0.1)
 *         KEY2: 微调步长切换(0.1/0.01)
 *         KEY1: 保存并退出
 */
void servo_manual_adjust(void)
{
    if(IS_OK)  // 必须按下确认键才进入调整模式
    {
        // 使用当前保存的中值初始化
        float current_angle = g_servo_mid;
        float last_display_angle = current_angle;
        float step = 0.1;  // 调整步长

        // 清屏一次
        ips200_full(IPS200_DEFAULT_BGCOLOR);

        // 显示固定标题和提示(只显示一次)
        showstr(0, 0 * DIS_Y, "SERVO ADJUST");
        showstr(0, 2 * DIS_Y, "Current:");
        showfloat(80, 2 * DIS_Y, current_angle, 4, 2);  // 初始显示角度
        showstr(0, 4 * DIS_Y, "Step:");
        showfloat(60, 4 * DIS_Y, step, 3, 2);  // 初始显示步长
        showstr(0, 6 * DIS_Y, "KEY1=SAVE");
        showstr(0, 7 * DIS_Y, "KEY2=STEP");
        showstr(0, 8 * DIS_Y, "KEY3=UP");
        showstr(0, 9 * DIS_Y, "KEY4=DOWN");

        printf("Entering servo manual adjust mode, current mid = %.2f\r\n", current_angle);

        // 立即设置舵机到当前中值角度(绝对角度)
        printf("Setting servo to absolute angle: %.2f\r\n", current_angle);
        pwm_set_duty(SERVO_MOTOR_PWM, (uint16)SERVO_MOTOR_DUTY(current_angle));

        // 进入手动调整主循环
        while(1)
        {
            // 更新按键状态(必须在每次循环开始)
            button_entry(NULL);

            // 检测返回键(button1): 保存并退出
            if(button1)
            {
                g_servo_mid = current_angle;  // 更新全局中值变量
                config_save();                 // 使用配置库保存所有配置
                printf("Configuration saved: servo_mid = %.2f\r\n", g_servo_mid);

                // 清屏并退出
                ips200_full(IPS200_DEFAULT_BGCOLOR);
                break;
            }

            // 检测步长切换键(button2): 切换 0.1 <-> 0.01
            if(button2)
            {
                step = (step == 0.1) ? 0.01 : 0.1;
                // 清除旧的步长显示
                showstr(60, 4 * DIS_Y, "      ");
                // 显示新步长
                showfloat(60, 4 * DIS_Y, step, 3, 2);
                printf("Step changed to: %.2f\r\n", step);
            }

            // 检测KEY3: 增加角度
            if(button3)
            {
                current_angle += step;
                printf("Angle increased to: %.2f\r\n", current_angle);

                // 立即更新舵机到绝对角度位置
                printf("Setting servo PWM duty: %u (angle=%.2f)\r\n",
                       (uint16)SERVO_MOTOR_DUTY(current_angle), current_angle);
                pwm_set_duty(SERVO_MOTOR_PWM, (uint16)SERVO_MOTOR_DUTY(current_angle));
            }

            // 检测KEY4: 减小角度
            if(button4)
            {
                current_angle -= step;
                printf("Angle decreased to: %.2f\r\n", current_angle);

                // 立即更新舵机到绝对角度位置
                printf("Setting servo PWM duty: %u (angle=%.2f)\r\n",
                       (uint16)SERVO_MOTOR_DUTY(current_angle), current_angle);
                pwm_set_duty(SERVO_MOTOR_PWM, (uint16)SERVO_MOTOR_DUTY(current_angle));
            }

            // 只在角度变化时更新显示(减少屏幕刷新)
            if(current_angle != last_display_angle)
            {
                // 清除旧的角度显示
                showstr(80, 2 * DIS_Y, "        ");
                // 显示新角度
                showfloat(80, 2 * DIS_Y, current_angle, 4, 2);
                last_display_angle = current_angle;
            }

            system_delay_ms(20);  // 主循环延迟,与button_entry一致
        }
    }
}

// ==================== 旧的配置函数已移除 ====================
// servo_save_config() 和 servo_load_config() 已移除
// 现在统一使用 config_flash.cpp 中的 config_save() 和 config_load()

