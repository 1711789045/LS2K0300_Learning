#include "zf_common_headfile.h"

// 控制标志位
uint8 go_flag = 0,stop_flag = 0,stop_time = 0;

// 速度控制参数
int16 speed = 2000;  // 目标速度
uint8 start_time = 0;  // 启动计时

// 差速控制参数
float dif_speed_reduce = 0.3;  // 差速减速系数
float dif_speed_plus = 0.3;    // 差速加速系数
