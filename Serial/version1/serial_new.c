#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"
#define MAX_DATA 256

extern int int_ok;
// The Baud Rate. Here is the some values: minimum: 2400, 9600, 115200, maximum in this configuration is: 4000000
#define LOCAL_UARTBAUDRATE 115200

// Functions prototype

void UART1_DMA1_Init(int mode);

/* Handle declarations */

UART_HandleTypeDef UartHandle;

DMA_HandleTypeDef HandleDMAUartTx, HandleDMAUartRx;

//

//uint8_t DataBufferForTransmit[MAX_DATA + 1]; // buffers for send and receive data by USART
uint8_t DataBufferForTransmit[MAX_DATA + 1] = "abcde"; // buffers for send and receive data by USART
uint8_t DataBufferForReceive[MAX_DATA + 1]; // additional elements only for correct working the "printf" function. they are always equal zero
uint16_t amountOfDataToTransmit=5;
uint8_t circQueue[MAX_DATA]; // Circular queue array
uint32_t head, tail, capacity; // circular queue attributes
char *HalStatus[] = { "OK", "ERROR", "BUSY", "TIMEOUT" };

//---------------------------------------------

// Display the receiver buffer content

void CmdUSARTReceived(int mode)
{
//	if (mode != CMD_INTERACTIVE) return;
	printf("Received Data:\t\t");
	for (int i = 0; DataBufferForReceive[i] != 0 && i<MAX_DATA; i++){ // loop until either the end of string or the maximum size of array will be reached
		printf("%c", DataBufferForReceive[i]);
	}
	printf("\n");
}

//---------------------------------------------

// The USART transmission using DMA
void CmdUARTSend(int mode)
{
	//char *c;
	int rc;

	if (mode != CMD_INTERACTIVE) return;
	//arun-test
	if (USART1->CR1 == 0) UART1_DMA1_Init(1); // check if UART Initialization was done
	// check if previous process was unsuccessful
	if (UartHandle.State != HAL_UART_STATE_READY)
	{
		HAL_UART_DMAStop(&UartHandle); // Stops the DMA Transfer
		__HAL_UART_ENABLE(&UartHandle); // Restart USART
		printf("USART settings were recovered after error\n");
	}

	for (int i = 0; i <= MAX_DATA; i++) DataBufferForReceive[i] = 0; // to clear receive buffer
	// need to put the code in DataBufferForTransmit to be sent and set the amountOfDataToTransmit in here
	rc = HAL_UART_Receive_DMA(&UartHandle, DataBufferForReceive, amountOfDataToTransmit); // UART receiver initialization
	if (rc != HAL_OK) printf("Receiver initialization error. HAL Status: %s\n", HalStatus[rc]);
	rc = HAL_UART_Transmit_DMA(&UartHandle, DataBufferForTransmit, amountOfDataToTransmit); // UART transmitter initialization
	if (rc != HAL_OK) printf("Transmitter initialization error. HAL Status: %s\n", HalStatus[rc]);
	printf("Transmitted Bytes:\t%d\nTransmitted data:\t%s\n", amountOfDataToTransmit, DataBufferForTransmit);
}
ADD_CMD("uart_send",CmdUARTSend,"  SENT A CHARACTER")

//---------------------------------------------

// user callbacks function is executed at the end of the receive process
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* changing the */
	int_ok=1;

}

//---------------------------------------------

// user callbacks function is executed at the end of the transmit process

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)

{
	// end of the transmit stage
	// may need to change queue/status etc
}

//---------------------------------------------

// user callback will be executed when a communication error is detected

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	// Error indicator, display or do something?
}

//---------------------------------------------

// IRQ Handlers, by himself
// Channel for transmitter

void DMA1_Channel4_IRQHandler(void)

{
	HAL_DMA_IRQHandler(&HandleDMAUartTx); // This function handles DMA Channel4 interrupt request.
}

// Channel for receiver

void DMA1_Channel5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&HandleDMAUartRx); // This function handles DMA Channel5 interrupt request.
}

//---------------------------------------------

