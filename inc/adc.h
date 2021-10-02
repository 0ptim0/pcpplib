#include "stm32_base.h"
#include "gpio.h"
#pragma once

#define SAMPLING_TIME LL_ADC_SAMPLINGTIME_239CYCLES_5
#define USE_DMA 1
#define ADC_SCALE  3.3 / (0x0FFF)

class adc_class {
private:
    ADC_HandleTypeDef ADC_InitStructure;
public:
    adc_class(ADC_TypeDef *ADC) {
        ADC_InitStructure.Instance = ADC;
    }
    uint16_t buf[2];
    void Init();
};