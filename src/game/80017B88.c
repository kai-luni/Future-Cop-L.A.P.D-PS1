// Categories: Graphics > Model Renderer > Detail Level
#include "game/types.h"

extern s32 D_8009F594;
extern s32 D_8009F5A0;

/**
 * @brief Detail-level predicate: compares two global quality words.
 *
 * @return 1 when D_8009F594 >= D_8009F5A0, else 0.
 *
 * @par Confirmed
 * Pure comparison of the two globals, no side effects.
 *
 * @par Likely role
 * The high-detail gate of the effect code: func_8004B5F8 halves its
 * fragment budget when this returns 1, func_80043430 branches on it —
 * the pair likely encodes the current detail setting against a threshold.
 *
 * @par Unknown
 * Which global is the setting and which the threshold.
 *
 * @par Callers
 * func_80043430, func_8004B5F8.
 */
s32 func_80017B88(void)
{
    return D_8009F594 >= D_8009F5A0;
}
