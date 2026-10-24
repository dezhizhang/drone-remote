#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/* 暂沿用相邻 drone 的 MCU 配置；烧录前按遥控器芯片丝印核对。 */
#define BOARD_NAME                      "ATGUIGU drone remote (pinout pending)"
#define BOARD_EXPECTED_SYSCLK_HZ        64000000UL
#define BOARD_PINMAP_VERIFIED           0U
#define BOARD_DEBUG_UART_BAUDRATE       115200U

#define APP_INPUT_PERIOD_MS             20U
#define APP_MONITOR_PERIOD_MS           1000U
#define APP_UART_TX_TIMEOUT_MS          20U

#endif
