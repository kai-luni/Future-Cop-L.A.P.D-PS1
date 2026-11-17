// Categories: Platform > Memory Card
#include "game/types.h"

struct CardContext {
    s32 unk00;
    s32 slots[16];
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");

extern void func_800207CC(void);

/**
 * @brief Returns the memory-card slot index that holds the given game id.
 *
 * Slots 0..15 of the card context are searched by their stored id; an empty
 * slot triggers func_800207CC (slot scan/refresh) and the value is read again.
 * Out-of-range slot numbers return -1.
 * @param slot Requested slot number.
 * @return The id stored in that slot, or -1 when out of range.
 * @par Globals
 * D_8009AB08 - card context (16 slot words).
 * @par Control flow
 * Bounds test, one refresh, one read.
 * @par Calls
 * func_800207CC - slot scan.
 * @par Side effects
 * May refresh the slot table.
 * @par Callers
 * Confirmed: the memory-card save/load paths (80020xxx block).
 */
s32 func_80020DF8(u32 slot)
{
    if (slot < 16) {
        if (gOperationState->slots[slot] == 0) {
            func_800207CC();
        }
        return gOperationState->slots[slot];
    }
    return -1;
}
