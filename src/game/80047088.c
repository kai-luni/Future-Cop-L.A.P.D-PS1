// Categories: Platform > Audio | Game Engine > Music
#include "game/types.h"

struct Unk47088Info {
    u8 unk00[0xC1];
    s8 stream;
};

struct Unk47088Object {
    u8 unk00[0xAC];
    struct Unk47088Info *info;
};

struct Unk47088Stream {
    s32 playing;
    s32 track;
    s32 handle;
};

extern struct Unk47088Stream gStreamTable[] __asm__("D_800A1810");

extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern void func_800243F8(s8 *arg0);
extern void gPlaySoundIdOnVoice(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle,
                          struct Unk47088Object *object) __asm__("func_80046E88");
void gApplyAnimationSoundEvent(s32 arg0, struct Unk47088Object *object) __asm__("func_80047088");


/**
 * @brief Switches or re-triggers the music track that belongs to an object.
 *
 * Only the request bit 0x20 is honoured. The stream block is selected by the
 * object's info record, and the three low bits of arg0 are the requested
 * track. If that stream is already playing, the same track only lowers the
 * volume of the running voice by 0x64 (through the voice pointer returned by
 * func_80046C84 with the track-relative id), while a different track stops
 * the old one with sound stream->track + 0x32, releases the handle and clears
 * the playing flag. A requested track other than 7 starts the new loop with
 * func_80046E88(track + 0x22, ...) and marks the stream playing; track 6 is
 * handled as a one-shot that only lowers its voice.
 *
 * @param arg0 Request flags, bit 0x20 plus the track number in bits 0..2.
 * @param object Object whose info record names the stream (0xC1).
 *
 * @par Globals
 * D_800A1810 - stream table (playing, track, handle).
 *
 * @par Control flow
 * Guard on the request bit, then stop/restart or volume-only paths.
 *
 * @par Calls
 * func_80046C84 - voice lookup by id. func_800243F8 - release the stream handle.
 * func_80046E88 - start the looping track.
 *
 * @par Side effects
 * Writes the stream state and lowers or stops voices.
 *
 * @par Callers
 * Confirmed: func_800267C8, func_800588D4.
 */
void gApplyAnimationSoundEvent(s32 arg0, struct Unk47088Object *object)
{
    struct Unk47088Stream *stream;
    s32 track;
    s32 *voice;

    if (arg0 & 0x20) {
        stream = &gStreamTable[object->info->stream];
        track = arg0 & 7;

        if (stream->playing != 0) {
            if (track == stream->track) {
                voice = (s32 *)gPlayNonpositionalSoundEffect(track | 0x38, (s32)object);
                if (voice != 0) {
                    voice[2] -= 0x64;
                }
            } else {
                gPlayNonpositionalSoundEffect(stream->track + 0x32, (s32)object);
                func_800243F8((s8 *)stream->handle);
                stream->playing = 0;
            }
        }

        if (track != 7 && stream->playing == 0) {
            stream->track = track;
            if (track != 6) {
                gPlaySoundIdOnVoice(track + 0x22, 0, 0, -1, stream->handle, object);
                stream->playing = 1;
            } else {
                voice = (s32 *)gPlayNonpositionalSoundEffect(0x3D, (s32)object);
                if (voice != 0) {
                    voice[2] -= 0x64;
                }
            }
        }
    }
}
