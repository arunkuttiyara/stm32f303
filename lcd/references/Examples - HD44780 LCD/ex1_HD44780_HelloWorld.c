/*#############################################################
File Name       : ex1_HD44780_HelloWorld.c
Author          : Grant Phillips
Date Modified   : 17/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description     : Example program that writes "Hello World!" to 
									a HD44780-based character LCD.       

Requirements    : * STM32F3-Discovery Board
									* Make sure the HD44780_F3.c file is in the 
                    same folder than the project
									* Define the custom configurations on line 80 
										to 112 in the HD44780_F3.c file.
              
Circuit         : * The HD44780-based character LCD is connected 
										as follows (line 92 to 110 in HD44780_F3.c):
										Vss (LCD pin 1)				-	GND
										Vdd (LCD pin 2)				-	3.3V
										Vee (pin3)						-	Contrast circuit
										RS  (pin4)						-	PD0
										RW  (pin5)						-	PD1
										E   (pin6)						-	PD2
										DB0 (pin7)						-	
										DB1 (pin8)						-	
										DB2 (pin9)						-	
										DB3 (pin10)						-	
										DB4 (pin11)						-	PD4
										DB5 (pin12)						-	PD5
										DB6 (pin13)						-	PD6
										DB7 (pin14)						-	PD7
									* The LCD will be setup for 4-bit mode (line
										84), 16 columns (line 88) and 2 rows (line
										89)

See the STM32F3-Discovery User Manual (UM1570) for the block 
diagram of the STM32F303VCT6 processor (p13), a summary of
the GPIO connections (p21-29) and the schematic diagram (p31-34)

##############################################################*/

/* #includes -------------------------------------------------*/
#include "main.h"																								//main library to include for device drivers, peripheral drivers, etc.
#include "HD44780_F3.c"																					//for the HD44780_F3 character lcd functions on the STM32F3-Discovery

/* #defines --------------------------------------------------*/

/* #function prototypes --------------------------------------*/

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
		while(1);																										//will end up in this infinite loop if there was an error with Systick_Config
	
	HD44780_Init();												//initialize the lcd
	
	HD44780_PutStr("Hello World!");				//print text
	
	/* Main program loop */
  while (1)
  {
		//do nothing
  }	
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



