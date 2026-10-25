/*
 * :file description:
 * :name: /drone-remote/App/Src/app_freertos.c
 * :author: 张德志
 * :date created: 2026-10-25 07:18:14
 * :last editor: 张德志
 * :date last edited: 2026-10-25 10:46:00
 */
#include "app_freertos.h"
#include "app_fault.h"
#include "board_config.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_ip5305t.h"

void power_task(void *args);

#define POWER_TASK_STACK_SIZE 512

// 任务优先级数值越小优先级越小，合法范围 0 ~ configMAX_PRIORITIES-1（当前 0~3）
#define POWER_TASK_PRIORITY 3

TaskHandle_t power_task_handle;

void App_FreeRTOS_Start(void)
{

    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &power_task_handle);

    vTaskStartScheduler();
}

/* configCHECK_FOR_STACK_OVERFLOW=2 时内核要求的钩子，走统一故障处理。 */
void vApplicationStackOverflowHook(TaskHandle_t task, char *task_name)
{
    (void)task;
    (void)task_name;
    App_Fault(APP_FAULT_STACK, __FILE__, __LINE__);
}

void power_task(void *args)
{
    (void)args;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        // 启动电源
        Int_IP5305T_start();

        vTaskDelayUntil(&xLastWakeTime, 10000);
    }
}