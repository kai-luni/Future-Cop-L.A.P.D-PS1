// Categories: Game Engine > Resource Cleanup
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
    u8 unk0C[4];
    s32 unk10;
    u8 unk14[4];
    u8 flag18;
};

extern struct Unk4749CHandle gAssetHandle __asm__("D_800A0A80");

extern void gReleaseSoundVoicesByOwner(s32 owner) __asm__("func_80022240");
extern void gMarkPortraitVoiceStopped(void) __asm__("func_80029720");
extern void func_80045370(void);
extern s32 func_800474F0(struct Unk4749CHandle *handle);

void gStopTrackedSoundVoice(void) __asm__("func_80023E84");

/**
 * @brief Releases the tracked sound voice and clears its active state and deadline.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * For state 1, releases software voices by signed owner byte +0x0D when the resource is
 * present, clears the state and conditionally calls func_80045370. Always releases the tracked
 * handle, clears +0x10 and calls gMarkPortraitVoiceStopped.
 *
 * @par Globals
 * Reads and modifies gAssetHandle, including gTrackedSoundState at +0x08.
 *
 * @par Callers
 * Direct calls from gUpdateTrackedSoundVoice (func_80023D78), gStartTrackedSoundVoice
 * (func_80023F08), gAcquireTrackedSoundVoice (func_80024064), func_80029DD8,
 * gSoundHaltAllVoices (func_80047554).
 *
 * @par Role and path
 * Called by audio shutdown, timeout, failed playback and portrait reset paths.
 *
 * @par Unknown
 * The flag-dependent transport cleanup resides in func_80045370.
 */
void gStopTrackedSoundVoice(void)
{
    if (gAssetHandle.state == 1) {
        if (gAssetHandle.resource != 0) {
            gReleaseSoundVoicesByOwner(gAssetHandle.resource->owner);
        }
        gAssetHandle.state = 0;
        if (gAssetHandle.flag18 == 0) {
            func_80045370();
        }
    }
    func_800474F0(&gAssetHandle);
    gAssetHandle.unk10 = 0;
    gMarkPortraitVoiceStopped();
}
