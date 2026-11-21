// Categories: Gameplay > Entity Update | Utilities > Memory
#include "game/types.h"

extern u8 gSourceBlock20[] __asm__("D_8009AFA0");

extern void gCopyMemory(u8 *destination, u8 *source, s32 size) __asm__("func_8001E318");
void gCopyDefaultGaugeRecord(u8 *destination) __asm__("func_8005A09C");


/**
 * @brief Copies the 20-byte default parameter template D_8009AFA0 into destination.
 *
 * Calls func_8001E318(destination, D_8009AFA0, 0x14) to populate a 20-byte block (words 0x12C, 0x12C, 0x51EB, 0x64, 0x0E140002) used when initializing entity gauge/sub-object data.
 *
 * @param destination Target buffer of at least 20 (0x14) bytes.
 *
 * @par Globals
 * D_8009AFA0 - 20-byte default template block read as copy source.
 *
 * @par Control flow
 * Single call to func_8001E318 with fixed source and byte count 0x14; no branches or loops.
 *
 * @par Calls
 * func_8001E318 - aligned memory copy.
 *
 * @par Side effects
 * Overwrites 20 bytes at destination.
 *
 * @par Callers
 * Confirmed: func_800331CC (0x800335D0).
 */
void gCopyDefaultGaugeRecord(u8 *destination)
{
    gCopyMemory(destination, gSourceBlock20, 0x14);
}
