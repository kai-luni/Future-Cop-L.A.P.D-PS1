// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009C774;
extern u8 *D_8009C75C;

/**
 *  @brief Tests whether a selected table entry has the value eight.
 *
 *  The lookup is gated by a global flag and uses the shifted argument to
 *  select a 240-byte record, reading its byte at offset 0xE8.
 *  @param arg0 Value shifted right by four to select the record.
 *  @return One if the flag is nonzero and the selected byte is eight; otherwise zero.
 *  @par Globals
 *  D_8009C774 gates the lookup; D_8009C75C points to the record table.
 *  @par Control flow
 *  Returns zero when the flag is clear; otherwise compares the selected byte.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the table index is not validated locally.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80089E40(s32 arg0)
{
    s32 result;

    if (D_8009C774 == 0) {
        result = 0;
    } else {
        result = D_8009C75C[(arg0 >> 4) * 240 + 0xE8] == 8;
    }
    return result;
}
