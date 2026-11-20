// Categories: Platform > GPU | Graphics > GPU > Draw Environment
#include "../../include/game/types.h"

extern s32 gGpuDrawOffset __asm__("D_8009DD90");

s32 gSetGpuDrawOffset(s32 value) __asm__("func_80091F60");

/**
 * @brief Exchanges the library state's origin/drawing-area word.
 *
 * @param value New value for D_8009DD90.
 * @return The previous value.
 *
 * @par Confirmed
 * previous = D_8009DD90; D_8009DD90 = value; return previous. The getter
 * func_80091F74 documents the word as the library state's
 * origin/drawing-area field.
 *
 * @par Callers
 * func_8001BF78 (fade layers).
 */
s32 gSetGpuDrawOffset(s32 value)
{
    s32 previous;

    previous = gGpuDrawOffset;
    gGpuDrawOffset = value;
    return previous;
}
