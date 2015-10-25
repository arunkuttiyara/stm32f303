/*#############################################################
File Name       : ex1_PWM_ramp_up.c
Author          : Grant Phillips
Date Modified   : 16/03/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description     : Example program that demonstrates the use of
                  an PWM output.  The output will be cycled from
									0%, 25%, 50%, 75% and 100% every 5 seconds. If
									and LED or DC motor driver are connected to the
									pin, you will see the LED go from dim to bright
									and the DC motor turn slow to fast.

Requirements    : * STM32F3-Discovery Board
              
Circuit         : * An LED or DC motor driver connected to PB6
									* Connect PB6 to an oscilloscope to see the 
									  effect of an PWM output graphically
										
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
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																					//will end up in this infinite loop if there was an error with Systick_Config
	
	PWM_Init();																					//initialize the PWM module
	
	//set the duty cycle to 0%
	TIM_SetCompare1(TIM4,0);														//for other channels this would be changed to TIM_SetCompare2..., or TIM_SetCompare3 etc.								
		
	/* Main program loop */
  while (1)
  {
		//set the duty cycle to 25% 
		//duty cycle = pos pulse width/period
		//       0.5 = 12.5us         /50us to 10us
		//pulse width value cannot use a comma, so must be rounded off
		//i.e. pos pulse width = 13us (13 * 1us = 13us as described in the the PWM_Init() timebase section)
		TIM_SetCompare1(TIM4, 13);
		Delay(5000);
		
		//set the duty cycle to 50% 
		TIM_SetCompare1(TIM4, 25);		//(25us / 50us = 0.5)
		Delay(5000);
		
		//set the duty cycle to 75% 
		TIM_SetCompare1(TIM4, 38);		//(38us / 50us = 0.76)
		Delay(5000);
		
		//set the duty cycle to 100% 
		TIM_SetCompare1(TIM4, 50);		//(50us / 50us = 1.0)
		Delay(5000);
		
		//set the duty cycle to 0% 
		TIM_SetCompare1(TIM4, 0);			//(0us / 50us = 0.0)
		Delay(5000);
  }	
}



void PWM_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;								//structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;						//structure for TIM Time Base
	TIM_OCInitTypeDef				TIM_OCInitStructure;							//structure for TIM Output Compare
	
	/* ===== Configure the PWM pin =====
	
	PB6 will be used as the PWM pin in this example.  PB6 is shared TIM4_CH1 (Timer 4 Channel 1) and also 
	TIM8_CH1 (see the STM32F3-Discovery User Manual p24).  We will use TIM4 in this example, seeing that it
	is a General Purpose Timer and is easy to work with. Also see Chpt 18 on General Purpose Timers (TIM15/16/17) 
	in the STM32F303xx Reference Manual (RM0316) for more detail on the use of TIM4.  This section sets the 
	timebase for TIM4 to 20Khz, and that will be the same for channel 2, 3 and 4.  Often when DC motors get
	driven by a PWM signal, one can hear a "whine" sound coming from the DC motor which is caused by the frequency
	of the PWM signal.  For this reason the timebase will be set to 20Khz, which will make the signal fall outside
	the normal hearing spectrum of a human.  You can make this higher if you want.
	
	Please note that any other free pin that shares a TIM could be used on the STM32F3-Discovery board.  Just
	make sure that the pin is not used by any of the onboard peripherals (p21-29 in the User Manual).	
	*/
	

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);				//enable the AHB Peripheral Clock to use GPIOB
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//enable the TIM4 clock
	
	/* Pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;									//configure PB6 in alternate function pushpull mode
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						
  GPIO_Init(GPIOB, &GPIO_InitStructure);											
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_2);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM4 (GPIO_AF_2)
	
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
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);									//for other channels this would be changed to TIM_OC2Init..., or TIM_OC3Init etc.
	
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



