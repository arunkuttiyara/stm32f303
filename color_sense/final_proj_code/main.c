#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_accelerometer.h"
#include "stm32f3_discovery_gyroscope.h"

#include "common.h"



/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Input Capture Configuration Structure declaration */
TIM_IC_InitTypeDef     sICConfig;

/* Captured Values */
uint32_t               uwIC2Value1 = 0;
uint32_t               uwIC2Value2 = 0;
uint32_t               uwDiffCapture = 0;

/* Capture index */
uint16_t               uhCaptureIndex = 0;

/* Frequency Value */
uint32_t               uwFrequency = 0;





/* Private variables ---------------------------------------------------------*/
const Led_TypeDef LEDs[] = {LED3, LED4, LED5, LED6, LED7, LED8, LED9, LED10};
const uint32_t numLEDs = sizeof(LEDs)/sizeof(LEDs[0]);

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

extern uint32_t uwTick;
volatile int flag=0,sec=0,a=0;

int delay(int delayVal){   
	TIM1->CNT = 0; /* Reset counter */
	while(TIM1->CNT < delayVal) {
		asm volatile ("nop\n");
	}
	return 0;
}



void HAL_IncTick(void)
{
	uwTick++;
	if((uwTick%1000)==0){
		flag=1;
		sec++;
	}
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		if(uhCaptureIndex == 0)
		{
			/* Get the 1st Input Capture value */
			uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			uhCaptureIndex = 1;
		}
		else if(uhCaptureIndex == 1)
		{
			/* Get the 2nd Input Capture value */
			uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); 

			/* Capture computation */
			if (uwIC2Value2 > uwIC2Value1)
			{
				uwDiffCapture = (uwIC2Value2 - uwIC2Value1); 
			}
			else if (uwIC2Value2 < uwIC2Value1)
			{
				uwDiffCapture = ((0xFFFF - uwIC2Value1) + uwIC2Value2); 
			}
			else
			{
				/* If capture values are equal, we have reached the limit of frequency
				   measures */
				Error_Handler();
			}
			/* Frequency computation: for this example TIMx (TIM1) is clocked by
			   APB2Clk */      
			uwFrequency = HAL_RCC_GetPCLK2Freq() / uwDiffCapture;
			uhCaptureIndex = 0;
		}
	}
}



