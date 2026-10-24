#include "main.h"
#include "usart.h"

void HAL_MspInit(void)
{
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void HAL_UART_MspInit(UART_HandleTypeDef *uart)
{
    if (uart->Instance == USART2) {
        GPIO_InitTypeDef gpio = {0};
        __HAL_RCC_AFIO_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_AFIO_REMAP_USART2_DISABLE();

        /* Keep PA2 at idle high while HAL_UART_Init configures a disabled UART.
         * MX_USART2_UART_Init hands it over only after the UART is enabled. */
        gpio.Pin = GPIO_PIN_2;
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        gpio.Pull = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_Init(GPIOA, &gpio);

        gpio.Pin = GPIO_PIN_3;
        gpio.Mode = GPIO_MODE_INPUT;
        gpio.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &gpio);
    }
}
