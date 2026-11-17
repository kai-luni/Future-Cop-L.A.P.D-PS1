// Categories: Graphics > Lighting | Resources > Memory Pools
#include "game/types.h"

extern s32 D_8009F794;
extern s32 D_8009F788;
extern void *gSpriteRegionTable __asm__("D_8009F778");
extern void *gLightGrid __asm__("D_8009F780");
extern s32 gLightNodePool __asm__("D_8009F784");
extern void *gSpriteDescriptorTable __asm__("D_8009F77C");

extern s32 func_800194DC(s32 count, s32 size, s32 tag);

/**
 * @brief Initializes the dynamic map-light subsystem globals and allocates its 0x180-slot FixedPool.
 *
 * Clears the five light-list/clock state globals and allocates a 384-entry pool of 0x44-byte light nodes via func_800194DC under heap tag 2. Category Graphics > Lighting | Resources > Memory Pools follows pool consumer func_80049C38 (map light spawner) and func_800194DC.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F794, D_8009F788, D_8009F778, D_8009F780, D_8009F77C - light/effect pointers and clock reset to 0.
 * D_8009F784 - receives the `FixedPool` handle for 0x180 nodes of 0x44 bytes.
 *
 * @par Control flow
 * Unconditional straight-line stores and one allocator call; no branches or loops.
 *
 * @par Calls
 * func_800194DC - allocates and links `0x180` blocks of `0x44` bytes in pool tag 2.
 *
 * @par Side effects
 * Allocates heap memory under tag 2 and resets all six D_8009F778..D_8009F794 globals.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024CC4 in asm/us/main.s, mission boot init) / Likely: none / Unknown: none.
 */
void func_80049690(void)
{
    D_8009F794 = 0;
    D_8009F788 = 0;
    gSpriteRegionTable = 0;
    gLightGrid = 0;
    gLightNodePool = func_800194DC(0x180, 0x44, 2);
    gSpriteDescriptorTable = 0;
}
