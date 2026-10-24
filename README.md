# 尚硅谷四轴遥控器 · HAL + FreeRTOS 入门工程

本工程从空仓库初始化，复用相邻 `drone` 工程中的 ST HAL、CMSIS、FreeRTOS 源码，但所有依赖都已放入本仓库，编译不依赖 `../drone`，也不需要联网、Keil 或 CubeIDE。

**遥控器芯片及引脚尚未确认，暂按 STM32F103C8T6 配置（64 KiB Flash / 20 KiB RAM）。** 这是一份可编译的 RTOS 基础工程；摇杆、按键、LED、屏幕和无线模块尚未接入，当前不能遥控飞行器。相邻 `drone` 目前也没有无线接收协议，因此没有凭空定义一份“兼容协议”。

## 编译与烧录

需要 `arm-none-eabi-gcc`（含 C 库及 binutils）、GNU Make、Python 3，烧录使用 OpenOCD + ST-Link。兼容 macOS 自带 Make 3.81。

```sh
make doctor               # 检查本机工具
make -j4                  # Debug 编译；直接 make 也可以
make check                # 检查固件向量、HAL/RTOS 钩子与 Flash/RAM 范围
make flash                # 编译、固件检查、烧录、校验、复位运行

make BUILD=release -j4
make BUILD=release check
make BUILD=release flash
make clean                # 清理当前 Debug 构建目录
make BUILD=release clean
make help
```

默认产物：`build/make/debug/drone-remote.elf`、`.hex`、`.bin`、`.map`。Release 在 `build/make/release/`。`make` 自动生成编辑器使用的 `compile_commands.json`；应用代码开启严格编译告警，并跟踪头文件依赖。

工具不在 PATH 时：

```sh
make TOOLCHAIN_BIN=/path/to/arm-toolchain/bin
make flash OPENOCD=/path/to/openocd
```

烧录前核对遥控器芯片型号，并把 ST-Link 接到**遥控器板**：SWDIO → PA13、SWCLK → PA14、GND 共地，按板卡要求供电及连接探针参考电压。默认使用软件复位，NRST 可不接。

```sh
make -n flash             # 查看将执行的操作，不烧录
make flash                # 实际写入连接的目标板
```

看到 `Verified OK` 和复位成功，才表示本次写入、校验完成。OpenOCD 出错时 Make 返回非零状态，不会把失败报告成成功。如果程序导致普通连接失败，可接 NRST 并使用复位下连接配置：

```sh
make flash OPENOCD_CONFIG=Config/openocd-nrst.cfg
```

此备用配置复用相邻 `drone` 的复位连接方式，需要实际连接 NRST；遥控器上的适用性仍需上板验证。自定义探针也可通过 `OPENOCD_CONFIG=/path/to/probe.cfg` 覆盖。

## 运行后看什么

默认调试串口为 USART2：PA2 TX、PA3 RX，115200 / 8N1 / 无流控。USB-TTL 接收端 RX 接 PA2、GND 共地，使用 3.3 V 逻辑。板载串口是否连接这些引脚，需要核对原理图。

调度启动后每秒发送：

```text
remote: FreeRTOS running; input pending
```

`input pending` 表示摇杆驱动还没接，不是调度失败。当前没有 LED 引脚配置，因此不以 LED 闪烁作为运行依据。串口尚未经过遥控器实机验证。

| 任务 | 周期 | 优先级 | 当前职责 |
| --- | --- | --- | --- |
| `input` | 20 ms / 50 Hz | 2 | 调用摇杆、按键预留接口；更新输入状态和循环计数 |
| `monitor` | 1000 ms | 1 | 发送串口心跳；记录发送结果和任务剩余栈 |
| Idle | 空闲时 | 0 | FreeRTOS 自带空闲任务 |

通过调试器观察 `g_app_status`：

- `input_cycles`、`monitor_cycles` 应持续增加。
- `uart_tx_count` 应每秒增加，`uart_tx_errors` 正常为 0。
- `input_valid`、`board_pinmap_verified` 当前均为 0。
- `*_stack_free_words` 是历史最小剩余栈空间，单位为 32 位字，乘 4 得到字节。

`g_remote_input` 仅是调试用输入副本，当前四通道与按键均为 0，且标记无效，不会向飞机发送。任务间需要完整一致的数据时，应增加队列或加锁复制，不能把 `volatile` 当作同步机制。

任务栈与控制块均静态分配，无 `malloc`、`heap_x.c` 或 `printf`。串口由一个任务独占。两个业务任务各 256 字栈（1 KiB），Idle 为 128 字；中断主栈预留 2 KiB。接入真实驱动后需重新观察剩余栈。

RTOS 使用 SysTick 1 kHz；HAL 使用 TIM4 1 kHz，两者独立。TIM4 已保留为 HAL 时基，后续不能同时拿来驱动蜂鸣器或其他 PWM。时钟采用 HSI / 2 × 16 = 64 MHz，不依赖未知的外部晶振。断言、栈溢出和 CPU 异常记录在 `g_app_fault`，随后停机，记录在复位后清零。

## 后续接硬件的位置

```text
Core/          启动、时钟、中断、HAL 时基、USART2
App/           两个 FreeRTOS 任务、状态计数、故障记录
BSP/           遥控器引脚及输入驱动适配入口
Config/        板级参数、FreeRTOS 配置、OpenOCD 配置
Drivers/       ST HAL 与 CMSIS 源码及许可
Middlewares/   FreeRTOS V11.1.0 内核、Cortex-M3 GCC 移植层及许可
linker/        64 KiB Flash / 20 KiB RAM 链接脚本
scripts/       固件检查、编译数据库、锁定版本的依赖恢复
```

确认芯片型号后，再确认启动文件、芯片宏及链接脚本容量。引脚核对后，在 `BSP_Board_Init()` 初始化外设，在 `BSP_Remote_ReadInput()` 接入摇杆和按键。四通道采用油门 0..1000、横滚/俯仰/偏航 -1000..1000 的应用层单位；这不是无线线协议。实现驱动前该接口始终返回 `false`。

VS Code 已提供构建、检查、烧录任务；F5 使用 Cortex-Debug + OpenOCD，先构建再下载并停在 `main()`。这些入口尚未通过遥控器实机验证。

依赖来源及锁定版本见 [THIRD_PARTY.md](THIRD_PARTY.md)。通常不需要运行 `make deps`；该命令仅用于从官方仓库按固定提交及 SHA-256 恢复第三方源码。

验证范围见 [docs/validation.md](docs/validation.md)。
