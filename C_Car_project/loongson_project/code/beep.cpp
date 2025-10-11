#include "zf_common_headfile.h"
#include "beep.h"

uint8 beep_flag = 0;

void beep_on(void){
	gpio_set_level(BEEP, 0x1);                                            // BEEP Ïì

}

void beep_off(void){
	gpio_set_level(BEEP, 0x0);                                             // BEEP Í£

}
