#include "stm32_conf.h"
#include "gpio.h"
#include "adc.h"

void adc_class::Init()
{
    if(ADC_InitStructure.Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
    } else if(ADC_InitStructure.Instance == ADC2) {
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
    ADC_ChannelConfTypeDef ADC_ChInitStructure;
    DMA_HandleTypeDef DMA_InitStructure;

    ADC_InitStructure.Init.ScanConvMode = ENABLE;
    ADC_InitStructure.Init.ContinuousConvMode = ENABLE;
    ADC_InitStructure.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.Init.NbrOfConversion = 2;
    ADC_InitStructure.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    HAL_ADC_Init(&ADC_InitStructure);

    __HAL_RCC_DMA1_CLK_ENABLE();
    DMA_InitStructure.Instance = DMA1_Channel1;
    DMA_InitStructure.Init.Mode = DMA_CIRCULAR;
    DMA_InitStructure.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA_InitStructure.Init.MemInc = DMA_MINC_ENABLE;
    DMA_InitStructure.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_InitStructure.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    DMA_InitStructure.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DMA_InitStructure.Init.Priority = DMA_PRIORITY_HIGH;
    ADC_InitStructure.DMA_Handle = &DMA_InitStructure;
    HAL_DMA_Init(&DMA_InitStructure);

    ADC_ChInitStructure.Channel = ADC_CHANNEL_1;
    ADC_ChInitStructure.Rank = ADC_REGULAR_RANK_1;
    ADC_ChInitStructure.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&ADC_InitStructure, &ADC_ChInitStructure);
    ADC_ChInitStructure.Channel = ADC_CHANNEL_2;
    ADC_ChInitStructure.Rank = ADC_REGULAR_RANK_2;
    ADC_ChInitStructure.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&ADC_InitStructure, &ADC_ChInitStructure);

    HAL_ADC_Start_DMA(&ADC_InitStructure, reinterpret_cast<uint32_t *>(this->buf), 2);
}