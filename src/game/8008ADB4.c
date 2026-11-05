#include "game/types.h"

extern u32 *D_8009C788;
extern u8 *D_8009C78C;

extern s32 func_8008C8B0(void);

s32 func_8008ADB4(void)
{
    u32 *counter = D_8009C788;
    u8 *state = D_8009C78C;

    *counter = 0xFFFFFF7F;

    if (*(volatile u16 *)(state + 4) & 0x80) {
        for (;;) {
            if (func_8008C8B0()) {
                return 0;
            }
            if (!(*(volatile u16 *)(D_8009C78C + 4) & 0x80)) {
                break;
            }
        }
    }

    *(volatile u16 *)(D_8009C78C + 0xA) |= 0x10;

    return 1;
}
