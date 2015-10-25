/*#############################################################
File Name       : ex1_HC-SR04_measure_swv.c
Author          : Grant Phillips
Date Modified   : 11/09/2014
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description     : Example program that demonstrates the use of
                  an HC-SR04 ultrasonic distance sensor.
									
									The output is displayed on a PC using the
									Serial Wire Viewer (SWV).  See the SWV examples
									for an explanation of the use of this feature.

Requirements    : * STM32F3-Discovery Board
              
Circuit         : * A wire link between PB3 and pin6 of the SWD
										connector (CN3) as explained in the SWV
										examples section
									* The HC-SR04 ultrasonic sensor connected as:
										VCC		-		+5V
										TRIG	-		PB4
										ECHO	-		PD4
										GND		-		GND
										
																		*NOTE*
									Servo1 - 4 will now not be availble due to it
									sharing resources with the driver for the 
									HC-SR04.  Servo5 - 12 will still be available.

See the STM32F3-Discovery User Manual (UM1570) for the block 
diagram of the STM32F303VCT6 processor (p13), a summary of
the GPIO connections (p21-29) and the schematic diagram (p31-34)

##############################################################*/

/* #includes -------------------------------------------------*/
#include "main.h"																								//main library to include for device drivers, peripheral drivers, etc.

/* #defines --------------------------------------------------*/

/* #function prototypes --------------------------------------*/
void HC_SR04_Init(void);																				//function prototype for a user funtion HC_SR04_Init()

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
  uint32_t distance;																	//declase a 32-bit unsigned int to hold the distance value
	uint32_t mm;																				//32-bit unsigned int to hold the distance value in mm
	unsigned int dec, frac;															//used for converting the integer distance to a float value for printing
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																					//will end up in this infinite loop if there was an error with Systick_Config
	
	HC_SR04_Init();																			//initialize the HC-SR04 ultrasonic sensor - see the function definition below
		
	/* Main program loop */
  while (1)
  {
		distance = (TIM_GetCapture1(TIM2)/72);						//read the TIM2 value and divide by 72 (72Mhz); the result is in us.
		
		printf("%d us     " , distance);   					    

		//HC-SR04 formula: distance (in m) = pulse in seconds * 340 / 2
		mm = distance * 340 / 2;													//mm is in micro meters (because distance is in us), e.g. if distance = 728us, then mm = 123760um
		
		dec = mm / 1000;																	//calculate the integer part, e.g. 123
		frac = mm - (dec * 1000);													//calculate the fraction part, e.g. 760
		printf("%d.%03d mm\n" , dec, frac);   					//%03d prints a integer value in a field of 3 characters with leading 0's
		
    Delay(10);
  }	
}



/*
Retarget the C library printf function to the SWO Viewer.
Overwrites int fputc(int ch, FILE *f) function used by printf.
*/								
int fputc(int ch, FILE *f)
{
	return(ITM_SendChar(ch));
}



void HC_SR04_Init(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;								//structure for GPIO setup
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;						//structure for TIM Time Base
	TIM_OCInitTypeDef				TIM_OCInitStructure;							//structure for TIM Output Compare
	TIM_ICInitTypeDef  			TIM_ICInitStructure;							//structure for TIM Input Compare
	
	/* ===== Configure the TRIG pin =====
	
	PB4 will be used as the TRIGGER pin in this example.  It will be used in PWM mode and will generate
	a 10us HIGH pulse every 60ms to trigger the HC-SR04 to do a measurement.  PB4 is shared TIM16_CH1
	(Timer 16 Channel 1) and also TIM3_CH1 (see the STM32F3-Discovery User Manual p24).  We will use
	TIM16 in this example, seeing that the SERVO_F3.c driver make use of TIM3 for most of the servo outputs.
	Also see Chpt 18 on General Purpose Timers (TIM15/16/17) in the STM32F303xx Reference Manual (RM0316) for
	more detail on the use of TIM16.
	
	Please note that any other free pin that shares a TIM could be used on the STM32F3-Discovery board.  Just
	make sure that the pin is not used by any of the onboard peripherals (p21-29 in the User Manual).	
	*/
	

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);				//enable the AHB Peripheral Clock to use GPIOB
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);			//enable the TIM16 clock
	
	/* Pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;									//configure PB4 in alternate function pushpull mode
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;							
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						
  GPIO_Init(GPIOB, &GPIO_InitStructure);											
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM16 (GPIO_AF_1)
	
	/* Time base configuration */ 
	//The MCU on the STM32F3-Discovery board is clocked at 72Mhz and we need a 16.667Hz timebase (60ms period)
	//Update_event = TIM_CLK /((PSC + 1)*(ARR   + 1)*(RCR + 1))
	//			16.667 = 72000000/((71  + 1)*(60000 + 1)*(0   + 1))
  TIM_TimeBaseStructure.TIM_Prescaler = 71;  								//PSC value; meanse that TIM register will be incremented every 1us (72000000 / (71+1) = 1000000Hz = 1us
  TIM_TimeBaseStructure.TIM_Period = 60000;                 //ARR value; create a time period of 60ms (1us * 60000 = 60ms)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);

	/* Output Control (OC) configuration - PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					//configure the PWM
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM_OC1Init(TIM16, &TIM_OCInitStructure);									//for other channels this would be changed to TIM_OC2Init..., or TIM_OC3Init etc.
	
	TIM_CtrlPWMOutputs(TIM16, ENABLE);												//enable the PWM output
	TIM_Cmd(TIM16, ENABLE);																		//enable TIM16
	
	//set the pulse width to 10us (10 * 1us = 10us as described in the the timebase section)
	TIM_SetCompare1(TIM16, 10);																//for other channels this would be changed to TIM_SetCompare2..., or TIM_SetCompare3 etc.
	
	
	
	/* ===== Configure the ECHO pin =====
	PD4 will be used as the ECHO pin in this example.  It will be used in PWM INPUT mode and will use a counter
	tied to a timer to determine the pulse width of the signal from the HC-SR04 which represent the distance.
	PD4 is shared with TIM2_CH2 and USART2_RTS (see the STM32F3-Discovery User Manual p23).  We will use TIM2 in 
	this example, seeing that TIM2 is a general purpose timer (easy to use) and is 32-bits (to hold a large value
	for long pulses).  Also see Chpt 17 on General Purpose Timers (TIM2/3/4) in the STM32F303xx Reference Manual 
	RM0316) for	more detail on the use of TIM2 as PWM INPUT mode.  The TIM2 register value will be updated every
	60ms with a new value for the resultant distance.  A read from the TIM2 register will thus provide the latest
	distance measured.  Not software code is required to trigger the sensor and to measure the pulse, it is all
	done by using a PWM output (TRIG) and a PWM input (ECHO) and using the associated hardware peripherals.
	
	Please note that any other free pin that shares a TIM could be used on the STM32F3-Discovery board.  Just
	make sure that the pin is not used by any of the onboard peripherals (p21-29 in the User Manual).	
	*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//enable the TIM2 clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);				//enable the AHB Peripheral Clock to use GPIOD
  
	/* Pin configuration */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;								//configure PD4 in alternate function pushpull mode
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_2);			//connect the pin to the desired peripherals' Alternate Function (AF) - TIM2 (GPIO_AF_2)
	
	/* Input Control (IC) configuration - PWM1 Mode configuration: Channel1 */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;					//configre TIM2 for input-PWM mode
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);

	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);

  TIM_Cmd(TIM2, ENABLE);																		//enable TIM2
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



