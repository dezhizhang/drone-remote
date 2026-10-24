#include "bsp_board.h"

void BSP_Board_Init(void)
{
    /* 等待遥控器原理图：这里接入 ADC、按键、LED、无线模块初始化。 */
}

bool BSP_Remote_ReadInput(BSP_RemoteInput *input)
{
    /* 占位接口：不把默认零值当作有效摇杆数据，也不向飞行器发包。 */
    *input = (BSP_RemoteInput){0};
    return false;
}

void BSP_Board_EnterSafeState(void)
{
    /* 尚未驱动无线或其他外设；故障处理可在中断关闭时调用此函数。 */
}
