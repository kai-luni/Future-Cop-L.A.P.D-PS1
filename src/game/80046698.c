#include "game/types.h"

struct SoundChannel {
    s32 id;
    s32 priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C;
    s32 unk10;
    u8 unk14[0x14];
    s32 unk28;
};

struct SoundVoice {
    s32 bank;
    u8 unk04[8];
    s8 priority;
    u8 unk0D[3];
    s32 unk10;
    u8 unk14[4];
    struct SoundChannel *owner;
    struct SoundVoice *next;
    struct SoundVoice *prev;
    struct SoundVoice *bankNext;
    struct SoundVoice *bankPrev;
};

extern struct SoundVoice *D_8009F73C;
extern s32 D_8009F740;
extern struct SoundVoice *D_8009F744;
extern s32 D_8009F748;
extern struct SoundVoice *D_8009F74C;
extern s32 D_8009F750;
extern struct SoundVoice *D_8009F754;
extern s32 D_8009F758;

extern void func_800243F8(s8 *arg0);
extern void func_80046804(struct SoundVoice *voice);

void func_80046698(struct SoundVoice *voice)
{
    struct SoundVoice *old;

    if (voice->unk10 == 0) {
        return;
    }

    voice->unk10 = 0;
    func_800243F8((s8 *)voice);

    if (voice->prev != 0) {
        voice->prev->next = voice->next;
    } else {
        switch (voice->priority) {
        case 0:
            D_8009F74C = voice->next;
            break;
        case 2:
            D_8009F754 = voice->next;
            break;
        default:
            D_8009F744 = voice->next;
            break;
        }
    }

    if (voice->next != 0) {
        voice->next->prev = voice->prev;
    }

    old = D_8009F73C;
    D_8009F73C = voice;
    voice->next = old;

    switch (voice->priority) {
    case 0:
        D_8009F750--;
        break;
    case 2:
        D_8009F758--;
        break;
    default:
        D_8009F748--;
        break;
    }

    D_8009F740++;

    if (voice->bank != 0xFF) {
        func_80046804(voice);
    }

    if (voice->owner != 0) {
        voice->owner->unk28 = 0;
    }
}
