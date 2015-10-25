#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

/* UART handle declaration */
UART_HandleTypeDef UartHandle;
//NVIC_InitTypeDef NVIC_InitStructure;
uint8_t USARTTxBuffer[80];
//char *p;
char testchat[100] = "test";
char p[250];
uint8_t *r;
extern int int_ok;
void CmdUSARTReceived(){
	printf("recieved buffer is %s \n",p);
}

void serialInit(int mode)
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


	/* NVIC for USART */
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	__USART1_CLK_ENABLE();
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
	   - Word Length = 8 Bits
	   - Stop Bit = One Stop bit
	   - Parity = ODD parity
	   - BaudRate = UARTBAUDRATE baud
	   - Hardware flow control disabled (RTS and CTS signals) */
	UartHandle.Instance = USART1;
	//	UartHandle.Init.BaudRate = 9600;
	UartHandle.Init.BaudRate   = UARTBAUDRATE;
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

	/* Enable the UART Rx interrupt transfers, when a transfer
	 * completes, it will call us back with the Rx Complete callback.
	 * We will 'expect' only a single character.  When it arrives, we
	 * will stuff it in the input buffer, and then call back for another
	 * character.
	 */
	if(HAL_UART_Receive_IT(&UartHandle,
				(uint8_t *)(&(p[0])),1) != HAL_OK) {
		Error_Handler();
	}
	printf("UART Initialiser\n");
}

ADD_CMD("uart",serialInit,"          SERIAL COMMUNICATION")

/**
 * @brief  Rx Transfer completed callback
 * @param  UartHandle: UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *ThisUartHandle)
{
	if(ThisUartHandle == &UartHandle) { 	
		r = (uint8_t *)(uint8_t *)(&(p[0]));
		/* Re-arm interrupt */
		if(HAL_UART_Receive_IT(&UartHandle,r,1) != HAL_OK) {
			Error_Handler();
		}
		/* Enable the UART Data Register not empty Interrupt */
		// __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
		int_ok=1;
	}
}

/* IRQ handler trampoline for HAL UART Driver */
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&UartHandle);
}

void uartsent(int mode)
{
	uint32_t test=0;
	char a='a';
	if (mode!=CMD_INTERACTIVE) return;
	
	//fetch_uint32_arg(&test);
	fetch_char_arg(&a);
	printf("receievd char is %d \n",(int )test);
	printf("##### receievd char is %c \n",a);

	USARTTxBuffer[0]='c';
	printf("CC : sending USART buffer %c \n",USARTTxBuffer[0]);
//	HAL_UART_Transmit(&UartHandle,USARTTxBuffer,1,1000);
	HAL_UART_Transmit_IT(&UartHandle,USARTTxBuffer,1);

}
ADD_CMD("uart_send",uartsent,"		SENT A CHARACTER")

