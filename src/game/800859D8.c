// Categories: Graphics > Video Decode Output
#include "game/types.h"

extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;
extern s32 D_8009F4CC;

/**
 * @brief Selects the decoder's output scaling mode.
 *
 * @param arg0 Non-zero selects the "3-step" mode (D_8009F4C0 = 1, column step
 *             D_8009F4C8 = 3, scale enable D_8009F4C4 = 1); zero selects the
 *             "2-step" mode (2 / 2 / 0).
 *
 * @return Nothing. D_8009F4CC is always set to 2 (the number of buffers).
 *
 * @par Globals
 * D_8009F4C0 (mode), D_8009F4C4 (scale enable), D_8009F4C8 (column step),
 * D_8009F4CC (buffer count).
 *
 * @par Callers
 * func_800861E4 (with mode != 0) and func_80087EA0 (data-dependent).
 */
void func_800859D8(s32 arg0)
{
    if ((arg0 & 0xFF) != 0) {
        D_8009F4C0 = 1;
        D_8009F4C8 = 3;
        D_8009F4C4 = 1;
    } else {
        D_8009F4C0 = 2;
        D_8009F4C8 = 2;
        D_8009F4C4 = 0;
    }
    D_8009F4CC = 2;
}
