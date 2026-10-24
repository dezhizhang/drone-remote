#include "main.h"
#include "app_fault.h"

extern TIM_HandleTypeDef htim4;

void NMI_Handler(void)
{
    App_Fault(APP_FAULT_NMI, __FILE__, __LINE__);
}

void HardFault_Handler(void)
{
    App_Fault(APP_FAULT_HARD, __FILE__, __LINE__);
}

void MemManage_Handler(void)
{
    App_Fault(APP_FAULT_MEMORY, __FILE__, __LINE__);
}

void BusFault_Handler(void)
{
    App_Fault(APP_FAULT_BUS, __FILE__, __LINE__);
}

void UsageFault_Handler(void)
{
    App_Fault(APP_FAULT_USAGE, __FILE__, __LINE__);
}

void DebugMon_Handler(void)
{
}

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}

/* SVC、PendSV、SysTick 在 FreeRTOS ARM_CM3 port.c 中实现，不在这里重复定义。 */
