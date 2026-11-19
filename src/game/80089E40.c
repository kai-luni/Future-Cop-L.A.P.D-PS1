// Categories: Game Engine > State
#include "game/types.h"

extern s32 gPadExtEnableFlag __asm__("D_8009C774");
extern u8 *gPadBasePort __asm__("D_8009C75C");
s32 gIsPadPortState8(s32 arg0) __asm__("func_80089E40");


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
 *
 * @par Role and path
 * Returns 0 if D_8009C774 is zero; otherwise indexes the 240-byte pad port
 * array gPadBasePort by (arg0 >> 4) and returns whether byte +0xE8 equals 8.
 * No direct callers are present in the codebase.
 */
s32 gIsPadPortState8(s32 arg0)
{
    s32 result;

    if (gPadExtEnableFlag == 0) {
        result = 0;
    } else {
        result = gPadBasePort[(arg0 >> 4) * 240 + 0xE8] == 8;
    }
    return result;
}
