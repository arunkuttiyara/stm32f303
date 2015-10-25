/* file which contains command to change dc motor speed */

#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"
#include<stdlib.h>

void dc_speed(int channel)
{
	printf("changing the dc motor value to %d \n",(unsigned int )channel);
	TIM15->CCR1 = channel;		
}

void dc_cmd(int mode)
{
	uint32_t channel=0;
	if (mode!=CMD_INTERACTIVE) return;
	fetch_uint32_arg(&channel);
	printf("changing the dc motor value to %d \n",(unsigned int )channel);
	TIM15->CCR1 = channel;		
}
ADD_CMD("dc_speed",dc_cmd,"dc_speed speed : speed varies between 0-1000 ")


