// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009F628;

struct Unk21824 {
    u32 f0;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u32 f14;
    u32 f18;
    u16 f1C;
    u16 f1E;
    u32 f20;
    u32 f24;
};

/**
 *  @brief Sets a global state value to one.
 *
 *  This function provides a direct write to D_8009F628; the meaning of
 *  that state value is not established by the source.
 *  @par Globals
 *  D_8009F628 is assigned the value 1.
 *  @par Control flow
 *  Performs one unconditional assignment.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Changes D_8009F628.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80021888(void)
{
    D_8009F628 = 1;
}
