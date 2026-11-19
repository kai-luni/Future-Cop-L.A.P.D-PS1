// Categories: Game Engine > Audio
#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0xD];
    s8 owner;
    u8 unk0E[2];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
    s32 state;
    s32 unk0C;
    s32 unk10;
    u8 unk14[4];
    u8 flag18;
};

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern struct Unk4749CHandle gAssetHandle __asm__("D_800A0A80");
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern u8 gCvkSystemActive __asm__("D_8009F26C");

extern void gStopTrackedSoundVoice(void) __asm__("func_80023E84");
extern s32 func_800450A4(s32 id, s8 *mix);
extern s32 func_800474C4(struct Unk4749CHandle *handle);
extern void func_80047800(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);

void gUpdateTrackedSoundVoice(void) __asm__("func_80023D78");

/**
 * @brief Checks the tracked sound deadline and updates its voice mix when eligible.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the deadline at handle +0x10 and clears and stops the sound if it is outside the
 * current tick through tick +0xBB8, or if an active handle has no deadline. Requires CVK
 * enable, active state and byte +0x18 before looking up mix values and validating the voice
 * handle.
 *
 * @par Globals
 * Reads gCvkSystemActive and gFrameTickCounter and accesses the deadline, state and mix
 * selector in gAssetHandle.
 *
 * @par Callers
 * Direct calls from func_800462D0.
 *
 * @par Role and path
 * The audio update path maintains the active tracked sound between start and release.
 *
 * @par Unknown
 * The mix lookup and handle validator determine why a particular voice becomes invalid.
 */
void gUpdateTrackedSoundVoice(void)
{
    s8 mix[0x18];

    if (gAssetHandle.unk10 != 0) {
        if (gAssetHandle.unk10 > gFrameTickCounter + 0xBB8 ||
            gAssetHandle.unk10 < gFrameTickCounter) {
            gAssetHandle.unk10 = 0;
            gStopTrackedSoundVoice();
        }
    } else if (gAssetHandle.state != 0) {
        gAssetHandle.unk10 = 0;
        gStopTrackedSoundVoice();
    }

    if (gCvkSystemActive != 0 &&
        gAssetHandle.state != 0 &&
        gAssetHandle.flag18 != 0) {
        if ((u8)func_800450A4(gAssetHandle.unk0C, mix) != 0 &&
            (u8)func_800474C4(&gAssetHandle) == 1) {
            func_80047800((struct SoundVoice *)gAssetHandle.resource,
                          mix[1], mix[0], -1);
        } else {
            gStopTrackedSoundVoice();
        }
    }
}
