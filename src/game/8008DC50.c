// Categories: Platform > Register Operations
#include "game/types.h"
void gPatchKernelMaskHalfword(void) __asm__("func_8008DC50");


/**
 *  @brief Sets bits in a halfword addressed through a register-bound value.
 *
 *  Reads the halfword at p plus 0xA and writes it back with mask and 0x12 ORed in.
 *  The source binds p and mask to registers without initializing them locally.
 *  @par Globals
 *  None named in the source.
 *  @par Control flow
 *  Performs the read-modify-write, then counts down from 0x28 to zero.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes a halfword at the address derived from p and executes a delay loop.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Reads a halfword at register $3 + 0xA, ORs in register $2 and 0x12, writes
 * the result back, and spins for 0x28 iterations. Copied into kernel RAM at
 * 0xDF80 alongside func_8008DC7C by func_8008DDEC; not called directly from
 * C.
 */
void gPatchKernelMaskHalfword(void)
{
    register u32 p __asm__("$3");
    register u32 mask __asm__("$2");
    register u32 h __asm__("$15");
    register u32 v __asm__("$24");
    register u32 r __asm__("$25");

    h = *(u16 *)(p + 0xA);
    v = h | mask;
    r = v | 0x12;
    *(u16 *)(p + 0xA) = r;
    {
        register int i __asm__("$8");
        i = 0x28;
        do { i--; } while (i);
    }
}
