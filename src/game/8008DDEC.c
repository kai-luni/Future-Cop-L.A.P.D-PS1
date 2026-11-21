// Categories: BIOS > Kernel Patch | Platform > Memory Card
#include "game/types.h"

extern u32 func_8008DC40[];
extern u32 gCardPatchJumpStubStart[] __asm__("D_8008DCC0");
void gCopyKernelPatch0xDF80(void) __asm__("func_8008DDEC");


/**
 * @brief Copies the 0x70-byte BIOS memory-card and pad patch block to kernel RAM at 0xDF80.
 *
 * Copies 32-bit words from func_8008DC40 + 4 words (0x8008DC50, covering func_8008DC50 and
 * func_8008DC7C) up to D_8008DCC0 into low kernel address 0x0000DF80..0x0000DFF0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * func_8008DC40 - base symbol whose +0x10 offset (func_8008DC50) starts the patch; D_8008DCC0 - exclusive end address.
 *
 * @par Control flow
 * Single do-while word-copy loop incrementing src from 0x8008DC50 and dst from 0xDF80 until src == D_8008DCC0.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 28 words (0x70 bytes) to kernel memory at 0xDF80..0xDFF0.
 *
 * @par Callers
 * Confirmed: func_8008D6E0 (0x8008D71C, during BIOS memory-card initialization) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Copies 32-bit words from func_8008DC40 + 4 (0x8008DC50) up to D_8008DCC0
 * into fixed kernel RAM address 0x0000DF80. Performs no function calls and
 * accesses no D_800xxxxx data globals beyond the code-range boundary symbols.
 * Called by func_8008D6E0.
 */
void gCopyKernelPatch0xDF80(void)
{
    register u32 *dst __asm__("$2");
    register u32 *src __asm__("$10");
    register u32 *end __asm__("$9");

    dst = (u32 *)0xDF80;
    src = func_8008DC40 + 4;
    end = gCardPatchJumpStubStart;
    do {
        *dst++ = *src++;
    } while (src != end);
}
