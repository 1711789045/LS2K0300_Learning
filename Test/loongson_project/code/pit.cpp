#include "zf_common_headfile.h"
#include "beep.h"

timer_fd *pit_10ms_timer;
timer_fd *pit_100ms_timer;

void pit_10ms_callback()
{
    printf("pit_10ms_callback\r\n");
}

void pit_100ms_callback()
{
    if(beep_flag){
		beep_on();
		beep_flag--;
	}
	else
		beep_off();
    printf("pit_100ms_callback\r\n");
}

void pit_init()
{
    pit_10ms_timer = new timer_fd(10, pit_10ms_callback);
    pit_100ms_timer = new timer_fd(100, pit_100ms_callback);
}


