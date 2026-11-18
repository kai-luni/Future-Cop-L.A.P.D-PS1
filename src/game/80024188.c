// Categories: Platform > Sound | Resources > Asset Loader
#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

extern struct Unk4749CHandle gAssetHandle __asm__("D_800A0A80");
extern s32 func_800474C4(struct Unk4749CHandle *handle);

/**
 * @brief Validates the global tracked sound handle D_800A0A80 and returns its resource pointer.
 *
 * Calls func_800474C4(&D_800A0A80) to check that the handle's resource pointer is non-null
 * and its serial matches; returns D_800A0A80.resource if valid (result == 1), otherwise 0.
 *
 * @param void No parameters.
 * @return Pointer to the active Unk4749CResource cast to s32 if valid, or 0 if stale/null.
 *
 * @par Globals
 * D_800A0A80 - tracked sound handle struct (resource pointer and serial checked and returned).
 *
 * @par Control flow
 * Single conditional branch checking (u8)func_800474C4(&D_800A0A80) == 1.
 *
 * @par Calls
 * func_800474C4 - checks D_800A0A80.resource != 0 and resource->serial == D_800A0A80.serial.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: gSoundPauseVoices (0x80047704, resolves the preserved background stream voice node during pause) / Likely: none / Unknown: none.
 */
s32 func_80024188(void)
{
    register s32 value __asm__("$4");

    if ((u8)func_800474C4(&gAssetHandle) == 1) {
        value = (s32)gAssetHandle.resource;
    } else {
        value = 0;
    }
    return value;
}
