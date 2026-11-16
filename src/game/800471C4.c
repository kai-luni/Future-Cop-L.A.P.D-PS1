// Categories: Platform > Sound | Gameplay > Entity Update
#include "game/types.h"

struct Unk471C4Info {
    u8 unk00[0xC1];
    s8 stream;
};

struct Unk471C4Object {
    u8 unk00[0xAC];
    struct Unk471C4Info *info;
};

struct Unk471C4Stream {
    s32 playing;
    s32 track;
    s32 handle;
};

extern struct Unk471C4Stream D_800A1810[];

extern void func_800243F8(s32 handle);
extern void func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle,
                          struct Unk471C4Object *object);

/**
 * @brief Switches or stops the looping sound track bound to an entity's stream slot.
 *
 * Selects D_800A1810[object->info->stream]; returns if track is unchanged, stops any
 * active handle via func_800243F8, records track, and starts playback unless track == 0xFF.
 *
 * @param track Sound track ID to play, or 0xFF to stop without starting a new track.
 * @param object Entity carrying info pointer at +0xAC (stream index at info +0xC1).
 *
 * @par Globals
 * D_800A1810 - per-stream state array holding {playing, track, handle}.
 * @par Control flow
 * Early returns if track == stream->track; if playing != 0 stops handle; updates stream->track; early returns if track == 0xFF; otherwise starts sound.
 * @par Calls
 * func_800243F8 - stops the active voice handle; func_80046E88 - plays track using stream->handle and object.
 * @par Side effects
 * Updates stream->playing and stream->track in D_800A1810 and triggers audio start/stop.
 * @par Callers
 * Confirmed: func_80053A74 (starts movement sound), func_80055988 (stops sound with track 0xFF).
 */
void func_800471C4(s32 track, struct Unk471C4Object *object)
{
    struct Unk471C4Stream *stream;

    stream = &D_800A1810[object->info->stream];
    if (track == stream->track) {
        return;
    }
    if (stream->playing != 0) {
        func_800243F8(stream->handle);
        stream->playing = 0;
    }
    stream->track = track;
    if (track == 0xFF) {
        return;
    }
    func_80046E88(track, 0, 0, -1, stream->handle, object);
    stream->playing = 1;
}
