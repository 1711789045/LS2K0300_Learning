#include "zf_common_headfile.h"
#include "Header.h"

timer_fd *pit_10ms_timer;
timer_fd *pit_100ms_timer;

// 10ms定时器回调函数 - 快速控制任务
void pit_10ms_callback()
{
    // 舵机控制标志位（10ms周期）
    servo_f = 1;

    // 电机控制标志位（10ms周期）
    motor_f = 1;

    // 编码器读取标志位（10ms周期）
    encoder_f = 1;

    // 启动计时器递增
    if(go_flag && start_time < 255)
        start_time++;

    // 停止计时器递增
    if(stop_flag && stop_time < 255)
        stop_time++;

    // 环岛计时器递增
    if(circle_flag > 0 && circle_flag < 5 && circle_time < 65535)
        circle_time++;
}

// 100ms定时器回调函数 - 慢速任务
void pit_100ms_callback()
{
    // 蜂鸣器控制
    if(beep_flag){
		beep_on();
		beep_flag--;
	}
	else
		beep_off();

	// 打印动态前瞻状态
	if(dynamic_weight_enable){
		const char* status_str;
		switch(dynamic_weight_status){
			case 0: status_str = "FAR "; break;  // 远前瞻（直道）
			case 1: status_str = "MID "; break;  // 中前瞻（缓弯）
			case 2: status_str = "NEAR"; break;  // 近前瞻（急弯）
			default: status_str = "????"; break;
		}
		printf("[Dynamic Weight] Status: %s, Curvature: %.1f\n", status_str, current_curvature);
	} else {
		printf("[Dynamic Weight] DISABLED (Fixed Weight: %d)\n", mid_weight_select);
	}
}

void pit_init()
{
    pit_10ms_timer = new timer_fd(10, pit_10ms_callback);
    pit_100ms_timer = new timer_fd(100, pit_100ms_callback);
}


