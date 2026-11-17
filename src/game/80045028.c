// Categories: Game Engine > State
#include "game/types.h"

extern s32 gVolumeLevels __asm__("D_800A1800");

/**
 *  @brief Tests whether a mode matches the active global state.
 *
 *  The sentinel state -1 never matches, even when mode is also -1.
 *  Otherwise the result reflects equality with the stored state.
 *  @param mode Mode value to compare with the global state.
 *  @return 1 if the state is not -1 and equals mode; otherwise 0.
 *  @par Globals
 *  D_800A1800 supplies the state to test.
 *  @par Control flow
 *  Checks the sentinel and equality before selecting the return value.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80045164.
 */
s32 func_80045028(s32 mode)
{
    s32 state = gVolumeLevels;

    if (state != -1 && mode == state) {
        return 1;
    }
    return 0;
}
