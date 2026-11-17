// Categories: Game Engine > Menu System | Graphics > Text
#include "game/types.h"

extern u8 *gFontBankTable __asm__("D_8009F468");
extern s8 gPanelSyncValue __asm__("D_8009F8BC");
extern s32 func_80048E28(s32 index);

/**
 * @brief Synchronizes the active font bank to the current menu panel byte at offset +8.
 *
 * Confirmed: reads signed byte index = *(s8 *)(D_8009F468 + 8) from the active menu struct and compares it with cached D_8009F8BC; if different, stores D_8009F468[8] into D_8009F8BC and calls func_80048E28(index).
 * Category follows active menu pointer D_8009F468, front-end callers, and font-bank selector func_80048E28 (which indexes D_800A19A0 and calls func_80048DEC).
 * Likely: avoids redundant font-bank rebinding across menu transitions and per-frame UI ticks unless the active panel specifies a different font bank; Unknown: none.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F468 - active menu structure pointer (reads byte at +8); D_8009F8BC - cached font bank index (updated on change).
 * @par Control flow
 * Single equality guard previous == index; skips store and call when unchanged.
 * @par Calls
 * func_80048E28 - selects font bank D_800A19A0[index] and calls func_80048DEC when index < D_8009F364.
 * @par Side effects
 * Updates D_8009F8BC and switches active font bank globals D_8009F76C/D_8009F35C via func_80048E28 when D_8009F468[8] changes.
 * @par Callers
 * Confirmed: func_800778E4 (0x8007792C), func_80078650 (0x80078674), func_80078740 (0x80078798 in asm/us/main.s), func_80078DD0 (0x800790D8 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_800788B4(void)
{
    register u8 *state __asm__("$2");
    register s32 previous __asm__("$3");
    register s32 index __asm__("$4");

    state = gFontBankTable;
    previous = gPanelSyncValue;
    index = *(s8 *)(state + 8);
    if (previous == index) {
        goto done;
    }
    gPanelSyncValue = state[8];
    func_80048E28(index);
done:
    ;
}
