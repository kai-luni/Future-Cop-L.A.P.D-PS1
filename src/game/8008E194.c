// Categories: Platform > CD-ROM | Platform > Memory Card
#include "../../include/game/types.h"

extern s32 D_8009C8A8;

/**
 * @brief Swaps the driver callback held in D_8009C8A8 and returns the old one.
 *
 * The CD/card driver keeps its registered callback in this single word (the sibling slot D_8009C8AC is handled by func_8008E1A8); the memory-card loader chains its own handler in front of the driver default and restores the previous value when the operation ends.
 *
 * @param value Callback address to install; 0 clears the slot.
 * @return The address that was installed before this call.
 *
 * @par Globals
 * D_8009C8A8 - the driver callback word; read for the return value and overwritten with the argument, without validating it.
 *
 * @par Control flow
 * Straight line, no branches: the load and the store are the whole body.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * D_8009C8A8 is overwritten on every call, so the driver immediately dispatches to the new handler.
 *
 * @par Callers
 * Confirmed: func_8001C614, func_8001C718 (0x8001C7B4) and func_8001CE54 (0x8001D018, 0x8001D058) install and restore the card completion handler; func_800895E4, func_800908A0, func_800908D4, func_80090B48, func_80090C18, func_80090E00 and func_80090FA0 also call it.
 * Unknown: two further calls at 0x80089074 and 0x800890CC from a driver routine that is not in the matching set.
 */
s32 func_8008E194(s32 value)
{
    s32 previous;

    previous = D_8009C8A8;
    D_8009C8A8 = value;
    return previous;
}
