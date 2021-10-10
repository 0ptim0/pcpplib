#include "stm32_base.h"
#include "usart.h"

static SemaphoreHandle_t mutex;
static SemaphoreHandle_t semaphore;

static uint32_t e = 0;

usart_class::usart_class(const usart_cfg_t *const cfg) 
            : cfg(cfg), TX(cfg->tx_cfg), RX(cfg->rx_cfg)
{
}

int usart_class::Init(void)
{   
    portENTER_CRITICAL();
    int rv;
    if(rv = this->ClockEnable() != 0) return rv;
    
    if(rv = TX.Init() != 0) return rv;
    if(rv = RX.Init() != 0) return rv;

    NVIC_SetPriorityGrouping(0);

    USART_InitStructure.Instance = cfg->USART;
    USART_InitStructure.Init.BaudRate = cfg->speed;
    USART_InitStructure.Init.Mode = UART_MODE_TX_RX;
    USART_InitStructure.Init.OverSampling = UART_OVERSAMPLING_16;
    USART_InitStructure.Init.Parity = UART_PARITY_NONE;
    USART_InitStructure.Init.StopBits = UART_STOPBITS_1;
    USART_InitStructure.Init.WordLength = UART_WORDLENGTH_8B;

    if(HAL_UART_Init(&USART_InitStructure) != HAL_OK) return EACCES;
    if(mutex == NULL) mutex = xSemaphoreCreateMutex();
    if(semaphore == NULL) semaphore = xSemaphoreCreateBinary();
    portEXIT_CRITICAL();
    return 0;
}

int usart_class::Transmit(uint8_t *pdata, uint16_t length) 
{   
    if(xSemaphoreTake(mutex, pdMS_TO_TICKS(cfg->timeout)) == pdTRUE) {
        
        HAL_UART_Init(&USART_InitStructure);
        EnableIRQn();
        HAL_UART_Transmit_IT(&USART_InitStructure, pdata, length);

        if(xSemaphoreTake(semaphore, pdMS_TO_TICKS(cfg->timeout)) == pdFALSE &&
        (__HAL_UART_GET_FLAG(&USART_InitStructure, USART_SR_TXE))) {
            xSemaphoreGive(mutex);
            HAL_UART_AbortTransmit_IT(&USART_InitStructure);
            HAL_UART_DeInit(&USART_InitStructure);
            return ETIME;
        } else {
            xSemaphoreGive(mutex);
            HAL_UART_DeInit(&USART_InitStructure);
            return 0;
        }
    } else {
        return ETIME;
    }
}

int usart_class::Receive(uint8_t *pdata, uint16_t length) 
{   
    if(xSemaphoreTake(mutex, pdMS_TO_TICKS(cfg->timeout)) == pdTRUE) {

        HAL_UART_Init(&USART_InitStructure);
        EnableIRQn();
        HAL_UART_Receive_IT(&USART_InitStructure, pdata, length);

        if(xSemaphoreTake(semaphore, pdMS_TO_TICKS(cfg->timeout)) == pdFALSE &&
        !(__HAL_UART_GET_FLAG(&USART_InitStructure, USART_SR_RXNE))) {
            xSemaphoreGive(mutex);
            HAL_UART_AbortReceive_IT(&USART_InitStructure);
            HAL_UART_DeInit(&USART_InitStructure);
            return ETIME;
        } else {
            xSemaphoreGive(mutex);
            HAL_UART_DeInit(&USART_InitStructure);
            return 0;
        }
    } else {
        return ETIME;
    }
}

int usart_class::Handler(void) 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    HAL_UART_IRQHandler(&USART_InitStructure);

    if(USART_InitStructure.ErrorCode != HAL_UART_ERROR_NONE) {
        ErrorHandler();
    }

    if(USART_InitStructure.RxState == HAL_UART_STATE_READY) {
        DisableIRQn();
        xSemaphoreGiveFromISR(semaphore, &xHigherPriorityTaskWoken);
        if(xHigherPriorityTaskWoken == pdTRUE) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    return 0;
}

int usart_class::ErrorHandler(void) 
{
    HAL_UART_AbortReceive_IT(&USART_InitStructure);
    __HAL_UART_CLEAR_OREFLAG(&USART_InitStructure);
    __HAL_UART_CLEAR_FEFLAG(&USART_InitStructure);
    HAL_UART_DeInit(&USART_InitStructure);
}


int usart_class::EnableIRQn(void) 
{
    portENTER_CRITICAL();
    if(cfg->USART == USART1) {
        NVIC_EnableIRQ(USART1_IRQn);
        NVIC_SetPriority(USART1_IRQn, 11);
    } else if(cfg->USART == USART2) {
        NVIC_EnableIRQ(USART2_IRQn);
        NVIC_SetPriority(USART2_IRQn, 11);
    } else if(cfg->USART == USART3) {
        NVIC_EnableIRQ(USART3_IRQn);
        NVIC_SetPriority(USART3_IRQn, 11);
    } else {
        portEXIT_CRITICAL();
        return EINVAL;
    }
    portEXIT_CRITICAL();
    return 0;
}

int usart_class::DisableIRQn(void) 
{
    portENTER_CRITICAL();
    if(cfg->USART == USART1) {
        NVIC_DisableIRQ(USART1_IRQn);
        NVIC_SetPriority(USART1_IRQn, 11);
    } else if(cfg->USART == USART2) {
        NVIC_DisableIRQ(USART2_IRQn);
        NVIC_SetPriority(USART2_IRQn, 11);
    } else if(cfg->USART == USART3) {
        NVIC_DisableIRQ(USART3_IRQn);
        NVIC_SetPriority(USART3_IRQn, 11);
    } else {
        portEXIT_CRITICAL();
        return EINVAL;
    }
    portEXIT_CRITICAL();
    return 0;
}

int usart_class::ClockEnable(void)
{
    switch(reinterpret_cast<uint32_t>(cfg->USART)) {

#ifdef USART1
        case USART1_BASE:
            __HAL_RCC_USART1_CLK_ENABLE();
            break;
#endif

#ifdef USART2
        case USART2_BASE:
            __HAL_RCC_USART2_CLK_ENABLE();
            break;
#endif

#ifdef USART3
        case USART3_BASE:
            __HAL_RCC_USART3_CLK_ENABLE();
            break;
#endif
        default:
            return EINVAL;
    }
    return 0;
}
