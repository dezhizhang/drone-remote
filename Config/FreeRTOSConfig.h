#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
#include "app_fault.h"

extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                     1
#define configUSE_TIME_SLICING                   1
#define configCPU_CLOCK_HZ                       SystemCoreClock
#define configTICK_RATE_HZ                       1000U
#define configMAX_PRIORITIES                     4U
#define configMINIMAL_STACK_SIZE                 128U
#define configMAX_TASK_NAME_LEN                  16
#define configTICK_TYPE_WIDTH_IN_BITS            TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                  1
#define configUSE_TICKLESS_IDLE                  0

/* 任务统一用 xTaskCreate 动态创建，由 heap_4.c 提供 pvPortMalloc。 */
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configKERNEL_PROVIDED_STATIC_MEMORY      0

/* F103C8 共 20K SRAM，堆 8K；任务栈深度单位是字（4 字节）。 */
#define configTOTAL_HEAP_SIZE                    (8U * 1024U)
#define configUSE_MUTEXES                       0
#define configUSE_RECURSIVE_MUTEXES              0
#define configUSE_COUNTING_SEMAPHORES            0
#define configUSE_TASK_NOTIFICATIONS             1
#define configQUEUE_REGISTRY_SIZE                0
#define configUSE_QUEUE_SETS                     0
#define configUSE_TIMERS                         0
#define configUSE_CO_ROUTINES                    0
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_MALLOC_FAILED_HOOK             0
#define configUSE_TRACE_FACILITY                 0
#define configGENERATE_RUN_TIME_STATS            0
#define configUSE_STATS_FORMATTING_FUNCTIONS     0

#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskDelayUntil                  1
#define INCLUDE_vTaskSuspend                     0
#define INCLUDE_vTaskDelete                      0
#define INCLUDE_uxTaskGetStackHighWaterMark       1
#define INCLUDE_xTaskGetSchedulerState           1

/* F103 的 NVIC 有 4 位优先级。使用 RTOS API 的中断优先级需 >= 5。 */
#define configPRIO_BITS                          4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY          (15U << (8U - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     (5U << (8U - configPRIO_BITS))
#define configCHECK_HANDLER_INSTALLATION         1

#define vPortSVCHandler                          SVC_Handler
#define xPortPendSVHandler                       PendSV_Handler
#define xPortSysTickHandler                      SysTick_Handler

#define configASSERT(condition) do { \
    if (!(condition)) { App_Fault(APP_FAULT_ASSERT, __FILE__, __LINE__); } \
} while (0)

#endif
