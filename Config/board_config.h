#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "main.h"

/* 用户已确认遥控器主控为 STM32F103C8T6；外设引脚仍待核对。 */

/* IP5305T KEY -> PA0，已确认（2026-10-25）。 */
#define POWER_KEY_Pin                    GPIO_PIN_0
#define POWER_KEY_GPIO_Port              GPIOA
#define BOARD_NAME                      "ATGUIGU drone remote (pinout pending)"
#define BOARD_EXPECTED_SYSCLK_HZ        64000000UL
#define BOARD_PINMAP_VERIFIED           0U
#define BOARD_DEBUG_UART_BAUDRATE       115200U

#define APP_INPUT_PERIOD_MS             20U
#define APP_MONITOR_PERIOD_MS           1000U
#define APP_UART_TX_TIMEOUT_MS          20U

#endif
