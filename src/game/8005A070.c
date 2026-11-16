// Categories: Gameplay > Player > Gauge | Graphics > HUD > Gauge
#include "game/types.h"

extern u8 D_8009AFA0[];

extern void func_8001E318(u8 *destination, u8 *source, s32 size);

/**
 * @brief Copies the default 20-byte gauge template into object + 0x54.
 *
 * D_8009AFA0 is the pristine gauge record - four words (value, limit, two reserved) plus the state byte and padding, 0x14 bytes in all, exactly the layout func_8005A190 updates. Player setup func_800573A4 calls this after wiping the rest of the state block.
 *
 * @param object Entity whose gauge array begins at +0x54.
 * @return Nothing.
 *
 * @par Globals
 * D_8009AFA0 - the read-only default gauge template, never written here.
 *
 * @par Control flow
 * One fixed-size copy of 0x14 bytes; no loop at source level.
 *
 * @par Calls
 * func_8001E318 - block copy of 0x14 bytes.
 *
 * @par Side effects
 * Overwrites 0x14 bytes at object + 0x54, including the gauge state byte, so any earlier gauge state is lost.
 *
 * @par Callers
 * Confirmed: func_800573A4 (0x80057598), the player/entity setup. The sibling func_8005A09C performs the same copy to an arbitrary address.
 */
void func_8005A070(u8 *object)
{
    func_8001E318(object + 0x54, D_8009AFA0, 0x14);
}
