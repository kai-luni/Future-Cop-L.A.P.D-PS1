// Categories: Graphics > GPU Command Lists > Draw Mode | Platform > GPU
#include "../../include/game/types.h"
u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage) __asm__("func_80093F58");


/**
 * @brief Builds the E1h GPU draw-mode (texture page) command word.
 *
 * @param dfe Nonzero sets bit 0x400.
 * @param dtd Nonzero sets bit 0x200.
 * @param tpage Texture-page bits, masked with 0x9FF.
 * @return The command word 0xE1000000 | flags | tpage.
 *
 * @par Confirmed
 * Pure bit packing, no memory access. The mask 0x9FF keeps bits 0..8 and
 * 11 - the same layout func_80092230 packs from coordinates and modes.
 *
 * @par Unknown
 * The exact meaning of the dfe/dtd flag names is not proven here.
 *
 * @par Callers
 * func_80093A9C, func_80093AF0, func_80093CE8.
 *
 * @par Environment path
 * Encodes dfe, dtd, and tpage bits into a 32-bit GP0 0xE1 draw-mode (texture
 * page) command word and returns it (called by func_80093A9C, func_80093AF0,
 * func_80093CE8).
 */
u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage)
{
    return 0xE1000000
        | (dtd != 0 ? 0x200 : 0)
        | (dfe != 0 ? 0x400 : 0)
        | (tpage & 0x9FF);
}
