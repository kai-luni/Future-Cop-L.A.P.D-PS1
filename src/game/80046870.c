#include "game/types.h"

struct SoundVoice {
    s32 bank;
    u8 volume;
    s8 unk5;
    u8 unk6;
    u8 unk7;
    s32 priority;
    s8 pan;
    u8 unk0D;
    u8 unk0E;
    u8 unk0F;
    s32 seq;
    s32 unk14;
    u8 unk18[4];
    struct SoundVoice *next;
    struct SoundVoice *prev;
};

extern struct SoundVoice *D_8009F744;
extern s32 D_8009F748;
extern struct SoundVoice *D_8009F74C;
extern s32 D_8009F750;
extern struct SoundVoice *D_8009F754;
extern s32 D_8009F758;

extern void func_800469A0(struct SoundVoice *voice);

void func_80046870(struct SoundVoice *voice)
{
    struct SoundVoice *prev;
    struct SoundVoice *next;

    switch (voice->pan) {
    case 0:
        if (D_8009F74C != 0) {
            D_8009F74C->prev = voice;
        }
        voice->prev = 0;
        voice->next = D_8009F74C;
        D_8009F74C = voice;
        D_8009F750++;
        break;
    case 2:
        if (D_8009F754 != 0) {
            D_8009F754->prev = voice;
        }
        voice->prev = 0;
        voice->next = D_8009F754;
        D_8009F754 = voice;
        D_8009F758++;
        break;
    default:
        prev = 0;
        for (next = D_8009F744; next != 0; next = next->next) {
            if (voice->priority < next->priority) {
                break;
            }
            prev = next;
        }
        voice->prev = prev;
        voice->next = next;
        if (prev != 0) {
            prev->next = voice;
        } else {
            D_8009F744 = voice;
        }
        if (next != 0) {
            next->prev = voice;
        }
        D_8009F748++;
        break;
    }

    if (voice->bank != 0xFF) {
        func_800469A0(voice);
    }
}
