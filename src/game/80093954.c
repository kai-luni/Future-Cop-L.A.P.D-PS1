// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
#include "game/types.h"

extern u32 func_800940C4(u8 *p);

/**
 * @brief Builds a 2-word GP0 texture-window primitive packet (PsyQ SetTexWindow).
 *
 * Sets the GPU packet length byte at obj[3] to 2, writes the packed GP0 0xE2 texture-window
 * command word returned by func_800940C4(arg) at obj+4, and clears the trailing word at obj+8.
 *
 * @param obj Destination 12-byte GPU primitive buffer (length byte at +3, command words at +4/+8).
 * @param arg Pointer to an 8-byte texture-window rectangle passed to func_800940C4, or 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line packet initialization with one call to func_800940C4.
 *
 * @par Calls
 * func_800940C4 - packs the 8-byte rectangle into a GP0 0xE2 command word (or returns 0 for null).
 *
 * @par Side effects
 * Writes bytes/words at obj[3], *(u32 *)(obj + 4), and *(u32 *)(obj + 8).
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s / Likely: sibling of func_8009398C / Unknown: whether invoked indirectly or unreferenced PsyQ libgpu unit.
 */
void func_80093954(u8 *obj, u8 *arg)
{
    obj[3] = 2;
    *(u32 *)(obj + 4) = func_800940C4(arg);
    *(u32 *)(obj + 8) = 0;
}
