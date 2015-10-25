#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

/* UART handle declaration */
UART_HandleTypeDef UartHandle;

void vigserialInit(int mode)
{
 if (mode!=CMD_INTERACTIVE) return;

GPIO_InitTypeDef GPIO_InitStruct;
/* Configure the GPIO pins for the UART */
__GPIOC_CLK_ENABLE();
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 7;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
GPIO_InitStruct.Pin = GPIO_PIN_4;
GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
GPIO_InitStruct.Pull = GPIO_PULLUP;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 7;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


__USART1_CLK_ENABLE();
/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
/* UART configured as follows:
- Word Length = 8 Bits
- Stop Bit = One Stop bit
- Parity = ODD parity
- BaudRate = UARTBAUDRATE baud
- Hardware flow control disabled (RTS and CTS signals) */
UartHandle.Instance = USART1;
UartHandle.Init.BaudRate = 9600;
UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
UartHandle.Init.StopBits = UART_STOPBITS_1;
UartHandle.Init.Parity = UART_PARITY_NONE;
UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
UartHandle.Init.Mode = UART_MODE_TX_RX;
if (HAL_UART_Init(&UartHandle) != HAL_OK)
{
/* Initialization Error */
Error_Handler();
}
printf("UART Initialiser\n");

}

ADD_CMD("USART",vigserialInit,"          SERIAL COMMUNICATION")



uint8_t USARTTxBuffer[80];

void uartsent(int mode)
{
uint32_t c;
if (mode!=CMD_INTERACTIVE) return;
fetch_uint32_arg(&c);
USARTTxBuffer[0]=c;
printf("sending a character\n");
HAL_UART_Transmit(&UartHandle,USARTTxBuffer,1,1000);

}

ADD_CMD("UART_SENT",uartsent,"		SENT A CHARACTER")






