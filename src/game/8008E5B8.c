// Categories: Platform > CD-ROM | Platform > DMA
#include "game/types.h"

extern s32 func_8008FD30(void *destination, s32 size);

/**
 * @brief Transfers a CD-ROM sector block via DMA3 and returns success.
 *
 * Calls func_8008FD30(destination, size) to program CD-ROM control and DMA3 registers (D_8009CB6C..D_8009CBB0) and wait for the data-ready flag (bit 0x40), returning 1 when func_8008FD30 returns 0 (Sony libcd CdGetSector pattern).
 *
 * @param destination Target RAM address for the DMA3 transfer.
 * @param size Transfer count in 32-bit words (low 16 bits for DMA3 BCR).
 * @return 1 if func_8008FD30 returns 0 (always true), 0 otherwise.
 *
 * @par Globals
 * None directly; func_8008FD30 accesses CD and DMA3 register pointers D_8009CB6C..D_8009CBB0.
 *
 * @par Control flow
 * Single call to func_8008FD30 followed by boolean comparison against 0.
 *
 * @par Calls
 * func_8008FD30 - configures CD/DMA3 hardware and transfers size words to destination.
 *
 * @par Side effects
 * Triggers a CD-ROM to RAM DMA3 write into destination via func_8008FD30.
 *
 * @par Callers
 * Confirmed: func_800908D4 (0x80090948 for 3-word sector header, 0x800909D4 for sector payload) / Likely: none / Unknown: none.
 */
s32 func_8008E5B8(void *destination, s32 size)
{
    return func_8008FD30(destination, size) == 0;
}
