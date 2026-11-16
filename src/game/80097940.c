// Categories: Platform > Bit Manipulation
#include "../../include/game/types.h"

extern u32 *D_8009EEB8;

/**
 *  @brief Updates bits 24 through 27 of a pointed-to word.
 *
 *  Clears that four-bit field and sets it to 0x2, preserving the other bits.
 *  The purpose of the word is not established by this function.
 *  @par Globals
 *  D_8009EEB8 points to the word being updated.
 *  @par Control flow
 *  Performs one unconditional read-modify-write operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the word addressed by D_8009EEB8.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80097940(void)
{
    *D_8009EEB8 = (*D_8009EEB8 & 0xF0FFFFFFU) | 0x20000000U;
}
