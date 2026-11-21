// Categories: Platform > Sound > Voices | Game Engine > Options Menu
#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern s32 gSlotVoiceHandle[] __asm__("D_8009F678");
extern s32 gSlotTrackId[] __asm__("D_8009FA78");
extern void *gPlayerEntities[] __asm__("D_8009FA58");

extern void gStopSoundVoiceHandle(s32 handle) __asm__("func_80046A34");
extern s32 gAllocVoiceForSoundId(s32 arg0, s32 arg1, s32 arg2, s32 id) __asm__("func_80046A58");
extern void gPlaySoundIdOnVoice(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object) __asm__("func_80046E88");
extern void func_80047800(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);

void gUpdateOptionsPreviewVoice(s32 track, s32 volume, s32 slot) __asm__("func_80025E18");

/**
 * @brief Starts, stops or adjusts a player slot's options-menu sound preview.
 *
 * @param track Sound/track ID to preview.
 * @param volume Target volume (0 stops any active handle; > 0 plays or updates).
 * @param slot Player/panel slot index into D_8009F678, D_8009FA78, and D_8009FA58.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Checks the existing voice and stored track. A changed track or zero volume releases and
 * clears them. Positive volume either updates the existing voice using its pan byte or
 * allocates and starts a new voice, recording the track only on success.
 *
 * @par Globals
 * Reads and updates gSlotVoiceHandle and gSlotTrackId and reads gPlayerEntities.
 *
 * @par Callers
 * Direct calls from gDrawOptionsSubPanel (func_800260B8).
 *
 * @par Role and path
 * The options submenu previews a sound while the player adjusts a volume setting.
 *
 * @par Unknown
 * A negative volume skips playback and is distinct from an explicit zero-volume stop.
 */
void gUpdateOptionsPreviewVoice(s32 track, s32 volume, s32 slot)
{
    struct SoundVoice *voice;
    s32 handle;
    u32 offset;

    if (gSlotVoiceHandle[slot] != 0) {
        if (track != gSlotTrackId[slot] || volume == 0) {
            gStopSoundVoiceHandle(gSlotVoiceHandle[slot]);
            gSlotVoiceHandle[slot] = 0;
            gSlotTrackId[slot] = 0;
        }
    }
    offset = (u32)slot << 2;
    if (volume > 0) {
        s32 *handle_slot;
        handle_slot = (s32 *)((u8 *)gSlotVoiceHandle + offset);
        if (*handle_slot != 0) {
            voice = (struct SoundVoice *)*handle_slot;
            func_80047800(voice, voice->pan, volume, -1);
        } else {
            handle = gAllocVoiceForSoundId(volume, 0xA, 0, track);
            *handle_slot = handle;
            if (handle != 0) {
                gPlaySoundIdOnVoice(track, 0, 0, -1, handle, *(void **)((u8 *)gPlayerEntities + offset));
                *(s32 *)((u8 *)gSlotTrackId + offset) = track;
            } else {
                *(s32 *)((u8 *)gSlotTrackId + offset) = 0;
            }
        }
    }
}
