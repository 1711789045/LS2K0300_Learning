#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "zf_common_headfile.h"

// ==================== 控制模块配置参数声明 ====================
// 这些参数在 control.cpp 中定义，可被 config_flash 等模块引用

// 控制标志位
extern uint8 go_flag,stop_flag,stop_time;

// 速度控制参数
extern int16 speed;       // 目标速度
extern uint8 start_time;  // 启动计时

// 差速控制参数
extern float dif_speed_reduce;  // 差速减速系数
extern float dif_speed_plus;    // 差速加速系数

// 发车和停车函数
void car_start(void);   // 发车函数
void car_stop(void);    // 停车函数（处理3秒减速+关闭硬件）

#endif
