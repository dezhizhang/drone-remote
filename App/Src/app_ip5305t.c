/*
 * :file description:
 * :name: /drone-remote/App/Src/app_IP5305T.c
 * :author: 张德志
 * :date created: 2026-10-25 10:24:27
 * :last editor: 张德志
 * :date last edited: 2026-10-25 10:34:34
 */
#include "app_ip5305t.h"
#include "board_config.h"

void Int_IP5305T_start(void)
{
    GPIO_InitTypeDef key = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    key.Pin = POWER_KEY_Pin;
    key.Mode = GPIO_MODE_OUTPUT_PP;
    key.Pull = GPIO_NOPULL;
    key.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(POWER_KEY_GPIO_Port, &key);

    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    vTaskDelay(100);
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
}