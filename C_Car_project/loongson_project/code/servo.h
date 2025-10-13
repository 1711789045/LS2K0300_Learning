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

#ifndef __SERVO_H__
#define __SERVO_H__
#include "zf_common_headfile.h"

// 定义舵机PWM设备路径
#define SERVO_MOTOR_PWM             "/dev/zf_device_pwm_servo"

// 舵机控制范围限制（度）
#define SERVO_MOTOR_LIMIT           (15)      // 最大偏转角度限制
#define SERVO_MOTOR_MID             (69.7)    // 中值角度

// 函数声明
void servo_init(void);
void servo_setangle(float angle);
void servo_set_pid(float kp, float ki, float kd1, float kd2);
void servo_control(uint8 mid_line);
void servo_process(void);
void servo_manual_adjust(void);  // 舵机手动调整功能
void servo_save_config(void);    // 保存舵机配置到文件
void servo_load_config(void);    // 从文件读取舵机配置

// 外部变量声明
extern float angle;
extern uint8 servo_f;
extern float g_servo_mid;  // 全局舵机中值变量

// 舵机PID参数（用于菜单调节）
extern float kp;
extern float ki;
extern float kd1;
extern float kd2;

#endif
