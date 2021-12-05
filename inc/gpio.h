#pragma once
#include "stm32_base.h"
#include "rcc_periph.h"

struct gpio_cfg_t {
    GPIO_TypeDef *GPIO;
    GPIO_InitTypeDef GPIO_InitStructure;
};

struct gpio_exti_cfg_t {
    EXTI_ConfigTypeDef EXTI_ConfigStructure;
    EXTI_HandleTypeDef EXTI_Handle;
};

class gpio_base_class
{
public:
    virtual int Init(void) = 0;
};

class gpio_class : public gpio_base_class, public rcc_periph_class
{
protected:
    gpio_cfg_t *cfg;
    gpio_exti_cfg_t *exti_cfg;

public:
    gpio_class(gpio_cfg_t *cfg);
    gpio_class(gpio_cfg_t *cfg, gpio_exti_cfg_t *exti_cfg);
    gpio_class();
    int Init(void);
    int SetConf(gpio_cfg_t *cfg_ext);
    int SetConf(gpio_cfg_t *cfg_ext, gpio_exti_cfg_t *exti_cfg_ext);
    void SetOn(void);
    void SetOff(void);
    void Toggle(void);
    int EnableEXTI_IT(void);
    int ClearBitEXTI_IT(void);
    int ClockEnable(void);
    uint32_t GetPin(void);
};
