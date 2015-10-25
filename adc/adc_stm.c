#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"


ADC_InitTypeDef       ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
GPIO_InitTypeDef      GPIO_InitStructure;
/* Configure ADC */
ADC_ChannelConfTypeDef sConfig;

void adc_init(int mode)
{

 if (mode!=CMD_INTERACTIVE) return;

/* Enable peripheral clocks  */
__GPIOC_CLK_ENABLE();


GPIO_InitStruct.Pin = GPIO_PIN_P1;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

GPIO_InitStruct.Pin = GPIO_PIN_P2;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

GPIO_InitStruct.Pin = GPIO_PIN_P3;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

GPIO_InitStruct.Pin = GPIO_PIN_P4;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


/*Enable the ADC interface */
__ADC1_CLK_ENABLE();


ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
ADC_InitStructure.ADC_ScanConvMode = DISABLE;
ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
ADC_InitStructure.ADC_NbrOfChannel = 1;


if (HAL_ADC_Init(&ADC_InitStructure) != HAL_OK)
{
/* Initialization Error */
Error_Handler();
}

printf("ADC Initialisation done \n");

}

ADD_CMD("adcinit",adc_init,"          ADC ")


void adc_read(int mode)
{
int channel=0;

printf(" enter the channel from which value to be read \n");
scanf("%d",&a);

/* Configure channel sample order*/

if (channel == 0)
{
sConfig.Channel = ADC_CHANNEL_1;
sConfig.SamplingTime = ADCSAMPLES;
HAL_ADC_ConfigChannel(&ADC_InitStructure,&sConfig);
}
else if (channel == 1)
{
sConfig.Channel = ADC_CHANNEL_2;
sConfig.SamplingTime = ADCSAMPLES;
HAL_ADC_ConfigChannel(&ADC_InitStructure,&sConfig);
}
else if(channel == 2){
sConfig.Channel = ADC_CHANNEL_3;
sConfig.SamplingTime = ADCSAMPLES;
HAL_ADC_ConfigChannel(&ADC_InitStructure,&sConfig);
}
else if(channel == 3){
sConfig.Channel = ADC_CHANNEL_4;
sConfig.SamplingTime = ADCSAMPLES;
HAL_ADC_ConfigChannel(&ADC_InitStructure,&sConfig);
}
else
{
	printf(" wrong channel value enetred \n");
}

HAL_ADC_GetValue(&ADC_InitStructure);

}

ADD_CMD("adcread",adc_read,"          ADC Read ")


