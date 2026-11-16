// Categories: Platform > Hardware Init
#include "game/types.h"

extern s32 D_8009DEFC;
extern s32 D_8009DF00;
extern s32 func_80091270(s32 arg0);

/**
 * @brief Arms a 0xF0-tick deadline for the transfer engine.
 *
 * The current tick (func_80091270(-1)) plus 0xF0 becomes D_8009DEFC, the expiry
 * stamp, and D_8009DF00 is cleared to mark "nothing pending". The pair is read
 * by the poll loops of the same block (func_80094F4C and neighbours).
 * @par Globals
 * D_8009DEFC - deadline; D_8009DF00 - pending marker.
 * @par Control flow
 * None.
 * @par Calls
 * func_80091270 - tick counter.
 * @par Side effects
 * Writes both words.
 * @par Callers
 * Confirmed: the hardware init block around func_80094F4C.
 */
void func_800951D8(void)
{
    s32 value;

    value = func_80091270(-1) + 0xF0;
    D_8009DEFC = value;
    D_8009DF00 = 0;
}
