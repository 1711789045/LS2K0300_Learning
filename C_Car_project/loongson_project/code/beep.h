#ifndef __BEEP_H__
#define __BEEP_H__
#include "zf_common_headfile.h"

#define BEEP "/dev/zf_driver_gpio_beep"

extern uint8 beep_flag;

void beep_on(void);
void beep_off(void);

#endif
