// Categories: Graphics > GPU | Platform > GPU
#include "game/types.h"

extern u8 gDispEnvCache[] __asm__("D_8009DE5C");
extern void func_8008CD10(u8 *arg0, u8 *arg1, u32 arg2);

/**
 * @brief Copies the cached 0x14-byte DISPENV structure into a caller buffer (PsyQ GetDispEnv).
 *
 * Reads the GPU library's cached 0x14-byte display environment block at D_8009DE5C (populated by
 * func_800934BC) into arg0 via func_8008CD10.
 *
 * @param arg0 Destination buffer receiving the 0x14-byte DISPENV copy.
 * @return The destination pointer arg0.
 *
 * @par Globals
 * D_8009DE5C - cached 0x14-byte DISPENV block read as copy source.
 *
 * @par Control flow
 * Straight-line call to func_8008CD10 followed by returning arg0.
 *
 * @par Calls
 * func_8008CD10 - BIOS memcpy (A0:2A) copying 0x14 bytes from D_8009DE5C to arg0.
 *
 * @par Side effects
 * Writes 0x14 bytes to the caller buffer at arg0.
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s / Likely: none / Unknown: whether unreferenced PsyQ libgpu unit or called indirectly.
 */
u8 *func_800938F0(u8 *arg0)
{
    func_8008CD10(arg0, gDispEnvCache, 0x14);
    return arg0;
}
