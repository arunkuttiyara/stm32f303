
/*
    
     lcd library functions 
     Library functions are based on  code developed by Mr Grant Philphs
 */




#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_accelerometer.h"
#include "stm32f3_discovery_gyroscope.h"
#include <math.h>



/* COMMANDS */
#define HD44780_CMD_RESET            	0x30     	/*!< Resets display - used in init 3x */
#define HD44780_CMD_CLEAR            	0x01     	/*!< Clears display */
#define HD44780_CMD_RETURN_HOME      	0x02     	/*!< Sets DDRAM pointer to 0 */
#define HD44780_CMD_ENTRY_MODE       	0x04     	/*!< Sets how the pointer is updated after a character write */
#define HD44780_CMD_DISPLAY          	0x08     	/*!< Display settings */
#define HD44780_CMD_SHIFT            	0x10     	/*!< Cursor and display movement */
#define HD44780_CMD_FUNCTION         	0x20     	/*!< Screen type setup */
#define HD44780_CMD_CGRAM_ADDR       	0x40     	/*!< Sets CGRAM address */
#define HD44780_CMD_DDRAM_ADDR       	0x80     	/*!< Sets DDRAM address */

/* ENTRY_MODE Command parameters */
#define HD44780_ENTRY_SHIFT_DISP 			0x01	 		/*!< Shift display */
#define HD44780_ENTRY_SHIFT_CURS 			0x00	 		/*!< Shift cursor */
#define HD44780_ENTRY_ADDR_INC   			0x02     	/*!< Increments pointer */
#define HD44780_ENTRY_ADDR_DEC   			0x00	 		/*!< Decrements pointer */

/* DISPLAY Command parameters */
#define HD44780_DISP_ON       				0x04      /*!< Enables the display */
#define HD44780_DISP_OFF      				0x00      /*!< Disables the display */
#define HD44780_DISP_CURS_ON  				0x02      /*!< Enables cursor */
#define HD44780_DISP_CURS_OFF 				0x00      /*!< Disables cursor */
#define HD44780_DISP_BLINK_ON					0x01      /*!< Enables cursor blinking */
#define HD44780_DISP_BLINK_OFF  			0x00      /*!< Disables cursor blinking */

/* SHIFT Command parameters */
#define HD44780_SHIFT_DISPLAY    			0x08      /*!< Shifts the display or shifts the cursor if not set */
#define HD44780_SHIFT_CURSOR    			0x00      /*!< Shifts the display or shifts the cursor if not set */
#define HD44780_SHIFT_RIGHT      			0x04      /*!< Shift to the right */
#define HD44780_SHIFT_LEFT      			0x00      /*!< Shift to the left  */

/* FUNCTION Command parameters */
#define HD44780_FUNC_BUS_8BIT  				0x10      /*!< 8 bit bus */
#define HD44780_FUNC_BUS_4BIT  				0x00      /*!< 4 bit bus */
#define HD44780_FUNC_LINES_2   				0x08      /*!< 2 lines */
#define HD44780_FUNC_LINES_1   				0x00      /*!< 1 line */
#define HD44780_FUNC_FONT_5x10 				0x04      /*!< 5x10 font */
#define HD44780_FUNC_FONT_5x8  				0x00      /*!< 5x8 font */

/* Busy Flag - actually not used */
#define HD44780_BUSY_FLAG        			0x80      /*!< Busy flag */



#define HD44780_CONF_BUS							HD44780_FUNC_BUS_4BIT
//#define HD44780_CONF_BUS							HD44780_FUNC_BUS_8BIT
#define HD44780_CONF_LINES						HD44780_FUNC_LINES_2
#define HD44780_CONF_FONT							HD44780_FUNC_FONT_5x8

#define HD44780_DISP_LENGTH						16
#define HD44780_DISP_ROWS							2
#define HD44780_CONF_SCROLL_MS				20

/* HD44780 Data lines - use the same port for all the lines */
#define HD44780_DATAPORT							GPIOD
#define HD44780_DATABIT0							GPIO_PIN_8	//not used in 4-bit mode
#define HD44780_DATABIT1							GPIO_PIN_9	//not used in 4-bit mode
#define HD44780_DATABIT2							GPIO_PIN_10	//not used in 4-bit mode
#define HD44780_DATABIT3							GPIO_PIN_11	//not used in 4-bit mode
#define HD44780_DATABIT4							GPIO_PIN_12
#define HD44780_DATABIT5							GPIO_PIN_13
#define HD44780_DATABIT6							GPIO_PIN_14
#define HD44780_DATABIT7							GPIO_PIN_15

/* HD44780 Control lines - use the same port for all the lines */
#define HD44780_CONTROLPORT						GPIOD
#define HD44780_RS_BIT								GPIO_PIN_3
#define HD44780_RW_BIT								GPIO_PIN_4
#define HD44780_EN_BIT								GPIO_PIN_7

/* defines the peripheral clocks that need to be enabled for the above pins */
#define HD44780_RCC_AHBPeriph					RCC_AHBPeriph_GPIOD		//or something like "RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOD"


void hd44780_Delay(unsigned long del)
{
	unsigned long i=0;

	while(i<del)
		i++;
}

