#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

#include <inttypes.h>


/* LCD commands */
#define CLEAR_DISPLAY           0x01    /* clears display and sets DDRAM address 0 in address counter */
#define GO_HOME                         0x02    /* sets DDRAM addres 0 and returns display from being shifted */
#define SHIFT_DISPLAY_LEFT      0x18
#define SHIFT_DISPLAY_RIGHT     0x1C

/* custom characters ram addresses ( to display them use ASCII 0..7 ) */
#define GOTO_CGRAM_0            0x40
#define GOTO_CGRAM_1            0x40 + 1 * 0x08
#define GOTO_CGRAM_2            0x40 + 2 * 0x08
#define GOTO_CGRAM_3            0x40 + 3 * 0x08
#define GOTO_CGRAM_4            0x40 + 4 * 0x08
#define GOTO_CGRAM_5            0x40 + 5 * 0x08
#define GOTO_CGRAM_6            0x40 + 6 * 0x08
#define GOTO_CGRAM_7            0x40 + 7 * 0x08


#define HIGH(bit)                       GPIOC->ODR |= 1<<bit
#define LOW(bit)                        GPIOC->BRR |= 1<<bit

#define D4                                      (4)
#define D5                                      (5)
#define D6                                      (6)
#define D7                                      (7)
#define RS                                      (0)
#define EN                                      (1)

#define LCD_Data(byte)          LCD_SendByte(byte, 1)
#define LCD_Command(byte)       LCD_SendByte(byte, 0)


void LCD_Init();
void LCD_SendByte(uint8_t byte, int type);
void LCD_SetData(uint8_t data);
void LCD_GotoXY(int x, int y);
void LCD_Print(char *text);
void LCD_printf(const char* format, ...);


#endif /* HD44780_H_INCLUDED */
Hide details
Change log
r60 by andrei.istodorescu on Nov 28, 2012   Diff
added headers for hd44780 and usart
Go to: 	
Older revisions
All revisions of this file
File info
Size: 1234 bytes, 46 lines
View raw file
