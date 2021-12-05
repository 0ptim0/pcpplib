#include "stm32_base.h"
#include "gpio.h"

gpio_class::gpio_class() : cfg(nullptr) 
{
}

gpio_class::gpio_class(gpio_cfg_t *cfg) : cfg(cfg) 
{
}

gpio_class::gpio_class(gpio_cfg_t *cfg, gpio_exti_cfg_t *exti_cfg) : cfg(cfg), exti_cfg(exti_cfg)
{
}

int gpio_class::Init(void) {   
    int rv;
    if(cfg == nullptr) return EINVAL;
    if(rv = this->ClockEnable() != 0) return rv;
    HAL_GPIO_Init(cfg->GPIO, (GPIO_InitTypeDef *)&cfg->GPIO_InitStructure);
    if(exti_cfg != nullptr) {
        EnableEXTI_IT();
        HAL_EXTI_SetConfigLine((EXTI_HandleTypeDef *)&exti_cfg->EXTI_Handle, 
                            (EXTI_ConfigTypeDef *)&exti_cfg->EXTI_ConfigStructure);
    }
    return 0;
}

int gpio_class::SetConf(gpio_cfg_t *cfg_ext) {
    int rv;
    cfg = cfg_ext;
    Init();
    return 0;
}

int gpio_class::SetConf(gpio_cfg_t *cfg_ext, gpio_exti_cfg_t *exti_cfg_ext) {
    int rv;
    cfg = cfg_ext;
    exti_cfg = exti_cfg_ext;
    Init();
    return 0;
}

void gpio_class::SetOn(void) {
    HAL_GPIO_WritePin(cfg->GPIO, cfg->GPIO_InitStructure.Pin, GPIO_PIN_SET);
}

void gpio_class::SetOff(void) {   
    HAL_GPIO_WritePin(cfg->GPIO, cfg->GPIO_InitStructure.Pin, GPIO_PIN_RESET);
}

void gpio_class::Toggle(void) {   
    HAL_GPIO_TogglePin(cfg->GPIO, cfg->GPIO_InitStructure.Pin);
}

int gpio_class::EnableEXTI_IT(void) {
    int rv = 0;

    if(__HAL_RCC_AFIO_IS_CLK_DISABLED()) __HAL_RCC_AFIO_CLK_ENABLE();

    if(cfg->GPIO_InitStructure.Pin == GPIO_PIN_0) {
        HAL_NVIC_SetPriority(EXTI0_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin == GPIO_PIN_1) {
        HAL_NVIC_SetPriority(EXTI1_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin == GPIO_PIN_2) {
        HAL_NVIC_SetPriority(EXTI2_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin == GPIO_PIN_3) {
        HAL_NVIC_SetPriority(EXTI3_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin == GPIO_PIN_4) {
        HAL_NVIC_SetPriority(EXTI4_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin >= GPIO_PIN_5 &&
              cfg->GPIO_InitStructure.Pin <= GPIO_PIN_9) {
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    } else if(cfg->GPIO_InitStructure.Pin >= GPIO_PIN_10 &&
              cfg->GPIO_InitStructure.Pin <= GPIO_PIN_15) {
        HAL_NVIC_SetPriority(EXTI15_10_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    } else {
        rv = EINVAL;
        return rv;
    }
    return rv;
}

int gpio_class::ClearBitEXTI_IT(void) {
    HAL_EXTI_ClearPending((EXTI_HandleTypeDef *)&exti_cfg->EXTI_Handle, EXTI_TRIGGER_RISING_FALLING);
    return 0;
}

uint32_t gpio_class::GetPin(void) {
    return cfg->GPIO_InitStructure.Pin;
}

int gpio_class::ClockEnable(void) {
    switch(reinterpret_cast<uint32_t>(cfg->GPIO)) {

#ifdef GPIOA
        case GPIOA_BASE:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
#endif

#ifdef GPIOB
        case GPIOB_BASE:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
#endif

#ifdef GPIOC
        case GPIOC_BASE:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
#endif

#ifdef GPIOD
        case GPIOD_BASE:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
#endif

#ifdef GPIOE
        case GPIOE_BASE:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
#endif

#ifdef GPIOF
        case GPIOF_BASE:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
#endif

#ifdef GPIOG
        case GPIOG_BASE:
            __HAL_RCC_GPIOG_CLK_ENABLE();
            break;
#endif

#ifdef GPIOH
        case GPIOH_BASE:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
#endif

        default:
            return EINVAL;
    }

    return 0;
}

