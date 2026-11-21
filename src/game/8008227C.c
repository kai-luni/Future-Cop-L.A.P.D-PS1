// Categories: Game Engine > State
#include "game/types.h"

extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern u8 *gSaveProfileImage __asm__("D_8009F92C");

/**
 *  @brief Writes a fixed 16-bit value into a global state buffer.
 *
 *  Stores 0x385E at byte offset 0x942 from the buffer pointer; the source
 *  does not identify the meaning of the field or the stored value.
 *  @par Globals
 *  D_8009F92C supplies the base address of the destination buffer.
 *  @par Control flow
 *  Performs one unconditional halfword store and returns.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the 16-bit field at D_8009F92C + 0x942.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008227C(void)
{
    *(u16 *)(gSaveProfileImage + 0x942) = 0x385E;
}
s32 gGetProfileSlotEditState(s32 arg0) __asm__("D_8008228C");


s32 gGetProfileSlotEditState(s32 arg0)
{
    s32 result;

    result = 0;
    if (gPlayerMenuDepth[arg0] != 0) {
        result = (gFontBankTable[arg0] < 10) << 1;
    }
    return result;
}
