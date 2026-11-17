// Categories: Game Engine > Init
#include "game/types.h"

extern u32 gInitStateWord __asm__("D_8009FA18");
extern u32 gGpStateWord __asm__("D_8009F6F0");

/**
 * @brief Clears mission-boot state words D_8009FA18 and D_8009F6F0 to zero.
 *
 * Resets both 32-bit globals to 0 during mission initialization in func_80024A24; neither global has other confirmed reads or writes in the binary.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA18 - 32-bit state word cleared to 0.
 * D_8009F6F0 - 32-bit gp-relative state word (0x4DC($gp)) cleared to 0.
 *
 * @par Control flow
 * Straight-line stores of zero to D_8009FA18 and D_8009F6F0, then return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 0 to D_8009FA18 and D_8009F6F0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C2C, mission boot sequence) / Likely: none / Unknown: none.
 */
void func_8002A598(void)
{
    gInitStateWord = 0;
    gGpStateWord = 0;
}
