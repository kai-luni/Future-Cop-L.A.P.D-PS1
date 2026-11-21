// Categories: Game Engine > Game State > Reset | Gameplay > Object Registry
#include "game/types.h"

extern s32 gDescriptorRegistryCount __asm__("D_8009F848");
void gResetDescriptorRegistryCount(void) __asm__("func_8006A88C");


/**
 * @brief Clears the 6-entry object/texture descriptor registry count D_8009F848 during mission reset.
 *
 * Zeroes D_8009F848 in the between-missions reset path func_80024D14, mirroring the boot reset func_8006A880 before D_800A3340 entries are repopulated. Category Game Engine > Game State > Reset | Gameplay > Object Registry follows the reset caller and registry counter global.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F848 - active entry count for the 6-slot D_800A3340 descriptor table, cleared to 0.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F848 = 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024DD0 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void gResetDescriptorRegistryCount(void)
{
    gDescriptorRegistryCount = 0;
}
