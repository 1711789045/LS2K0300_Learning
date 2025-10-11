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
#include "pid.h"
#include "image.h"
#include "control.h"
#include <stdlib.h>
#include <math.h>

// PWM 信息结构体
static struct pwm_info servo_pwm_info;

// PID 控制器
static PID_POSITIONAL_TypeDef turn_pid = {0};

// 舵机PID参数（可通过菜单调节）
float kp = 0.35;
float ki = 0;
float kd1 = 0.56;
float kd2 = 0;

// 舵机控制变量
float angle = 0;
float servo_pid_kp = 0.35;
float servo_pid_ki = 0;
float servo_pid_kd1 = 0.56;
float servo_pid_kd2 = 0;
uint8 servo_f = 0;
float last_err = 0;

// 占空比计算宏（根据角度计算PWM占空比）
// 舵机 0-180度对应 0.5ms-2.5ms 高电平
#define SERVO_MOTOR_DUTY(x) ((float)servo_pwm_info.duty_max / (1000.0 / (float)servo_pwm_info.freq) * (0.5 + (float)(x) / 90.0))

/**
 * @brief  舵机初始化
 * @param  无
 * @return 无
 */
void servo_init(void)
{
    // 获取舵机PWM设备信息
    pwm_get_dev_info(SERVO_MOTOR_PWM, &servo_pwm_info);

    // 打印PWM信息（调试用）
    printf("Servo PWM freq = %d Hz\r\n", servo_pwm_info.freq);
    printf("Servo PWM duty_max = %d\r\n", servo_pwm_info.duty_max);

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
    float servo_motor_duty = SERVO_MOTOR_MID + angle;

    // 限制在左右最大角度范围内
    if(servo_motor_duty >= SERVO_MOTOR_R_MAX)
        servo_motor_duty = SERVO_MOTOR_R_MAX;
    if(servo_motor_duty <= SERVO_MOTOR_L_MAX)
        servo_motor_duty = SERVO_MOTOR_L_MAX;

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
        servo_set_pid(kp, ki, kd1, kd2);

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
