#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

extern int state;
uint32_t step=0;
int step_up(int steps,int32_t clock)
{
	step_vale=abs(steps);
	for(int i=1;i<step_value;i++){
	
		if(5==state)
			state=1;
		else if(0==state)
			state=4;

		if(steps>0)
			state++;
		else
			state--;
		
		switch(state)
		{
			case 1 :
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
				delay(clock);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
				delay(clock);
				break;
			case 2 :
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
				delay(clock);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
				delay(clock);
				break;
			case 3:  
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
				delay(clock);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
				delay(clock);
				break;
			case 4:  
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
				delay(clock);
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
				delay(clock);
				break;
			default: 
				break;
		}
	}
	return 0;
}

void stepper_cmd(int mode)
{
	int rc=0;
	int32_t steps=0;
	uint32_t clock; 
	if (mode!=CMD_INTERACTIVE) return;


	rc = fetch_int32_arg(&steps);
	if(rc) {
		printf("missing : no of steps \n");
		return;
	}

	rc = fetch_uint32_arg(&clock);
	if(rc) {
		printf("missing : clk signals to wait between steps missing \n");
		return;
	}
}
ADD_CMD("stepper",stepper_cmd,"          give 1. signed no for direction+no of steps 2. delay between clock steps")


