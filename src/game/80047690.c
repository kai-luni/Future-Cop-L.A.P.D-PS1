// Categories: Game Engine > Frame | Graphics > Display
#include "game/types.h"

extern u8 D_8009F739;

extern void func_8002A16C(s32 value);
extern void func_80047554(s32 value);

/**
 * @brief Enables or disables the text/HUD layer around a draw section.
 *
 * @param enable Non-zero enables, zero disables.
 * @return Nothing.
 *
 * @par Confirmed
 * Enable sets D_8009F739 = 1 and calls func_8002A16C(1); disable calls
 * func_80047554(0). No other state is touched.
 *
 * @par Likely role
 * The bracket the frame drivers (func_800259D8, func_80025B7C,
 * func_80027510) and the debug box func_80082DDC use to switch the text
 * layer on for their overlays and back off afterwards.
 *
 * @par Unknown
 * What func_8002A16C/func_80047554 toggle beyond the layer flag.
 *
 * @par Callers
 * func_800259D8, func_80025B7C, func_80027510, func_80082DDC.
 */
void func_80047690(u8 enable)
{
    if (enable) {
        D_8009F739 = 1;
        func_8002A16C(1);
    } else {
        func_80047554(0);
    }
}
