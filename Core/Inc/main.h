#ifndef MAIN_H
#define MAIN_H

#include "stm32f1xx_hal.h"

void SystemClock_Config(void);
void Error_Handler(void) __attribute__((noreturn));

#endif
