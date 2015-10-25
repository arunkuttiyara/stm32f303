/*#############################################################
File Name       : ex2_HD44780_char_functions.c
Author          : Grant Phillips
Date Modified   : 17/10/2013
Compiler        : Keil ARM-MDK (uVision V4.70.0.0)
Tested On       : STM32F3-Discovery

Description     : Example program that demonstrates the use of
                  the character functions in the HD44780_F3.c
									library (HD44780_ClrScr(), HD44780_GotoXY(), 
									HD44780_PutChar(), HD44780_PutStr()) and 
                  the use of string function sprintf().      

Requirements    : * STM32F3-Discovery Board
									* Make sure the HD44780_F3.c file is in the 
                    same folder than the project
									* Define the custom configurations on line 80 
										to 112 in the HD44780_F3.c file.
              
Circuit         : * The HD44780-based LCD is connected and setup
										as described in ex1_HD44780_HelloWorld.c

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
	char buf[21];                    				//declare a string variable of 21 chars   
  unsigned char i;
	
	/* Set the SysTick Interrupt to occur every 1ms) */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
		while(1);																										//will end up in this infinite loop if there was an error with Systick_Config
	
	HD44780_Init();												//initialize the lcd
		
	/* Main program loop */
  while (1)
  {
		HD44780_ClrScr();								  		//clear the display
		Delay(1000);
		HD44780_GotoXY(0,0);									//set the character cursor to col=0, row=0
		HD44780_PutStr("Hello World!");				//display text
		Delay(500);
		
		HD44780_GotoXY(0,1);									//set the character cursor to col=0, row=1
		for(i=1;i<=8;i++)
    {
			sprintf(buf, "%d " , i);   					//print a integer value and space to the buf string
      HD44780_PutStr(buf);            		//write a string variable to the LCD
      Delay(500);
    }
		Delay(1000);
		
		HD44780_ClrScr();								  		//clear the display
		sprintf(buf, "Good Bye");  						//store a string constant into the buf string variable
    HD44780_GotoXY(0,0);									//set the character cursor to col=0, row=0 
    for(i=0;i<=7;i++)
    {
       HD44780_PutChar(buf[i]);      			//display a character on the LCD
       Delay(500);
    }
		
		HD44780_GotoXY(4,1);									//set the character cursor to col=4, row=1  
    for(i=65;i<=72;i++)
    {
      HD44780_PutChar(i);  								//display a integer value as a byte onn the LCD
																					//driver will convert it to the correpsonding ASCII char
      Delay(500);
    }
		Delay(1000);
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



