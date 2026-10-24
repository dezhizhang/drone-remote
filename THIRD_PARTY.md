# 第三方源码

本工程使用相邻 `drone` 已有的供应商源码副本，保留各目录中的上游许可。以下为固定版本，不随上游分支自动更新。

| 依赖 | 版本 / 固定提交 | 官方仓库 | 本地许可 |
| --- | --- | --- | --- |
| CMSIS Core | STM32CubeF1 v1.8.7 / `d12e75247d5bcedc734f829b394517ab4c2726e3` | https://github.com/STMicroelectronics/STM32CubeF1 | `Drivers/CMSIS/LICENSE.txt` |
| STM32F1 HAL | CubeF1 v1.8.7 gitlink / `fee494a92b5ad331f92ad21f76c66a5cb83773ee` | https://github.com/STMicroelectronics/stm32f1xx-hal-driver | `Drivers/STM32F1xx_HAL_Driver/LICENSE.md` |
| CMSIS Device F1 | CubeF1 v1.8.7 gitlink / `c8e9a4a4f16b6d2cb2a2083cbe5161025280fb22` | https://github.com/STMicroelectronics/cmsis-device-f1 | `Drivers/CMSIS/Device/ST/STM32F1xx/License.md` |
| FreeRTOS Kernel | V11.1.0 / `dbf70559b27d39c1fdb68dfb9a32140b6a6777a0` | https://github.com/FreeRTOS/FreeRTOS-Kernel | `Middlewares/Third_Party/FreeRTOS/LICENSE.md` |

完整恢复规则及归档 SHA-256 位于 `scripts/dependencies.lock.json`，恢复入口为 `make deps`。构建仅使用 HAL 必需模块以及 FreeRTOS tasks / list / queue / ARM_CM3 port，不使用动态内存管理器。
