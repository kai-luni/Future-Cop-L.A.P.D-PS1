// Categories: Game Engine > State
#include "game/types.h"

extern s32 *gDecodeJob __asm__("D_8009F4BC");

/**
 *  @brief Stores a value in a field of the globally referenced object.
 *
 *  The destination is the 32-bit field at byte offset 0x114 from the
 *  pointer held in D_8009F4BC.
 *  @param value Value to store in the field.
 *  @par Globals
 *  D_8009F4BC supplies the base pointer for the write.
 *  @par Control flow
 *  Performs one unconditional store.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the 32-bit field at the computed address.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800858F4(s32 value)
{
    *(s32 *)((u8 *)gDecodeJob + 0x114) = value;
}
