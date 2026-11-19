// Categories: Platform > Hardware Registers
#include "game/cdrom_globals.h"

#include "game/types.h"

extern u8 *gCdromIndexStatusPtr __asm__("D_8009CB6C");
extern u8 *gCdromCommandPtr __asm__("D_8009CB70");
extern u8 *gCdromParameterPtr __asm__("D_8009CB74");
extern u8 *gCdromRequestStatusPtr __asm__("D_8009CB78");
s32 gWriteCdAudioMixRegs(u8 *arg0) __asm__("func_8008F650");


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
 *
 * @par Role and path
 * Selects CD-ROM register bank 2 via *gCdromIndexStatusPtr (D_8009CB6C) to
 * write arg0[0..1] to *gCdromParameterPtr and *gCdromRequestStatusPtr, then
 * selects bank 3 to write arg0[2..3] and 0x20 to apply the audio mix. Returns
 * 0 and is driven by func_8008E578.
 */
s32 gWriteCdAudioMixRegs(u8 *arg0)
{
    *gCdromIndexStatusPtr = 2;
    *gCdromParameterPtr = arg0[0];
    *gCdromRequestStatusPtr = arg0[1];

    *gCdromIndexStatusPtr = 3;
    *gCdromCommandPtr = arg0[2];
    *gCdromParameterPtr = arg0[3];
    *gCdromRequestStatusPtr = 0x20;

    return 0;
}
