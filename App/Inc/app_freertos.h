#ifndef APP_FREERTOS_H
#define APP_FREERTOS_H

#include <stdint.h>
#include "bsp_board.h"

typedef struct {
    uint32_t input_cycles;
    uint32_t monitor_cycles;
    uint32_t input_valid;
    uint32_t uart_tx_count;
    uint32_t uart_tx_errors;
    uint32_t input_stack_free_words;
    uint32_t monitor_stack_free_words;
    uint32_t board_pinmap_verified;
} AppStatus;

/* 调试器观察用；字段各有一个写者，不能作为跨任务原子快照。 */
extern volatile AppStatus g_app_status;
extern volatile BSP_RemoteInput g_remote_input;

void App_FreeRTOS_Start(void);

#endif
