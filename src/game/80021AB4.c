// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct FlagState {
    u8 pad_00[0x18];
    u32 flags;
};

extern struct FlagState D_800A07A8;

/**
 *  @brief Adds bits to a shared flag field.
 *
 *  ORs the supplied mask into the field at offset 0x18 of D_800A07A8.
 *  Existing bits in that field remain set.
 *  @param flags Bit mask to add to the field.
 *  @par Globals
 *  D_800A07A8.flags is read and updated.
 *  @par Control flow
 *  Performs one unconditional read-modify-write operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the flags field of D_800A07A8.
 *  @par Callers
 *  Confirmed: func_80021F3C.
 */
void func_80021AB4(u32 flags)
{
    D_800A07A8.flags |= flags;
}
