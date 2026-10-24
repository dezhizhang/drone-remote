#ifndef BSP_BOARD_H
#define BSP_BOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t throttle;  /* 0..1000 */
    int16_t roll;       /* -1000..1000 */
    int16_t pitch;
    int16_t yaw;
    uint8_t buttons;
} BSP_RemoteInput;

void BSP_Board_Init(void);
void BSP_Board_EnterSafeState(void);
bool BSP_Remote_ReadInput(BSP_RemoteInput *input);

#endif
