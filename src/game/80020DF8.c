// Categories: Platform > Memory Card
#include "game/types.h"

struct CardContext {
    s32 unk00;
    s32 slots[16];
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");

extern void gParseCardDirectory(void) __asm__("func_800207CC");

s32 gReadCardSlotState(u32 slot) __asm__("func_80020DF8");

/**
 * @brief Returns the state of one card slot, refreshing the directory when its
 * cached state is zero.
 *
 * @param slot Slot index from 0 through 15.
 * @return The slot state, or -1 for an out-of-range index.
 *
 * @par Confirmed mechanics
 * Rejects unsigned indices of 16 or more. Reads the state word at manager +4
 * plus index times four, calls gParseCardDirectory if it was zero, then
 * reloads it.
 *
 * @par Globals
 * Reads gOperationState and the indexed state word.
 *
 * @par Callers
 * Direct calls in the retail listing are from gFindFreeCardBlockRun.
 *
 * @par Role and path
 * The free-run scan uses this accessor to distinguish state 2 from other slot
 * states.
 *
 * @par Unknown
 * Other meanings of the status words belong to the parser and card manager.
 */
s32 gReadCardSlotState(u32 slot)
{
    if (slot < 16) {
        if (gOperationState->slots[slot] == 0) {
            gParseCardDirectory();
        }
        return gOperationState->slots[slot];
    }
    return -1;
}
