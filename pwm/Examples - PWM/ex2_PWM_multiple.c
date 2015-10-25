/*#############################################################
File Name       : ex2_PWM_multiple.c
Author          : Grant Phillips
Date Modified   : 16/03/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description     : Example program that demonstrates the use of
                  multiple PWM outputs.  Each PWM output will
									individually be ramped up from 0 to 100%.  The
									signals will be 25% out of phase for demon-
									stration purposes.

Requirements    : * STM32F3-Discovery Board
              
Circuit         : * LEDs connected to:
									PB6		(TIM4_CH1)
									PD13	(TIM4_CH2)
									PD14	(TIM4_CH3)
							
									* Connect the PWM outputs to an oscilloscope 
										to see the effects graphically
										
																		*NOTE*
									Servo9 - 12 will now not be availble due to it
									sharing resources with TIM4.

See the STM32F3-Discovery User Manual (UM1570) for the block 
diagram of the STM32F303VCT6 processor (p13), a summary of
the GPIO connections (p21-29) and the schematic diagram (p31-34)

##############################################################*/

/* #includes -------------------------------------------------*/
#include "main.h"																								//main library to include for device drivers, peripheral drivers, etc.

/* #defines --------------------------------------------------*/

/* #function prototypes --------------------------------------*/
void PWM_Init(void);																						//function prototype for a user funtion PWM_Init()

/* #global variables -----------------------------------------*/
GPIO_InitTypeDef        GPIO_InitStructure;											//structure used for setting up a GPIO port
RCC_ClocksTypeDef 			RCC_Clocks;															//structure used for setting up the SysTick Interrupt



// Unused global variables that have to be included to ensure correct compiling */
// ###### DO NOT CHANGE ######
// ===============================================================================
__IO uint32_t TimingDelay = 0;																	//used with the Delay function
__IO uint8_t DataReady = 0;
__IO uint32_t USBConnectTimeOut = 100;
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t PrevXferComplete = 1;
// ===============================================================================



int main(void)
{
	__IO uint16_t LED1dc=25, LED2dc=50, LED3dc=10;			//8bit variable to store duty cycle % for each LED
																											//__IO forces the compiler not to "optimize" this as an constant, but force
																											//it as an variable
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																					//will end up in this infinite loop if there was an error with Systick_Config
	
	PWM_Init();																					//initialize the PWM module
	

	/* Main program loop */
  while (1)
  {
		//duty cycle = pos pulse width/period
		//i.e. pos pulse width = duty cycle * period
		
		//Channel - set the duty
		TIM_SetCompare1(TIM4,(50 * LED1dc)/100);	
		//Channe2 - set the duty
		TIM_SetCompare2(TIM4,(50 * LED2dc)/100);
		
		
		//Channe3 - set the duty
		TIM_SetCompare3(TIM4, (50 * LED3dc)/100);
		
		
		LED1dc++;																					//increment LED1 duty cycle %
		if (LED1dc>100)
			LED1dc=0;																				//reset to 0 if > 100
		
		LED2dc++;																					//increment LED2 duty cycle %
		if (LED2dc>100)
			LED2dc=0;																				//reset to 0 if > 100

		LED3dc++;																					//increment LED3 duty cycle %
		if (LED3dc>100)
			LED3dc=0;																				//reset to 0 if > 100

		Delay(100);
  }	
}



void PWM_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;								//structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;						//structure for TIM Time Base
	TIM_OCInitTypeDef				TIM_OCInitStructure;							//structure for TIM Output Compare
	
	/* ===== Configure the PWM pin =====
	See description in ex1_PWM_ramp_up.c	
	*/
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);				//enable the AHB Peripheral Clock to use GPIOB
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);				//enable the AHB Peripheral Clock to use GPIOD
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//enable the TIM4 clock
	
	/* Pin configuration for PB6 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;									//configure PB6 in alternate function pushpull mode
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						
  GPIO_Init(GPIOB, &GPIO_InitStructure);											
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_2);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM4 (GPIO_AF_2)
	
	/* Pin configuration for PD13 and PD14 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;	//configure PB13 and PD14 in alternate function pushpull mode
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						
  GPIO_Init(GPIOD, &GPIO_InitStructure);											
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM4 (GPIO_AF_2)
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_2);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM4 (GPIO_AF_2)
	
	/* Time base configuration */ 
	//The MCU on the STM32F3-Discovery board is clocked at 72Mhz and we need a 20kHz timebase (50us period)
	//Update_event = TIM_CLK /((PSC + 1)*(ARR   + 1)*(RCR + 1))
	//			 20000 = 72000000/((71  + 1)*(49    + 1)*(0   + 1))
  TIM_TimeBaseStructure.TIM_Prescaler = 71;  								//PSC value; meanse that TIM register will be incremented every 1us (72000000 / (71+1) = 1000000Hz = 1us
  TIM_TimeBaseStructure.TIM_Period = 49;                 		//ARR value - 1; create a time period of 50us (1us * 50 = 50us)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Output Control (OC) configuration - PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					//configure the PWM
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);									//for channel 1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);									//for channel 2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);									//for channel 3
	
	TIM_CtrlPWMOutputs(TIM4, ENABLE);													//enable the PWM output
	TIM_Cmd(TIM4, ENABLE);																		//enable TIM4
}











// -------------------------------------------------------------------------------

// Function to insert a timing delay of nTime
// ###### DO NOT CHANGE ######
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

// Function to Decrement the TimingDelay variable.
// ###### DO NOT CHANGE ######
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

// Unused functions that have to be included to ensure correct compiling
// ###### DO NOT CHANGE ######
// =======================================================================
uint32_t L3GD20_TIMEOUT_UserCallback(void)
{
  return 0;
}

uint32_t LSM303DLHC_TIMEOUT_UserCallback(void)
{
  return 0;
}
// =======================================================================



