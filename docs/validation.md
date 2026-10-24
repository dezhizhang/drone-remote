# 初始化验证记录

日期：2026-10-25。验证目标为暂定 STM32F103C8T6 配置，仅覆盖本机编译和固件结构，不代表遥控器实机验证完成。

| 检查 | 结果 |
| --- | --- |
| `make doctor` | ARM GCC / binutils、Python 3、OpenOCD 均可用 |
| `make -j4 check` | Debug 编译与固件检查通过，无编译告警 |
| `make BUILD=release -j4 check` | Release 编译与固件检查通过，无编译告警 |
| 中断向量 | Reset、故障处理、SVC、PendSV、SysTick、TIM4 共 10 个入口正确链接 |
| HAL / RTOS 钩子 | HAL TIM4 时基、Idle 静态内存、栈溢出处理为强符号 |
| 内存分配 | 无 `pvPortMalloc`、`vPortFree`、`malloc`、`_sbrk`、`_write` 依赖 |
| 镜像 | ELF 与 BIN 向量一致，Flash / RAM 范围及 2 KiB 主栈预留检查通过 |
| OpenOCD 配置 | 默认与 NRST 配置均可由本机 OpenOCD 解析；使用 `shutdown` 在探针初始化前退出 |
| `make flash` 流程 | 使用临时模拟 OpenOCD，验证先构建/检查，再传入正确 ELF 和 `verify reset exit`；未写入硬件 |
| 失败处理 | 模拟 OpenOCD 返回 7，Make 正确返回非零状态 |
| 编辑器 | 29 条编译数据库记录均指向当前仓库；VS Code JSON 可解析 |

最终产物占用：

| 配置 | Flash 镜像 | RAM（含预留 2 KiB 主栈） |
| --- | --- | --- |
| Debug | 9104 B / 65536 B | 5280 B / 20480 B |
| Release | 8004 B / 65536 B | 5272 B / 20480 B |

本轮未实际执行硬件烧录，也未测量调度频率、真实任务剩余栈或串口波形。遥控器 MCU 后缀和板卡引脚仍待确认；默认 USART2 引脚尚未核对。摇杆、按键、无线收发及飞行器联调不在当前已完成的能力范围内。

上板后应观察串口每秒心跳、`g_app_status` 的两个循环计数及 `g_app_fault.code`。实际烧录只有在 OpenOCD 报告校验、复位成功后，才能记录为完成。
