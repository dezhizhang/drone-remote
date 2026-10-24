#include "app_fault.h"
#include "bsp_board.h"
#include "stm32f1xx.h"

volatile AppFaultRecord g_app_fault;

void App_Fault(AppFaultCode code, const char *file, uint32_t line)
{
    __disable_irq();
    BSP_Board_EnterSafeState();
    g_app_fault.code = code;
    g_app_fault.file = file;
    g_app_fault.line = line;
    g_app_fault.cfsr = SCB->CFSR;
    g_app_fault.hfsr = SCB->HFSR;
    g_app_fault.bfar = SCB->BFAR;
    g_app_fault.mmfar = SCB->MMFAR;
    __DSB();
    for (;;) {
        __NOP();
    }
}
