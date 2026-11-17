// Categories: Game Engine > Init | Gameplay > Object Registry
#include "game/types.h"

extern s32 gDescriptorRegistryCount __asm__("D_8009F848");

/**
 * @brief Clears the 6-entry object/texture descriptor registry count D_8009F848 during mission boot.
 *
 * Zeroes D_8009F848 in the start-of-mission boot path func_80024A24, resetting the 0x54-stride table D_800A3340 before entries are registered by func_8006A918 and queried by func_8006AAEC. Category Game Engine > Init | Gameplay > Object Registry follows the boot caller and registry counter global.
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
 * Confirmed: func_80024A24 (0x80024B98 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8006A880(void)
{
    gDescriptorRegistryCount = 0;
}
