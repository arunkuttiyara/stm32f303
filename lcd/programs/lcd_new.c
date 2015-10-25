
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

#define lcdPort GPIOD->ODR // Port D drives LCD data pins
#define lcdE 0x40 // E signal (PC6)
#define lcdRW 0x20 // R/W signal (PC5) Connected to ground
#define lcdRS 0x10 // RS signal (PC4)
#define lcdCtl GPIOC->ODR // LCD control port direction
//#define lcdCtl GPIOD->ODR // LCD control port direction


void delaybyms(unsigned int j) {

	unsigned int k,l;
	for(k=0;k<j;k++)
		for(l=0;l<1427;l++);
}

void cmd2lcd (char cmd) {

	char temp;
	char xa, xb;

	lcdCtl &= ~(lcdRS+lcdRW); // select instruction register & pull R/W low
	lcdCtl |= lcdE; // pull E signal to high
	lcdPort = cmd; // output command

	xa = 1; // dummy statements to lengthen E
	xb = 2; // "
	delaybyms(100);
	
	lcdCtl &= ~lcdE; // pull E signal to low
	lcdCtl |= lcdRW; // pull R/W to high
//	delayby50us(1); // wait until the command is complete
	delaybyms(200);
}

void putc2lcd(char cx) {

	char temp;
	char xa, xb;
	lcdCtl |= lcdRS; // select LCD data register and pull R/W high
	lcdCtl &= ~lcdRW; // pull R/W to low
	lcdCtl |= lcdE; // pull E signal to high
	lcdPort = cx; // output data byte

	xa = 1; // create enough width for E
	xb = 2; // create enough width for E
	delaybyms(200);

	lcdCtl &= ~lcdE; // pull E to low
	lcdCtl |= lcdRW; // pull R/W signal to high

	//delayby50us(1);
	delaybyms(200);
}
void puts2lcd (char *ptr) {
	
	while (*ptr) {
		putc2lcd(*ptr);
		ptr++;
	}
}

void initlcd(void) {

	// configure lcdPort port (GPIOD) as output
	GPIOD->MODER |= 0x00005555;
	
	// configure LCD control pins (PC6, PC5, & PC4) as outputs
	GPIOC->MODER |= 0x00001500; 

	delaybyms(1000);
	//delayby100ms(5); // wait for LCD to become ready 
	cmd2lcd (0x38); // set 8-bit data, 2-line display, 5x8 font
	delaybyms(200);
	cmd2lcd (0x0F); // turn on display, cursor, blinking
	delaybyms(200);
	cmd2lcd (0x06); // move cursor right
	delaybyms(200);
	cmd2lcd (0x01); // clear screen, move cursor to home
	delaybyms(200);
	//delayby1ms (2); // wait until "clear display" command is complete
	delaybyms(200);
}

int display_led (void) {

	char *msg1 = "hello world!";
	char *msg2 = "I am ready!";

	initlcd();
	cmd2lcd(0x80); // move cursor to the 1st column of row 1 
	puts2lcd(msg1);
	cmd2lcd(0xC0); // move cursor to 2nd row, 1st column
	puts2lcd(msg2);
}
