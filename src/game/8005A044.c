// Categories: Game Engine > Init | Gameplay > Player
#include "../../include/game/types.h"

extern u8 gPlayerInvulnerable[] __asm__("D_8009FA8C");

/**
 * @brief Clears the 2-byte per-player gate/status array D_8009FA8C at mission boot.
 *
 * Loops backwards from index 1 down to 0, setting D_8009FA8C[1] and D_8009FA8C[0] to 0 before gameplay messages and cheat flags read or set them.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA8C - 2-entry per-player byte flag array read/written in func_80053188, func_800579D4, func_80058210, func_80060A0C, func_8006BA50, and func_8006C080.
 *
 * @par Control flow
 * Two-iteration do-while countdown loop clearing D_8009FA8C[1] then D_8009FA8C[0].
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 0 to D_8009FA8C[0..1].
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024BF8, mission boot sequence) / Likely: none / Unknown: none.
 */
void func_8005A044(void)
{
    u8 *cursor;
    s32 remaining;

    remaining = 1;
    cursor = &gPlayerInvulnerable[1];
    do {
        *cursor-- = 0;
    } while (--remaining >= 0);
}
