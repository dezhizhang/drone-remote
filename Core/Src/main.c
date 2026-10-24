#include "main.h"
#include "app_fault.h"
#include "app_freertos.h"
#include "bsp_board.h"
#include "usart.h"

int main(void)
{
    if (HAL_Init() != HAL_OK) {
        Error_Handler();
    }
    SystemClock_Config();
    BSP_Board_Init();
    MX_USART2_UART_Init();
    App_FreeRTOS_Start();

    /* 启动成功后只会运行任务；走到这里表示调度器启动失败。 */
    App_Fault(APP_FAULT_SCHEDULER, __FILE__, __LINE__);
}

void Error_Handler(void)
{
    App_Fault(APP_FAULT_HAL, __FILE__, __LINE__);
}
