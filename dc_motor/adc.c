/*

   program to initialize ADC and read values from dc motor's tachometer .

 */
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

// ADC handle definitions.
ADC_InitTypeDef ADC_InitStructure;
ADC_HandleTypeDef hadc;
GPIO_InitTypeDef GPIO_InitStruct;

/* Configure ADC */
uint32_t val;
HAL_StatusTypeDef rc;
ADC_ChannelConfTypeDef config;
//******************************************************************* ADC INITIALISATION **************************
// function used from main to initialize adc
void adc_init_new()
{
	__ADC34_CLK_ENABLE();
	__HAL_RCC_ADC12_CONFIG(RCC_ADC12PLLCLK_DIV1);
	/* ADC pins configuration
	   Enable the clock for the ADC GPIOs */
	__GPIOB_CLK_ENABLE();
	/* Configure these ADC pins in analog mode using HAL_GPIO_Init() */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

	/* Initialize ADC */
	hadc.Instance = ADC3;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION12b;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = EOC_SINGLE_CONV;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.NbrOfDiscConversion = 0;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = 0;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.Overrun = OVR_DATA_OVERWRITTEN;
	rc = HAL_ADC_Init(&hadc);
	if(rc != HAL_OK) 
	{
		printf("ADC1 initialization failed with rc=%u\n",rc);
	}
}
// command used initialize adc
void adc_init(int mode)
{
	if (mode!=CMD_INTERACTIVE) return;
	//ADC initialization: 
	/* Enable the ADC interface clock using __ADC_CLK_ENABLE() */
	__ADC34_CLK_ENABLE();
	__HAL_RCC_ADC12_CONFIG(RCC_ADC12PLLCLK_DIV1);
	/* ADC pins configuration
	   Enable the clock for the ADC GPIOs */
	__GPIOB_CLK_ENABLE();
	/* Configure these ADC pins in analog mode using HAL_GPIO_Init() */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

	/* Initialize ADC */
	hadc.Instance = ADC3;
	hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION12b;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc.Init.EOCSelection = EOC_SINGLE_CONV;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.NbrOfDiscConversion = 0;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = 0;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.Overrun = OVR_DATA_OVERWRITTEN;
	rc = HAL_ADC_Init(&hadc);
	if(rc != HAL_OK) 
	{
		printf("ADC1 initialization failed with rc=%u\n",rc);
	}
}

ADD_CMD("adcinit",adc_init," ADC ")

//******************************************************************* ADC READING **************************
//ADC reading:
void adc_read(int mode)
{
	uint32_t channel=5000; //just give a hard cord value to check is any value is entered or not.
	fetch_uint32_arg(&channel);
	if(channel == 5000){
		printf("Error : please enter the channel value \n");
		return;
	}

	/* Configure the selected channel */
	config.Channel = channel;
	config.Rank = 1; /* Rank needs to be 1, otherwise no conversion is done */
	config.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	config.SingleDiff = ADC_SINGLE_ENDED;
	config.OffsetNumber = ADC_OFFSET_NONE;
	config.Offset = 0;
	rc = HAL_ADC_ConfigChannel(&hadc,&config);
	if(rc != HAL_OK) {
		printf("ADC channel configure failed with rc=%u\n",(unsigned)rc);
		return;
	}

	/* Start the ADC peripheral */

	rc = HAL_ADC_Start(&hadc);
	if(rc != HAL_OK) {
		printf("ADC start failed with rc=%u\n",(unsigned)rc);
		return;
	}

	/* Wait for end of conversion */
	rc = HAL_ADC_PollForConversion(&hadc, 100);
	if(rc != HAL_OK) {
		printf("ADC poll for conversion failed with rc=%u\n",(unsigned)rc);
		return;
	}

	/* Read the ADC converted values */

	val = HAL_ADC_GetValue(&hadc);
	printf("val is %u for channel %d \n",(unsigned int)val,(unsigned int)channel);
	/* Stop the ADC peripheral */
	rc = HAL_ADC_Stop(&hadc);
	if(rc != HAL_OK) {
		printf("ADC stop failed with rc=%u\n",(unsigned)rc);
		return;
	}
	return;
}

ADD_CMD("adcread",adc_read," ADC Read ")

int get_motor_speed()
{

	/* Configure the selected channel */
	//config.Channel = channel;
	config.Channel = 12;//hardcoded to 12, since we know we are taking reading from PB0 for tachometer, which is channel 12
	config.Rank = 1; /* Rank needs to be 1, otherwise no conversion is done */
	config.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
	config.SingleDiff = ADC_SINGLE_ENDED;
	config.OffsetNumber = ADC_OFFSET_NONE;
	config.Offset = 0;
	rc = HAL_ADC_ConfigChannel(&hadc,&config);
	if(rc != HAL_OK) {
		printf("ADC channel configure failed rc=%u\n",(unsigned)rc);
		return 0;
	}

	/* Start the ADC peripheral */

	rc = HAL_ADC_Start(&hadc);
	if(rc != HAL_OK) {
		printf("ADC start failed with rc=%u\n",(unsigned)rc);
		return 0;
	}

	/* Wait for end of conversion */
	rc = HAL_ADC_PollForConversion(&hadc, 100);
	if(rc != HAL_OK) {
		printf("ADC poll for conversion failed with rc=%u\n",(unsigned)rc);
		return 0;
	}

	/* Read the ADC converted values */

	val = HAL_ADC_GetValue(&hadc);
	printf("motor speed is %u  \n",(unsigned int)val);
	/* Stop the ADC peripheral */
	rc = HAL_ADC_Stop(&hadc);
	if(rc != HAL_OK) {
		printf("ADC stop failed with rc=%u\n",(unsigned)rc);
		return 0;
	}
	return val;
}
