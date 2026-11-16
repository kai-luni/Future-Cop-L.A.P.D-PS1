// Categories: Platform > GPU | Graphics > Display Setup
#include "game/gpu_globals.h"

#include "game/types.h"

extern volatile u32 *gGpuGp0PortPtr __asm__("D_8009DEC4");
extern u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");

/**
 * @brief Probes the PS1 GPU version via GP1(0x10) and configures legacy GP0 mode or 2MB VRAM mode.
 *
 * Confirmed: writes 0x10000007 to *D_8009DEC8 (GPUSTAT/GP1, 0x1F801814); if (*D_8009DEC4 & 0xFFFFFF) != 2, writes (*D_8009DEC8 & 0x3FFF) | 0xE1001000 to *D_8009DEC4 (GP0/GPUREAD, 0x1F801810), performs a dummy read of *D_8009DEC4, and returns 0; otherwise returns 1 if !(flags & 8) or writes 0x09000001 to *D_8009DEC8 and returns 2.
 * Category follows direct GPU hardware register access through D_8009DEC4/D_8009DEC8 (confirmed in func_8009570C/func_80094F4C) during graphics reset.
 * Likely: PSX libgpu ResetGraph GPU-revision probe detecting v2 GPU and optionally enabling GP1(0x09) 2MB VRAM mode when bit 3 of flags is set; Unknown: why bit 12 (0x1000) is forced in GP0(0xE1) on older GPUs.
 *
 * @param flags Mode flags from func_80094F4C; bit 3 (0x8) triggers GP1 command 0x09000001 on revision-2 GPUs.
 * @return 0 if GPU version != 2, 1 if version == 2 and !(flags & 8), or 2 if version == 2 and (flags & 8).
 *
 * @par Globals
 * D_8009DEC8 - pointer to GPUSTAT/GP1 (0x1F801814); D_8009DEC4 - pointer to GP0/GPUREAD (0x1F801810).
 * @par Control flow
 * Early return 0 when GPUREAD version != 2; second branch checks (flags & 8) to return 1 or issue 0x09000001 and return 2.
 * @par Calls
 * None.
 * @par Side effects
 * Issues GP1 commands 0x10000007 and optionally 0x09000001 to *D_8009DEC8, and GP0 write/read on *D_8009DEC4 when GPUREAD != 2.
 * @par Callers
 * Confirmed: func_80094F4C (0x80095084 in asm/us/main.s, when (a0 & 7) == 0) / Likely: none / Unknown: none.
 */
s32 func_80095350(u32 flags)
{
    *gGpuGp1PortPtr = 0x10000007;

    if ((*gGpuGp0PortPtr & 0xFFFFFF) != 2) {
        *gGpuGp0PortPtr = (*gGpuGp1PortPtr & 0x3FFF) | 0xE1001000;
        (void)*gGpuGp0PortPtr;
        return 0;
    }

    if ((flags & 8) == 0) {
        return 1;
    }

    *gGpuGp1PortPtr = 0x09000001;
    return 2;
}
