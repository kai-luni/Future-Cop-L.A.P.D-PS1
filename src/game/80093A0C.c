// Categories: Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern u32 gPackDrawOffset(u32 x, u32 y) __asm__("func_800940A8");

/**
 * @brief Builds the "drawing offset" primitive (GP0 0xE5) for a rectangle
 *        origin.
 *
 * @param obj Target packet; obj[3] receives the command length 2.
 * @param rect Source rectangle (only x and y are used).
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * obj[4] = func_800940A8(x, y) (the packed offset word) and obj[8] = 0.
 *
 * @par Callers
 * func_8001B4A8, right after func_8009398C, to place the drawing origin.
 */
void func_80093A0C(u8 *obj, s16 *rect)
{
    obj[3] = 2;
    *(s32 *)(obj + 4) = gPackDrawOffset(*(s16 *)(rect + 0), rect[1]);
    *(s32 *)(obj + 8) = 0;
}
