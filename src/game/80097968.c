// Categories: Platform > Bit Manipulation
#include "../../include/game/types.h"

extern u32 *D_8009EEB8;

/**
 *  @brief Updates the high byte of a pointed-to word.
 *
 *  Clears bits 24 through 27, sets bit 25, and sets bit 29.
 *  All other bits are preserved; the word's purpose is not established here.
 *  @par Globals
 *  D_8009EEB8 points to the word being updated.
 *  @par Control flow
 *  Performs one unconditional read-modify-write operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the word addressed by D_8009EEB8.
 *  @par Callers
 *  Confirmed: func_800973B4.
 */
void func_80097968(void)
{
    *D_8009EEB8 = (*D_8009EEB8 & 0xF0FFFFFFU) | 0x22000000U;
}
