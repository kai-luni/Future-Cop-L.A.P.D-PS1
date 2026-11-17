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
extern void *D_8009FA58[];

extern void func_80046A34(s32 handle);
extern s32 func_80046A58(s32 arg0, s32 arg1, s32 arg2, s32 id);
extern void func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object);
extern void func_80047800(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);

/**
 * @brief Starts, stops, or adjusts a per-slot preview sound voice in the options menu.
 *
 * Stops the existing voice handle in D_8009F678[slot] if the requested track differs
 * from D_8009FA78[slot] or volume == 0; when volume > 0, updates the live voice volume
 * via func_80047800 or allocates and starts a new voice for D_8009FA58[slot].
 *
 * @param track Sound/track ID to preview.
 * @param volume Target volume (0 stops any active handle; > 0 plays or updates).
 * @param slot Player/panel slot index into D_8009F678, D_8009FA78, and D_8009FA58.
 *
 * @par Globals
 * D_8009F678 - per-slot active voice handles; D_8009FA78 - per-slot track IDs; D_8009FA58 - per-slot player objects.
 *
 * @par Control flow
 * Releases stale/muted handle if D_8009F678[slot] != 0; then if volume > 0 either updates existing voice or allocates and starts track.
 *
 * @par Calls
 * func_80046A34 - stops/releases handle; func_80047800 - updates volume; func_80046A58 - allocates voice; func_80046E88 - triggers playback.
 *
 * @par Side effects
 * Updates D_8009F678[slot], D_8009FA78[slot], and audio voice allocations.
 *
 * @par Callers
 * Confirmed: func_800260B8 (0x80026524, options sub-panel volume slider adjustments).
 */
void func_80025E18(s32 track, s32 volume, s32 slot)
{
    struct SoundVoice *voice;
    s32 handle;
    u32 offset;

    if (gSlotVoiceHandle[slot] != 0) {
        if (track != gSlotTrackId[slot] || volume == 0) {
            func_80046A34(gSlotVoiceHandle[slot]);
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
            handle = func_80046A58(volume, 0xA, 0, track);
            *handle_slot = handle;
            if (handle != 0) {
                func_80046E88(track, 0, 0, -1, handle, *(void **)((u8 *)D_8009FA58 + offset));
                *(s32 *)((u8 *)gSlotTrackId + offset) = track;
            } else {
                *(s32 *)((u8 *)gSlotTrackId + offset) = 0;
            }
        }
    }
}
