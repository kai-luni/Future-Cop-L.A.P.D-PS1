// Categories: Platform > Hardware Registers
#include "game/types.h"

extern u8 *D_8009CB6C;
extern u8 *D_8009CB70;
extern u8 *D_8009CB74;
extern u8 *D_8009CB78;

/**
 *  @brief Writes a four-byte command sequence through indexed register pointers.
 *
 *  Selects register bank 2 for the first two input bytes, then bank 3 for
 *  the remaining bytes and a final constant command value.
 *  @param arg0 Pointer to at least four input bytes.
 *  @return Always zero.
 *  @par Globals
 *  D_8009CB6C selects the bank; D_8009CB70, D_8009CB74, and D_8009CB78
 *  point to the byte locations written in the selected banks.
 *  @par Control flow
 *  Performs the writes in a fixed sequence without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the bank selector and register locations, ending with 0x20.
 *  @par Callers
 *  Confirmed: func_8008E578.
 */
s32 func_8008F650(u8 *arg0)
{
    *D_8009CB6C = 2;
    *D_8009CB74 = arg0[0];
    *D_8009CB78 = arg0[1];

    *D_8009CB6C = 3;
    *D_8009CB70 = arg0[2];
    *D_8009CB74 = arg0[3];
    *D_8009CB78 = 0x20;

    return 0;
}
