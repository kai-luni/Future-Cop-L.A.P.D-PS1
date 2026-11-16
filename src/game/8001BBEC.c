// Categories: Game Engine > State Management
#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    u8 unk8;
    u8 pad[3];
    s32 unkC;
    s32 unk10;
} Unk800A0660;

extern s32 D_8009F5E8;
extern Unk800A0660 D_800A0660[];

/**
 *  @brief Writes five values into the next entry of a four-slot ring.
 *
 *  The current global index selects an entry in the shared table, after which
 *  the index advances and wraps to zero at four.
 *  @param arg0 Value stored in the entry's unk4 field.
 *  @param arg1 Value stored in the entry's unk0 field.
 *  @param arg2 Value stored in the entry's unkC field.
 *  @param arg3 Value stored in the entry's unk10 field.
 *  @param arg4 Byte stored in the entry's unk8 field.
 *  @par Globals
 *  D_800A0660 is the entry table; D_8009F5E8 is its current write index.
 *  @par Control flow
 *  The index is incremented and reset to zero if it reaches or exceeds four.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites one table entry and updates the global index.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8001BBEC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4)
{
    Unk800A0660 *base;
    Unk800A0660 *entry;

    base = D_800A0660;
    entry = base + D_8009F5E8;
    entry->unk8 = arg4;
    entry->unk4 = arg0;
    entry->unk0 = arg1;
    entry->unkC = arg2;
    entry->unk10 = arg3;

    D_8009F5E8++;
    if (D_8009F5E8 >= 4) {
        D_8009F5E8 = 0;
    }
}
