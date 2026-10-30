#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern s32 D_8009F678[];
extern s32 D_8009FA78[];
extern void *D_8009FA58[];

extern void func_80046A34(s32 handle);
extern s32 func_80046A58(s32 arg0, s32 arg1, s32 arg2, s32 id);
extern void func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object);
extern void func_80047800(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);

void func_80025E18(s32 track, s32 volume, s32 slot)
{
    struct SoundVoice *voice;
    s32 handle;
    u32 offset;

    if (D_8009F678[slot] != 0) {
        if (track != D_8009FA78[slot] || volume == 0) {
            func_80046A34(D_8009F678[slot]);
            D_8009F678[slot] = 0;
            D_8009FA78[slot] = 0;
        }
    }
    offset = (u32)slot << 2;
    if (volume > 0) {
        s32 *handle_slot;
        handle_slot = (s32 *)((u8 *)D_8009F678 + offset);
        if (*handle_slot != 0) {
            voice = (struct SoundVoice *)*handle_slot;
            func_80047800(voice, voice->pan, volume, -1);
        } else {
            handle = func_80046A58(volume, 0xA, 0, track);
            *handle_slot = handle;
            if (handle != 0) {
                func_80046E88(track, 0, 0, -1, handle, *(void **)((u8 *)D_8009FA58 + offset));
                *(s32 *)((u8 *)D_8009FA78 + offset) = track;
            } else {
                *(s32 *)((u8 *)D_8009FA78 + offset) = 0;
            }
        }
    }
}
