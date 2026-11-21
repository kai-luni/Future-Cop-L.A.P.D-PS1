// Categories: Resources > Handles
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
    s32 unk14;
    u8 flag18;
};

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern struct Unk4749CHandle gAssetHandle __asm__("D_800A0A80");

extern int gIsTrackedSoundPlaying(void) __asm__("func_80024170");
extern void gStopTrackedSoundVoice(void) __asm__("func_80023E84");
extern s32 func_80045028(s32 mode);
extern struct Unk4749CResource *gAllocPrioritySoundVoice(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80046B04");
extern void gBindSoundVoiceHandle(struct Unk4749CHandle *handle, struct Unk4749CResource *resource) __asm__("func_8004749C");

s32 gAcquireTrackedSoundVoice(s32 arg0, s32 arg1) __asm__("func_80024064");

/**
 * @brief Acquires and initializes the voice handle used for the tracked sound.
 *
 * @param arg0 Mode passed to the validation function.
 * @param arg1 Value scaled by 21 for the handle.
 * @return Nonzero if a resource was acquired, otherwise zero.
 *
 * @par Confirmed mechanics
 * Requires CVK enable, stops existing playback when the tracked state is 1, clears the deadline
 * and allocates a voice. Binds the resource, records the mix selector on successful validation,
 * and stores signed delay divided by 21 with a fallback of 0x834 for quotients at least 0xBB9.
 *
 * @par Globals
 * Reads gCvkSystemActive and updates gAssetHandle.
 *
 * @par Callers
 * Direct calls from gStepSynchronizedPortrait (func_80029748).
 *
 * @par Role and path
 * The portrait player acquires this voice before trying to start it.
 *
 * @par Unknown
 * The mix-selector validation is defined by func_80045028.
 */
s32 gAcquireTrackedSoundVoice(s32 arg0, s32 arg1)
{
    struct Unk4749CResource *handle;

    handle = 0;
    if (gCvkSystemActive != 0) {
        if ((u8)gIsTrackedSoundPlaying()) {
            gStopTrackedSoundVoice();
        }
        gAssetHandle.unk10 = 0;
        handle = gAllocPrioritySoundVoice(0x7F, 0xA, 0, 0xFF);
        if (handle != 0) {
            gBindSoundVoiceHandle(&gAssetHandle, handle);
            if ((u8)func_80045028(arg0)) {
                gAssetHandle.flag18 = 1;
                gAssetHandle.unk0C = arg0;
            } else {
                gAssetHandle.flag18 = 0;
                gAssetHandle.unk0C = -1;
            }
        }
        if (handle != 0) {
            gAssetHandle.unk14 = arg1 / 21;
            if (gAssetHandle.unk14 >= 0xBB9) {
                gAssetHandle.unk14 = 0x834;
            }
        }
    }
    return handle != 0;
}
