// Categories: Game Engine > Entity Tables
#include "game/types.h"

struct Unk4F0F0Entry {
    s32 unk00;
    u8 flags;
    u8 unk05[7];
};

extern struct Unk4F0F0Entry *D_800A2210[];
extern s32 gPlacementSlotCount __asm__("D_8009F7B8");

/**
 *  @brief Sets or clears bit 1 of a table entry's flags.
 *
 *  Selects a table by index and changes the requested entry only for modes
 *  zero and one; the entry index itself is not bounds-checked.
 *  @param index Table index, checked against the global table count.
 *  @param mode Zero clears the bit, one sets it; other values do nothing.
 *  @param entry Index of the entry within the selected table.
 *  @par Globals
 *  D_800A2210 supplies table pointers; D_8009F7B8 bounds the table index.
 *  @par Control flow
 *  Rejects invalid indices or modes at least 0x100, then dispatches on mode.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May modify the selected entry's flags byte.
 *  @par Callers
 *  Confirmed: func_80069D68.
 */
void func_8004F0F0(s32 index, u32 mode, s32 entry)
{
    u8 *flag;
    u8 value;
    struct Unk4F0F0Entry *table;

    if (mode < 0x100 && index >= 0 && index < gPlacementSlotCount) {
        table = D_800A2210[index];
        if (mode == 0) {
            goto clear;
        }
        if (mode == 1) {
            goto set;
        }
        goto done;
clear:
        flag = &table[entry].flags;
        value = *flag & ~2;
        goto store;
set:
        flag = &table[entry].flags;
        value = *flag | 2;
store:
        *flag = value;
    }
done:
    return;
}
