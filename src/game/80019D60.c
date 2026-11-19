// Categories: Graphics > Display Setup
#include "game/types.h"

extern void gSetGeomOffsetAndH(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80010030");

/**
 * @brief Sets the display position to (160, 120) with parameter 0x200
 *        through the two-stage display setup.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Single call func_80010030(0xA0, 0x78, 0x200); func_80010030 forwards
 * (x, y) to func_800959F0 and then applies the third value as height.
 *
 * @par Likely role
 * Centering on a 320x240 screen (160, 120) with a 512-unit height field.
 *
 * @par Callers
 * func_80024A24.
 */
void func_80019D60(void)
{
    gSetGeomOffsetAndH(0xA0, 0x78, 0x200);
}
