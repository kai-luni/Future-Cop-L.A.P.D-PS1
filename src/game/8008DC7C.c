// Categories: Platform > Hardware
#include "game/types.h"

/**
 *  @brief Waits for a hardware status bit to clear and jumps through a fixed address when enabled.
 *
 *  Uses the value held in register $3 as a base for two status reads.
 *  The indirect jump target is loaded from address 0xDFFC.
 *  @par Globals
 *  None named; reads the word at 0xDFFC and words relative to register $3.
 *  @par Control flow
 *  If bit 0x80 at base + 0x1074 is set, polls bit 0x80 at base + 0x1044
 *  until it clears, then jumps to the loaded target; otherwise returns.
 *  @par Calls
 *  No direct calls; control transfers to the indirect target when enabled.
 *  @par Side effects
 *  Performs volatile polling and may transfer control without returning.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Tests bit 0x80 at register $3 + 0x1074 and, if set, busy-waits until bit
 * 0x80 at register $3 + 0x1044 clears before jumping to the pointer at
 * 0xDFFC. Copied into kernel RAM at 0xDF80 by func_8008DDEC; not called
 * directly from C.
 */
void func_8008DC7C(void)
{
    register u32 p __asm__("$3");

    if (*(u32 *)(p + 0x1074) & 0x80) {
        do { } while (*(volatile u32 *)(p + 0x1044) & 0x80);
        goto *(void *)*(u32 *)0xDFFC;
    }
}
