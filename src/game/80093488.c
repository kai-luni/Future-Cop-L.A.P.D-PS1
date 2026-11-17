// Categories: Graphics > GPU | Platform > GPU
#include "game/types.h"

extern u8 gDrawEnvCache[] __asm__("D_8009DE00");
extern void func_8008CD10(u8 *arg0, u8 *arg1, u32 arg2);

/**
 * @brief Copies the cached 0x5C-byte DRAWENV structure into a caller buffer (PsyQ GetDrawEnv).
 *
 * Reads the GPU library's cached 0x5C-byte drawing environment block at D_8009DE00 (populated by
 * func_800932F0 and func_800933B0 at D_8009DDF2 + 0x0E) into arg0 via func_8008CD10.
 *
 * @param arg0 Destination buffer receiving the 0x5C-byte DRAWENV copy.
 * @return The destination pointer arg0.
 *
 * @par Globals
 * D_8009DE00 - cached 0x5C-byte DRAWENV block (D_8009DDF2 + 0x0E) read as copy source.
 *
 * @par Control flow
 * Straight-line call to func_8008CD10 followed by returning arg0.
 *
 * @par Calls
 * func_8008CD10 - BIOS memcpy (A0:2A) copying 0x5C bytes from D_8009DE00 to arg0.
 *
 * @par Side effects
 * Writes 0x5C bytes to the caller buffer at arg0.
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s / Likely: none / Unknown: whether unreferenced PsyQ libgpu unit or called indirectly.
 */
u8 *func_80093488(u8 *arg0)
{
    func_8008CD10(arg0, gDrawEnvCache, 0x5C);
    return arg0;
}
