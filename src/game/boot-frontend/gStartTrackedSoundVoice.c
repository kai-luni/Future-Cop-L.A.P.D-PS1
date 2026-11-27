// Categories: Game Engine > Sound Playback
#include "game/types.h"

struct Resource { u8 pad[13]; s8 owner; };
struct Handle { struct Resource *resource; s32 serial, state, id, time, delay; u8 flag; };
struct Params { s32 pitch, volume, pan; };
extern struct Handle gAssetHandle __asm__("D_800A0A80");
extern s32 gSpuBufferPtr __asm__("D_8009F278");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern void gStopTrackedSoundVoice(void) __asm__("func_80023E84");
extern s32 gValidateSoundVoiceHandle(struct Handle *) __asm__("func_800474C4");
extern s32 gComputeTrackedSoundMix(s32, s8 *) __asm__("func_800450A4");
extern s8 gComputeTrackedVoiceVolume(void) __asm__("func_80045050");
extern s32 gAllocateSoundChannelVoice(s32, s32, struct Params *, s32) __asm__("func_800220C4");
extern s32 gReleaseValidatedSoundVoiceHandle(struct Handle *) __asm__("func_800474F0");
extern void gUpdateSoundVoiceMix(struct Resource *, s32, s32, s32) __asm__("func_80047800");
extern void gQueueSoundChannelStarts(s32, s32 *) __asm__("func_800221E4");

s32 gStartTrackedSoundVoice(void) __asm__("func_80023F08");

/**
 * @brief Attempts to start the tracked sound and reports whether its state is active.
 *
 * @param void No parameters.
 * @return Nonzero if the handle state is 1 after processing, otherwise zero.
 *
 * @par Confirmed mechanics
 * Checks the SPU base, validates the handle and chooses looked-up or default level and pan. It
 * allocates a software voice with pitch 0x400, queues its channel, writes state 1 and sets the
 * deadline to the current tick plus the stored delay. All exits compare the word at +0x08 to 1.
 *
 * @par Globals
 * Reads and updates gAssetHandle and reads gSpuBufferPtr and gFrameTickCounter.
 * gTrackedSoundState aliases gAssetHandle +0x08.
 *
 * @par Callers
 * Direct calls from gStepSynchronizedPortrait (func_80029748).
 *
 * @par Role and path
 * The synchronized portrait path uses this to launch its companion voice.
 *
 * @par Unknown
 * The return value indicates current state, including an already-active state.
 */
s32 gStartTrackedSoundVoice(void)
{
    s8 mix[24];
    struct Params params;
    s32 channel;
    struct Resource *resource;

    if (!gSpuBufferPtr) gStopTrackedSoundVoice();
    if ((u8)gValidateSoundVoiceHandle(&gAssetHandle) == 1) {
        resource = gAssetHandle.resource;
        if (gAssetHandle.flag) {
            if (!(u8)gComputeTrackedSoundMix(gAssetHandle.id, mix)) {
                resource = 0;
                gStopTrackedSoundVoice();
            }
        } else {
            mix[0] = gComputeTrackedVoiceVolume();
            mix[1] = 0x40;
        }
        if (resource) {
            params.pitch = 0x400;
            params.volume = mix[0];
            params.pan = mix[1];
            if (gAllocateSoundChannelVoice(100, resource->owner, &params, gSpuBufferPtr) == -1) {
                gReleaseValidatedSoundVoiceHandle(&gAssetHandle);
            } else {
                if (gAssetHandle.flag) gUpdateSoundVoiceMix(resource, mix[1], mix[0], -1);
                channel = resource->owner;
                gQueueSoundChannelStarts(1, &channel);
                gAssetHandle.state = 1;
                gAssetHandle.time = gFrameTickCounter + gAssetHandle.delay;
            }
        }
    }
    return gAssetHandle.state == 1;
}
