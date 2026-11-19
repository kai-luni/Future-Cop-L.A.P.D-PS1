// Categories: Platform > CD-ROM | Platform > Memory Card
#include "../../include/game/types.h"

extern s32 gCdReadyCallback __asm__("D_8009C8AC");
s32 gSetCdReadyCallback(s32 value) __asm__("func_8008E1A8");


/**
 * @brief Swaps the CD/card data-ready callback word D_8009C8AC and returns the previous handler.
 *
 * Installs a new handler pointer into D_8009C8AC (the sibling slot to D_8009C8A8 managed by func_8008E194) so drivers can chain sector/data-ready callbacks and restore the prior one afterward.
 *
 * @param value New callback pointer (or 0 to clear).
 * @return Previous callback value held in D_8009C8AC.
 *
 * @par Globals
 * D_8009C8AC - data-ready callback word, read then overwritten.
 *
 * @par Control flow
 * Straight-line load, store, and return; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites D_8009C8AC, immediately changing the target invoked on data-ready events.
 *
 * @par Callers
 * Confirmed: func_8001C808, func_8001D130, gCdStreamService, func_800908D4, func_80090B48, func_80090C18, func_80090E00, func_80090FA0, and func_80089018 (0x8008907C, 0x800890D8).
 *
 * @par Role and path
 * Saves the current callback pointer from D_8009C8AC, writes the new callback
 * value into D_8009C8AC, and returns the previous pointer. Driven by CD/file
 * loaders including func_8001C808, func_8001D130, gCdStreamService, and
 * func_800908D4..func_80090FA0.
 */
s32 gSetCdReadyCallback(s32 value)
{
    s32 previous;

    previous = gCdReadyCallback;
    gCdReadyCallback = value;
    return previous;
}
