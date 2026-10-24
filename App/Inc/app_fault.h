#ifndef APP_FAULT_H
#define APP_FAULT_H

#include <stdint.h>

typedef enum {
    APP_FAULT_NONE = 0,
    APP_FAULT_HAL,
    APP_FAULT_ASSERT,
    APP_FAULT_STACK,
    APP_FAULT_SCHEDULER,
    APP_FAULT_NMI,
    APP_FAULT_HARD,
    APP_FAULT_MEMORY,
    APP_FAULT_BUS,
    APP_FAULT_USAGE
} AppFaultCode;

typedef struct {
    AppFaultCode code;
    const char *file;
    uint32_t line;
    uint32_t cfsr;
    uint32_t hfsr;
    uint32_t bfar;
    uint32_t mmfar;
} AppFaultRecord;

/* Inspect this symbol in the debugger after a fault; cleared on reset. */
extern volatile AppFaultRecord g_app_fault;

void App_Fault(AppFaultCode code, const char *file, uint32_t line)
    __attribute__((noreturn));

#endif