int main(int argc, char **argv)
{
	uint32_t i;
	uint8_t accelRc, gyroRc;
	//GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure the system clock */
	SystemClock_Config();

	HAL_Init();
	TerminalInit();  /* Initialize UART and USB */
	/* Configure the LEDs... */
	for(i=0; i<numLEDs; i++) {
		BSP_LED_Init(LEDs[i]);
	}

	/* Initialize the pushbutton */
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

	/* Initialize the Accelerometer */
	accelRc = BSP_ACCELERO_Init();
	if(accelRc != ACCELERO_OK) {
		printf("Failed to initialize acceleromter\n");
		Error_Handler();
	}

	/* Initialize the Gyroscope */
	gyroRc = BSP_GYRO_Init();
	if(gyroRc != GYRO_OK) {
		printf("Failed to initialize Gyroscope\n");
		Error_Handler();
	}


	//TIM1 initialization
	__TIM1_CLK_ENABLE();
	TIM1->PSC = 1440; /* Set prescaler */
	TIM1->CR1 = 0x01; /* Enable timer */
	TIM1->CNT = 0; /* Reset counter */


	/*##-1- Configure the TIM peripheral #######################################*/ 
	/* TIM1 configuration: Input Capture mode ---------------------
	   The external signal is connected to TIM1 CH2 pin (PE.11)  
	   The Rising edge is used as active edge,
	   The TIM1 CCR2 is used to compute the frequency value 
	   ------------------------------------------------------------ */

	/* Set TIMx instance */
	TimHandle.Instance = TIMx;
	//TimHandle.Instance = TIM1;

	/* Initialize TIMx peripheral as follows:
	   + Period = 0xFFFF
	   + Prescaler = 0
	   + ClockDivision = 0
	   + Counter direction = Up
	 */

	TimHandle.Init.Period        = 0xFFFF;
	TimHandle.Init.Prescaler     = 0;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	if(HAL_TIM_IC_Init(&TimHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- Configure the Input Capture channel ################################*/ 
	/* Configure the Input Capture of channel 2 */
	sICConfig.ICPolarity  = TIM_ICPOLARITY_RISING;
	sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sICConfig.ICFilter    = 0;   
	if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sICConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/*##-3- Start the Input Capture in interrupt mode ##########################*/
	if(HAL_TIM_IC_Start_IT(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}


	while(1) {

		if(flag){
			printf(" frequency is %d  ..... \n",(unsigned int)uwFrequency);
			flag=0;
		
		if(uwFrequency<145000 && uwFrequency > 125000 ){
				printf("colour is red .... \n");
				printf(" a is .... %d \n",(int)a);
				if(a==0 || a==3 || a==1)
					{
				a=1;
				BSP_LED_On(LED_RED);
				BSP_LED_Off(LED_GREEN);
				BSP_LED_Off(LED_BLUE);
				BSP_LED_Off(LED_ORANGE);
					}
				else{
						printf("error : parts not painted  ");
						BSP_LED_On(LED_ORANGE);
						continue;
				}	
			}	
		else if(uwFrequency < 45000 && uwFrequency > 40000){
				printf("colour is green .... \n");
				
				if(a==1 || a==2)
					{
				a=2;
				BSP_LED_On(LED_GREEN);
				BSP_LED_Off(LED_RED);
				BSP_LED_Off(LED_BLUE);
				BSP_LED_Off(LED_ORANGE);
					}
				else{
						printf("error : parts not painted  ");
						BSP_LED_On(LED_ORANGE);
						continue;
			}
			}	
		else if (uwFrequency < 35000 && uwFrequency > 30000){
				printf("colour is blue .... \n");
				if(a==2 || a==3)
					{
				a=3;
				BSP_LED_On(LED_BLUE);
				BSP_LED_Off(LED_RED);
				BSP_LED_Off(LED_GREEN);
				BSP_LED_Off(LED_ORANGE);
					} else{
						printf("error : parts not painted  ");
						BSP_LED_On(LED_ORANGE);
						continue;
			}
			}	
		else if (uwFrequency < 280000 && uwFrequency > 210000){
				printf("error : parts rejected ..... ");
				BSP_LED_On(LED_ORANGE);
				BSP_LED_Off(LED_RED);
				BSP_LED_Off(LED_BLUE);
				BSP_LED_Off(LED_GREEN);
			}	
		}

		TaskInput();
	}

	return 0;
}


/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow : 
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 72000000
 *            HCLK(Hz)                       = 72000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            HSE PREDIV                     = 1
 *            PLLMUL                         = RCC_PLL_MUL9 (9)
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
	{
		Error_Handler();
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
	   clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)!= HAL_OK)
	{
		Error_Handler();
	}
}
/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler(void)
{
	BSP_LED_On(LED6);
	/* Infinite loop */
	while(1)
	{
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void CmdLED(int mode)
{
	uint32_t led,val;
	int rc;
	if(mode != CMD_INTERACTIVE) {
		return;
	}

	rc = fetch_uint32_arg(&led);
	if(rc) {
		printf("Missing LED index\n");
		return;
	}

	rc = fetch_uint32_arg(&val);
	if(rc) {
		printf("Missing state value, 0 for Off, 1 for On\n");
		return;
	}

	if((led < 3) || (led > 10)) {
		printf("Led index of %u is out of the range (3..10)\n",
				(unsigned int)led);
		return;
	}

	led -= 3;
	if(val) {
		BSP_LED_On(LEDs[led]);
	} else {
		BSP_LED_Off(LEDs[led]);
	}

} 

ADD_CMD("led",CmdLED,"<index> <state> Turn off/on LED")

void CmdAccel(int mode)
{
	int16_t xyz[3];

	if(mode != CMD_INTERACTIVE) {
		return;
	}

	BSP_ACCELERO_GetXYZ(xyz);

	printf("Accelerometer returns:\n"
			"   X: %d\n"
			"   Y: %d\n"
			"   Z: %d\n",
			xyz[0],xyz[1],xyz[2]);


}

ADD_CMD("accel", CmdAccel,"                Read Accelerometer");

void CmdGyro(int mode)
{
	float xyz[3];

	if(mode != CMD_INTERACTIVE) {
		return;
	}

	BSP_GYRO_GetXYZ(xyz);

	printf("Gyroscope returns:\n"
			"   X: %d\n"
			"   Y: %d\n"
			"   Z: %d\n",
			(int)(xyz[0]*256),
			(int)(xyz[1]*256),
			(int)(xyz[2]*256));
}

ADD_CMD("gyro", CmdGyro,"                Read Gyroscope");

void CmdButton(int mode)
{
	uint32_t button;

	if(mode != CMD_INTERACTIVE) {
		return;
	}

	button = BSP_PB_GetState(BUTTON_USER);

	printf("Button is currently: %s\n",
			button ? "Pressed" : "Released");

	return;
}

ADD_CMD("button", CmdButton,"                Print status of user button");
