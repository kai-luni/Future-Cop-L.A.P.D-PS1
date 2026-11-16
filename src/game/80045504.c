// Categories: Game Engine > State
#include "game/types.h"

struct Unk7938 {
    u8 unk00[0x10];
    u8 active;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    s32 unk18;
};

extern struct Unk7938 D_800A7938[];

/**
 *  @brief Resets selected fields of an indexed state entry.
 *
 *  Clears the active flag and several other fields, sets unk13 to one,
 *  and sets unk16 to 0xFF in the selected table entry.
 *  @param index Index of the entry to update.
 *  @par Globals
 *  D_800A7938 is the array containing the modified entry.
 *  @par Control flow
 *  Performs the field assignments directly without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the selected entry; its other fields are left unchanged.
 *  @par Callers
 *  Confirmed: func_80044EF8.
 */
void func_80045504(s32 index)
{
    struct Unk7938 *entry;

    entry = &D_800A7938[index];
    entry->unk13 = 1;
    entry->active = 0;
    entry->unk15 = 0;
    entry->unk11 = 0;
    entry->unk12 = 0;
    entry->unk14 = 0;
    entry->unk16 = 0xFF;
    entry->unk18 = 0;
}