#define hd44780_RS_On()                     HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RS_BIT, GPIO_PIN_SET)     
#define hd44780_RS_Off()                        HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RS_BIT, GPIO_PIN_RESET)
#define hd44780_RW_On()                     HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RW_BIT, GPIO_PIN_SET)
#define hd44780_RW_Off()                    HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RW_BIT, GPIO_PIN_RESET)
#define hd44780_EN_On()                     HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_EN_BIT, GPIO_PIN_SET)
#define hd44780_EN_Off()                    HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_EN_BIT, GPIO_PIN_RESET)
#define hd44780_init_delay()  hd44780_Delay(127900);	//16ms


#define hd44780_EN_high_delay()     	hd44780_Delay(20000)//4095)
#define hd44780_init_delay2()       	hd44780_Delay(51400)	//5ms
#define hd44780_init_delay3()       	hd44780_Delay(10280)	//1ms
#define hd44780_init_end_delay()    	hd44780_Delay(20560)	//2ms


#define hd44780_clear()                       	  hd44780_wr_cmd( HD44780_CMD_CLEAR )
#define hd44780_home()                        	  hd44780_wr_cmd( HD44780_CMD_RETURN_HOME )
#define hd44780_entry( inc_dec, shift )           hd44780_wr_cmd( ( HD44780_CMD_ENTRY_MODE | inc_dec | shift ) & 0x07 )
#define hd44780_display( on_off, cursor, blink )  hd44780_wr_cmd( ( HD44780_CMD_DISPLAY | on_off | cursor | blink ) & 0x0F )
#define hd44780_shift( inc_dec, shift )           hd44780_wr_cmd( ( HD44780_CMD_SHIFT | inc_dec | shift ) & 0x1F )
#define hd44780_function( bus, lines, font )      hd44780_wr_cmd( ( HD44780_CMD_FUNCTION | bus | lines | font ) & 0x3F )
#define hd44780_cgram_addr( addr )                hd44780_wr_cmd( HD44780_CMD_CGRAM_ADDR | ( addr & 0x3F ) )
#define hd44780_ddram_addr( addr )                hd44780_wr_cmd( HD44780_CMD_DDRAM_ADDR | ( addr & 0x7F ) )
#define hd44780_write_char( c )                   hd44780_wr_data( c & 0xff )


void hd44780_hi_nibble_write( unsigned char data )
{
	if ( data & 0x10 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
	}
	if ( data & 0x20 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
	}
	if ( data & 0x40 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	}
	if ( data & 0x80 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_RESET);
	}

    /* set the EN signal */
    hd44780_EN_On();

    /* wait */
    hd44780_EN_high_delay();

    /* reset the EN signal */
    hd44780_EN_Off();
}

void hd44780_low_nibble_write( unsigned char data )
{
	if ( data & 0x01 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
	}
	if ( data & 0x02 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
	}
	if ( data & 0x04 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	}
	if ( data & 0x08 ) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_RESET);
	}

    /* set the EN signal */
    hd44780_EN_On();

    /* wait */
    hd44780_EN_high_delay();

    /* reset the EN signal */
    hd44780_EN_Off();
}

/* 4bit write  */
void hd44780_write( unsigned char data )
{
	/* send the data bits - high nibble first */
	hd44780_hi_nibble_write( data );
	hd44780_low_nibble_write( data );
}


void hd44780_wr_cmd( unsigned char cmd )
{
	hd44780_RS_Off();
	//HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RS_BIT, GPIO_PIN_RESET);
	hd44780_write( cmd );
}


void hd44780_wr_data( unsigned char data )
{
	hd44780_RS_On();
	//HAL_GPIO_WritePin(HD44780_CONTROLPORT, HD44780_RS_BIT, GPIO_PIN_SET);	
	hd44780_write( data );
}



void HD44780_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	
	/* Configure the HD44780 Data lines (DB7 - DB4) as outputs*/
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	

	/* Configure the HD44780 Control lines (RS, RW, EN) as outputs*/
	GPIO_InitStruct.Pin = HD44780_RS_BIT | HD44780_RW_BIT | HD44780_EN_BIT;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* clear control bits */
	hd44780_EN_Off();
	hd44780_RS_Off();
	hd44780_RW_Off();
	
	/* wait initial delay for LCD to settle */
  /* reset procedure - 3 function calls resets the device */
  hd44780_init_delay();
  hd44780_hi_nibble_write( HD44780_CMD_RESET );
  hd44780_init_delay2();
  hd44780_hi_nibble_write( HD44780_CMD_RESET );
  hd44780_init_delay3();
  hd44780_hi_nibble_write( HD44780_CMD_RESET );
		
    /* 4bit interface */
  hd44780_hi_nibble_write( HD44780_CMD_FUNCTION );

  /* sets the configured values - can be set again only after reset */
  hd44780_function( HD44780_CONF_BUS, HD44780_CONF_LINES, HD44780_CONF_FONT );

  /* turn the display on with no cursor or blinking */
	hd44780_display( HD44780_DISP_ON, HD44780_DISP_CURS_OFF, HD44780_DISP_BLINK_OFF );
	
  /* clear the display */
  hd44780_clear();

  /* addr increment, shift cursor */
	hd44780_entry( HD44780_ENTRY_ADDR_INC, HD44780_ENTRY_SHIFT_CURS );

}



void HD44780_PutChar(unsigned char c)
{
	hd44780_wr_data(c & 0xff);
}



void HD44780_PutStr(char *str)
{
	__IO unsigned int i=0;

	do
	{
		HD44780_PutChar(str[i]);
		i++;
	}while(str[i]!='\0');
}



void HD44780_ClrScr(void)
{
	hd44780_wr_cmd(HD44780_CMD_CLEAR);
}


