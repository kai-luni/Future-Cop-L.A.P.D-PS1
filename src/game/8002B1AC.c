// Categories: Gameplay > Entity Update | AI > Command Processing
#include "game/types.h"
void gSyncCountdownFields(u8 *arg0) __asm__("func_8002B1AC");


/**
 * @brief Syncs a class-26 actor's +0x10C field with +0x158 and reloads countdown +0x10E.
 *
 * Compares the signed halfword at arg0 + 0x10C against arg0 + 0x158; if different, copies the unsigned halfword from +0x158 into +0x10C, then always copies +0x10C into +0x10E.
 *
 * @param arg0 Class-26 actor block (+0x10C, +0x10E, +0x158 halfwords).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Single conditional update of +0x10C on mismatch with +0x158, followed by unconditional copy to +0x10E.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes halfwords at arg0 + 0x10C (conditionally) and arg0 + 0x10E.
 *
 * @par Callers
 * Confirmed: func_8002B1DC (0x8002B350, 0x8002B44C), func_8002B4B4 (0x8002B540), func_8002B784 (0x8002BD44, 0x8002BDFC), func_8002BFB4 (0x8002C0E0).
 */
void gSyncCountdownFields(u8 *arg0)
{
    s16 old_value = *(s16 *)(arg0 + 0x10C);
    s16 new_value_signed = *(s16 *)(arg0 + 0x158);
    u16 new_value_unsigned = *(u16 *)(arg0 + 0x158);

    if (old_value != new_value_signed) {
        *(u16 *)(arg0 + 0x10C) = new_value_unsigned;
    }

    *(u16 *)(arg0 + 0x10E) = *(u16 *)(arg0 + 0x10C);
}