// Hardware initialization
//void UART1_DMA1_Init(void)
void UART1_DMA1_Init(int mode)
{
	int rc;
	GPIO_InitTypeDef GPIO_InitStruct;
	// Configure the GPIO pins for the UART
	__GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate Function Push Pull Mode
	GPIO_InitStruct.Pull = GPIO_PULLUP; // Pull-up activation
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; // High speed
	GPIO_InitStruct.Alternate = 7; // Peripheral to be connected to the selected pins
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	__DMA1_CLK_ENABLE();

	// stm32f3xx_hal_dma.h -> DMA Configuration Structure definition.

	HandleDMAUartTx.Instance = DMA1_Channel4; // Register base address

	HandleDMAUartTx.Init.Direction = DMA_MEMORY_TO_PERIPH; // Memory to peripheral direction

	HandleDMAUartTx.Init.PeriphInc = DMA_PINC_DISABLE; // Peripheral increment mode Disable

	HandleDMAUartTx.Init.MemInc = DMA_MINC_ENABLE; // Memory increment mode Enable

	HandleDMAUartTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // Peripheral data alignment : Byte

	HandleDMAUartTx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // Memory data alignment : Byte

	HandleDMAUartTx.Init.Mode = DMA_NORMAL; // The buffer mode - Normal Mode. Can be circular

	HandleDMAUartTx.Init.Priority = DMA_PRIORITY_VERY_HIGH; // Priority level : Very_High

	HandleDMAUartTx.Parent = &UartHandle; // Parent object state

	rc = HAL_DMA_Init(&HandleDMAUartTx);

	if (rc != HAL_OK) printf("DMA Channel 4 (Transmitter) initialization error. HAL Status: %s\n", HalStatus[rc]);

	else BSP_LED_On(LED_GREEN); // DMA Channel 4 (Transmitter) was initialized successfully

	HandleDMAUartRx.Instance = DMA1_Channel5; // Register base address

	HandleDMAUartRx.Init.Direction = DMA_PERIPH_TO_MEMORY; // Peripheral to memory direction

	HandleDMAUartRx.Init.PeriphInc = DMA_PINC_DISABLE; // Peripheral increment mode Disable

	HandleDMAUartRx.Init.MemInc = DMA_MINC_ENABLE; // Memory increment mode Enable

	HandleDMAUartRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // Peripheral data alignment : Byte

	HandleDMAUartRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // Memory data alignment : Byte

	HandleDMAUartRx.Init.Mode = DMA_NORMAL; // The buffer mode - Normal Mode. Can be circular

	HandleDMAUartRx.Init.Priority = DMA_PRIORITY_VERY_HIGH; // Priority level : Very_High

	HandleDMAUartRx.Parent = &UartHandle; // Parent object state

	rc = HAL_DMA_Init(&HandleDMAUartRx);

	if (rc != HAL_OK) printf("DMA Channel 5 (Receiver) initialization error. HAL Status: %s\n", HalStatus[rc]);

	else BSP_LED_On(LED_GREEN_2);// DMA Channel 5 (Receiver) was initialized successfully

	__USART1_CLK_ENABLE();

	// Put the USART peripheral in the Asynchronous mode (UART Mode)

	UartHandle.Instance = USART1; // UART registers base address

	UartHandle.Init.BaudRate = LOCAL_UARTBAUDRATE; // BaudRate = LOCAL_UARTBAUDRATE baud

	UartHandle.Init.WordLength = UART_WORDLENGTH_8B; // Word Length = 8 Bits

	UartHandle.Init.StopBits = UART_STOPBITS_1; // Stop Bit = One Stop bit

	UartHandle.Init.Parity = UART_PARITY_NONE; // Parity = ODD parity

	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE; // Hardware flow control disabled (RTS and CTS signals)

	UartHandle.Init.Mode = UART_MODE_TX_RX; // Specifies wether the Receive or Transmit mode is enabled or disabled.

	UartHandle.hdmatx = &HandleDMAUartTx; // UART Tx DMA Handle parameters

	UartHandle.hdmarx = &HandleDMAUartRx; // UART Rx DMA Handle parameters

	rc = HAL_UART_Init(&UartHandle);

	if (rc != HAL_OK) printf("UART initialization error. HAL Status: %s\n", HalStatus[rc]);

	else printf("UART was initialized successfully\n");

	// Enable DMA1 Channels 4 and 5 Interrupts

	NVIC_EnableIRQ(DMA1_Channel4_IRQn);

	NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

//ADD_CMD("uart",UART1_DMA1_Init," SERIAL COMMUNICATION")
ADD_CMD("uart",UART1_DMA1_Init,"  initialization ")





