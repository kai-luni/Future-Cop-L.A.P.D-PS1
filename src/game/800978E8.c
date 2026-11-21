// Categories: Platform > Register Configuration
#include "game/spu_globals.h"

extern unsigned int *gSpuDelayCtrlPtr __asm__("D_8009EEB4");
void gSetSpuDelayTimingMode(int arg0) __asm__("func_800978E8");


/**
 *  @brief Selects one of two bit patterns in a pointed-to control word.
 *
 *  Bits 16 through 18 are cleared and then set to either 0x30000 or 0x50000,
 *  while all other bits retain their previous values.
 *  @param arg0 Nonzero selects 0x30000; zero selects 0x50000.
 *  @par Globals
 *  D_8009EEB4 points to the word being modified.
 *  @par Control flow
 *  A conditional selects the pattern after the common mask operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the word addressed by D_8009EEB4.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gSetSpuDelayTimingMode(int arg0)
{
    *gSpuDelayCtrlPtr &= 0xFFF8FFFFU;
    if (arg0) {
        *gSpuDelayCtrlPtr |= 0x30000U;
    } else {
        *gSpuDelayCtrlPtr |= 0x50000U;
    }
}
