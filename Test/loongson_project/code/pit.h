#ifndef __PIT_H__
#define __PIT_H__
#include "zf_common_headfile.h"

extern timer_fd *pit_10ms_timer;
extern timer_fd *pit_100ms_timer;
void pit_init();

#endif