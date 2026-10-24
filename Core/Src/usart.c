#include "usart.h"
#include "board_config.h"
#include "main.h"

UART_HandleTypeDef huart2;

void MX_USART2_UART_Init(void)
{
    GPIO_InitTypeDef tx = {0};

    huart2.Instance = USART2;
    huart2.Init.BaudRate = BOARD_DEBUG_UART_BAUDRATE;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }

    /* USART2 now drives its idle level; PA2 can leave GPIO output mode. */
    tx.Pin = GPIO_PIN_2;
    tx.Mode = GPIO_MODE_AF_PP;
    tx.Pull = GPIO_NOPULL;
    tx.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &tx);
}
