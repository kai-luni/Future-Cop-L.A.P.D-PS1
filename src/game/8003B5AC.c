// Categories: Game Engine > Data Lookup
#include "game/types.h"

struct Item {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
};

struct Entry {
    u8 pad00[4];
    struct Item items[1];
};

extern struct Entry *gSlotSpawnDescs[] __asm__("D_800A0C70");

/**
 *  @brief Reads a mode-selected byte from an indexed entry item.
 *
 *  The entry pointer table is checked for a null entry before item access.
 *  Mode 0 selects byte 3, mode 2 selects byte 1, and mode 3 returns zero.
 *  Mode 1 and all other modes select byte 2.
 *  @param index Index into the entry pointer table.
 *  @param slot Index of the item within the selected entry.
 *  @param mode Selector for the item byte.
 *  @return The selected byte, or zero for a null entry or mode 3.
 *  @par Globals
 *  D_800A0C70 supplies the indexed entry pointer.
 *  @par Control flow
 *  Checks the entry, then dispatches by mode without validating either index.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None directly.
 *  @par Callers
 *  Confirmed: func_8003BAE4.
 */
u8 func_8003B5AC(u8 index, u8 slot, s32 mode)
{
    struct Entry *entry = gSlotSpawnDescs[index];

    if (entry == 0) {
        return 0;
    }
    if (mode == 1) {
        goto tail;
    }
    if (mode == 0) {
        goto c0;
    }
    if (mode == 2) {
        goto c2;
    }
    if (mode == 3) {
        return 0;
    }
    goto tail;
c0:
    return entry->items[slot].unk3;
c2:
    return entry->items[slot].unk1;
tail:
    return gSlotSpawnDescs[index]->items[slot].unk2;
}
