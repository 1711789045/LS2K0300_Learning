#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "zf_common_headfile.h"

// 控制标志位
extern uint8 go_flag,stop_flag,stop_time;

// 速度控制参数
extern int16 speed;       // 目标速度
extern uint8 start_time;  // 启动计时

// 差速控制参数
extern float dif_speed_reduce;  // 差速减速系数
extern float dif_speed_plus;    // 差速加速系数

#endif
