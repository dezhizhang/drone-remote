#include "main.h"

/* TIM4 is reserved for HAL's 1 ms timebase; SysTick belongs to FreeRTOS. */
TIM_HandleTypeDef htim4;

HAL_StatusTypeDef HAL_InitTick(uint32_t priority)
{
    RCC_ClkInitTypeDef clock;
    uint32_t latency;
    uint32_t timer_clock;

    if (priority >= (1UL << __NVIC_PRIO_BITS)) {
        return HAL_ERROR;
    }
    __HAL_RCC_TIM4_CLK_ENABLE();
    HAL_RCC_GetClockConfig(&clock, &latency);
    timer_clock = HAL_RCC_GetPCLK1Freq();
    if (clock.APB1CLKDivider != RCC_HCLK_DIV1) {
        timer_clock *= 2U;
    }

    /* Also called after RCC changes: stop the old tick before reconfiguring. */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
    if (htim4.Instance != NULL) {
        (void)HAL_TIM_Base_Stop_IT(&htim4);
    }
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = timer_clock / 1000000U - 1U;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000U - 1U;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
        return HAL_ERROR;
    }
    __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);
    HAL_NVIC_ClearPendingIRQ(TIM4_IRQn);
    HAL_NVIC_SetPriority(TIM4_IRQn, priority, 0U);
    uwTickPrio = priority;
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    return HAL_TIM_Base_Start_IT(&htim4);
}

void HAL_SuspendTick(void)
{
    __HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
}

void HAL_ResumeTick(void)
{
    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer)
{
    if (timer->Instance == TIM4) {
        HAL_IncTick();
    }
}
