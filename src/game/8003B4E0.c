// Categories: Gameplay > Map | Game Engine > Object Setup
#include "game/types.h"

struct Slot {
    u8 pad00[4];
    u8 value;
    u8 pad05[3];
};

struct Entry {
    u8 pad00[0x18];
    u16 flags;
    u8 column;
};

extern struct Entry *D_800A0C70[];
extern s32 func_80019668(void);

/**
 * @brief Resolves a map occupancy column byte from D_800A0C70 for a region and slot.
 *
 * Normally returns slot byte +0x04 + (slot << 2) of D_800A0C70[index], but when flag bit 0
 * (+0x18) is clear, slot is 0, mode is non-zero, and RNG bit 0 is set, returns entry->column (+0x1A).
 *
 * @param index Region index into D_800A0C70.
 * @param slot Column slot index (0..4) into the entry's 4-byte stride array.
 * @param mode Non-zero enables the random fallback to entry->column for slot 0.
 * @return Resolved column byte from the D_800A0C70[index] entry.
 *
 * @par Globals
 * D_800A0C70 - array of map region entry pointers.
 * @par Control flow
 * Nested guard on !(entry->flags & 1), slot == 0 && mode != 0, and (func_80019668() & 1), returning entry->column or slot byte.
 * @par Calls
 * func_80019668 - random number generator tested on bit 0 when mode != 0 and slot == 0.
 * @par Side effects
 * Advances RNG state via func_80019668 on the slot-0 special path.
 * @par Callers
 * Confirmed: func_8003B898 (multi-column map occupancy registration).
 */
u8 func_8003B4E0(u8 index, u8 slot, u8 mode)
{
    if (!(D_800A0C70[index]->flags & 1)) {
        if (slot == 0 && mode != 0) {
            if (func_80019668() & 1) {
                return D_800A0C70[index]->column;
            }
        }
    }
    return ((u8 *)D_800A0C70[index] + ((s32)slot << 2))[4];
}
