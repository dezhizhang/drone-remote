/*
 * :file description: 
 * :name: /drone-remote/App/Inc/app_IP5305T.h
 * :author: 张德志
 * :date created: 2026-10-25 10:23:18
 * :last editor: 张德志
 * :date last edited: 2026-10-25 10:33:12
 */
#ifndef APP_IP5305T_H
#define APP_IP5305T_H

#include "FreeRTOS.h"
#include "task.h"

/**
 * @description: 启动IP5305电源，避免自动关机
 * @return {*}
 */
void Int_IP5305T_start(void);

#endif