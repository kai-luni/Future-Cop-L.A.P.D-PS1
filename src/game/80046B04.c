#include "game/types.h"

struct SoundVoice {
    u32 slot;
    u8 volume;
    s8 unk5;
    u8 unk6;
    u8 unk7;
    s32 time;
    s8 pan;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    s32 seq;
    s32 unk14;
    struct SoundVoice *next;
    struct SoundVoice *freeNext;
};

struct SoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    struct SoundVoice *head;
};

extern struct SoundBank D_800A7938[];
extern struct SoundVoice *D_8009F744;
extern s32 D_8009F33C;

extern struct SoundVoice *func_8004665C(void);
extern void func_80046698(struct SoundVoice *voice);
extern void func_80046870(struct SoundVoice *voice);

s32 func_80046B04(s32 arg0, s32 arg1, s32 arg2, s32 slot)
{
    struct SoundBank *bank;
    struct SoundVoice *voice;
    struct SoundVoice *node;
    s32 value;

    voice = 0;
    value = arg0 + (arg1 << 4);
    bank = &D_800A7938[slot];

    if (slot == 0xFF || bank->voices < bank->unk14) {
        voice = func_8004665C();
        if (voice == 0) {
            node = D_8009F744;
            if (node != 0 && node->time < value) {
                func_80046698(node);
                voice = func_8004665C();
            }
        }
    } else {
        node = bank->head;
        if (node != 0 && node->time < value &&
            (node->pan == 1 || node->next != 0)) {
            func_80046698(node);
            voice = func_8004665C();
        }
    }

    if (voice != 0) {
        voice->slot = slot;
        voice->volume = arg0;
        voice->pan = arg2;
        voice->next = 0;
        voice->time = value;
        if (arg2 == 1) {
            voice->time = value + 0x64;
        }
        voice->seq = D_8009F33C;
        voice->unk5 = -1;
        D_8009F33C++;
        if (D_8009F33C == 0) {
            D_8009F33C = 1;
        }
        voice->unk14 = 0;
        func_80046870(voice);
    }
    return voice;
}
