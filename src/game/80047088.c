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

extern struct Unk47088Stream D_800A1810[];

extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_800243F8(s8 *arg0);
extern void func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle,
                          struct Unk47088Object *object);

void func_80047088(s32 arg0, struct Unk47088Object *object)
{
    struct Unk47088Stream *stream;
    s32 track;
    s32 *voice;

    if (arg0 & 0x20) {
        stream = &D_800A1810[object->info->stream];
        track = arg0 & 7;

        if (stream->playing != 0) {
            if (track == stream->track) {
                voice = (s32 *)func_80046C84(track | 0x38, (s32)object);
                if (voice != 0) {
                    voice[2] -= 0x64;
                }
            } else {
                func_80046C84(stream->track + 0x32, (s32)object);
                func_800243F8((s8 *)stream->handle);
                stream->playing = 0;
            }
        }

        if (track != 7 && stream->playing == 0) {
            stream->track = track;
            if (track != 6) {
                func_80046E88(track + 0x22, 0, 0, -1, stream->handle, object);
                stream->playing = 1;
            } else {
                voice = (s32 *)func_80046C84(0x3D, (s32)object);
                if (voice != 0) {
                    voice[2] -= 0x64;
                }
            }
        }
    }
}
