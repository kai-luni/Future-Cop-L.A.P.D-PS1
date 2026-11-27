// Categories: Game Engine > Init | Graphics > Textures
#include "game/types.h"

extern void **gTextureDescriptorTable __asm__("D_8009F7D8");
void gInitTextureDescriptorTable(void) __asm__("func_8005A2B0");


/**
 * @brief Clears the texture descriptor array pointer D_8009F7D8 during mission boot.
 *
 * Sets D_8009F7D8 to 0 (sw $zero, 0x5C4($gp)) before asset loading allocates the array via func_8005A414.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F7D8 - pointer to the loaded texture/effect descriptor pointer array.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F7D8 = 0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CCC, mission boot sequence) / Likely: none / Unknown: none.
 */
void gInitTextureDescriptorTable(void)
{
    gTextureDescriptorTable = 0;
}
