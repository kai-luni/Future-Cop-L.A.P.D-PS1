// Categories: Game Engine > Sound Playback
#include "game/types.h"

struct Resource { u8 pad[13]; s8 owner; };
struct Handle { struct Resource *resource; s32 serial, state, id, time, delay; u8 flag; };
struct Params { s32 pitch, volume, pan; };
extern struct Handle gAssetHandle __asm__("D_800A0A80");
extern s32 gSpuBufferPtr __asm__("D_8009F278");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern void func_80023E84(void);
extern s32 func_800474C4(struct Handle *);
extern s32 func_800450A4(s32, s8 *);
extern s8 func_80045050(void);
extern s32 gAllocateSoundChannelVoice(s32, s32, struct Params *, s32) __asm__("func_800220C4");
extern s32 func_800474F0(struct Handle *);
extern void func_80047800(struct Resource *, s32, s32, s32);
extern void gQueueSoundChannelStarts(s32, s32 *) __asm__("func_800221E4");

/**
 *  @brief Attempts to start the sound represented by the global handle.
 *
 *  The handle supplies a resource, optional mix lookup and delayed completion time.
 *  A successful start records the active state and schedules its time relative to the global clock.
 *  @return Nonzero if the handle state is 1 after processing, otherwise zero.
 *  @par Globals
 *  D_800A0A80 holds the handle; D_8009F278 supplies a playback argument and D_8009FA2C supplies the clock.
 *  @par Control flow
 *  Initialization and handle readiness gate playback; mix lookup failure resets setup and start failure invokes handle cleanup.
 *  @par Calls
 *  func_80023E84 initializes or resets setup; func_800474C4 checks the handle; func_800450A4 or func_80045050 supplies mix data.
 *  gAllocateSoundChannelVoice starts playback; func_800474F0 handles failure; func_80047800 and gQueueSoundChannelStarts perform success-side updates.
 *  @par Side effects
 *  May change handle state and time and invoke sound setup, playback and channel operations.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80023F08(void)
{
    s8 mix[24];
    struct Params params;
    s32 channel;
    struct Resource *resource;

    if (!gSpuBufferPtr) func_80023E84();
    if ((u8)func_800474C4(&gAssetHandle) == 1) {
        resource = gAssetHandle.resource;
        if (gAssetHandle.flag) {
            if (!(u8)func_800450A4(gAssetHandle.id, mix)) {
                resource = 0;
                func_80023E84();
            }
        } else {
            mix[0] = func_80045050();
            mix[1] = 0x40;
        }
        if (resource) {
            params.pitch = 0x400;
            params.volume = mix[0];
            params.pan = mix[1];
            if (gAllocateSoundChannelVoice(100, resource->owner, &params, gSpuBufferPtr) == -1) {
                func_800474F0(&gAssetHandle);
            } else {
                if (gAssetHandle.flag) func_80047800(resource, mix[1], mix[0], -1);
                channel = resource->owner;
                gQueueSoundChannelStarts(1, &channel);
                gAssetHandle.state = 1;
                gAssetHandle.time = gFrameTickCounter + gAssetHandle.delay;
            }
        }
    }
    return gAssetHandle.state == 1;
}
