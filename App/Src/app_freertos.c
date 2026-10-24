#include "app_freertos.h"
#include "app_fault.h"
#include "board_config.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

#define INPUT_STACK_WORDS    256U
#define MONITOR_STACK_WORDS  256U

static StaticTask_t input_tcb;
static StaticTask_t monitor_tcb;
static StackType_t input_stack[INPUT_STACK_WORDS];
static StackType_t monitor_stack[MONITOR_STACK_WORDS];

volatile AppStatus g_app_status = {
    .board_pinmap_verified = BOARD_PINMAP_VERIFIED
};
volatile BSP_RemoteInput g_remote_input;

static void InputTask(void *argument)
{
    (void)argument;
    TickType_t previous_wake = xTaskGetTickCount();

    for (;;) {
        BSP_RemoteInput input = {0};
        const bool valid = BSP_Remote_ReadInput(&input);
        g_remote_input = input;
        g_app_status.input_valid = valid ? 1U : 0U;
        g_app_status.input_cycles++;
        g_app_status.input_stack_free_words = uxTaskGetStackHighWaterMark(NULL);
        /* 固定 50 Hz，后续在 BSP 接入摇杆 ADC 和按键。 */
        (void)xTaskDelayUntil(&previous_wake, pdMS_TO_TICKS(APP_INPUT_PERIOD_MS));
    }
}

static void MonitorTask(void *argument)
{
    (void)argument;
    TickType_t previous_wake = xTaskGetTickCount();
    static const char pending[] = "remote: FreeRTOS running; input pending\r\n";
    static const char ready[] = "remote: FreeRTOS running; input ready\r\n";

    /* 只有此任务使用串口，避免多任务日志交错；不引入 printf 的堆和栈开销。 */
    for (;;) {
        const bool valid = g_app_status.input_valid != 0U;
        const char *message = valid ? ready : pending;
        const uint16_t length = valid ? (uint16_t)(sizeof(ready) - 1U)
                                      : (uint16_t)(sizeof(pending) - 1U);
        if (HAL_UART_Transmit(&huart2, (const uint8_t *)message, length,
                              APP_UART_TX_TIMEOUT_MS) == HAL_OK) {
            g_app_status.uart_tx_count++;
        } else {
            g_app_status.uart_tx_errors++;
        }
        g_app_status.monitor_cycles++;
        g_app_status.monitor_stack_free_words = uxTaskGetStackHighWaterMark(NULL);
        (void)xTaskDelayUntil(&previous_wake, pdMS_TO_TICKS(APP_MONITOR_PERIOD_MS));
    }
}

void App_FreeRTOS_Start(void)
{
    TaskHandle_t input = xTaskCreateStatic(InputTask, "input", INPUT_STACK_WORDS,
                                          NULL, 2U, input_stack, &input_tcb);
    TaskHandle_t monitor = xTaskCreateStatic(MonitorTask, "monitor", MONITOR_STACK_WORDS,
                                            NULL, 1U, monitor_stack, &monitor_tcb);
    configASSERT(input != NULL);
    configASSERT(monitor != NULL);
    vTaskStartScheduler();
}

void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **stack,
                                  configSTACK_DEPTH_TYPE *stack_words)
{
    static StaticTask_t idle_tcb;
    static StackType_t idle_stack[configMINIMAL_STACK_SIZE];
    *tcb = &idle_tcb;
    *stack = idle_stack;
    *stack_words = configMINIMAL_STACK_SIZE;
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void)task;
    App_Fault(APP_FAULT_STACK, task_name, 0U);
}
