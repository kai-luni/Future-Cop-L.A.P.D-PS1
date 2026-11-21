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
extern s32 gValidateSoundVoiceHandle(struct Unk4749CHandle *handle) __asm__("func_800474C4");

s32 gGetTrackedSoundVoice(void) __asm__("func_80024188");

/**
 * @brief Returns the tracked voice resource when the handle validates.
 *
 * @param void No parameters.
 * @return Pointer to the active Unk4749CResource cast to s32 if valid, or 0 if stale/null.
 *
 * @par Confirmed mechanics
 * Passes the handle address in a0 to func_800474C4 and compares the low byte of the result to
 * 1. Returns the resource word at offset zero on success or zero otherwise.
 *
 * @par Globals
 * Reads gAssetHandle.
 *
 * @par Callers
 * Direct calls from gSoundPauseVoices (func_800476D0).
 *
 * @par Role and path
 * The pause path resolves the tracked voice so it can spare it when stopping other voices.
 *
 * @par Unknown
 * The validator owns the resource and serial checks.
 */
s32 gGetTrackedSoundVoice(void)
{
    register s32 value __asm__("$4");

    if ((u8)gValidateSoundVoiceHandle(&gAssetHandle) == 1) {
        value = (s32)gAssetHandle.resource;
    } else {
        value = 0;
    }
    return value;
}
