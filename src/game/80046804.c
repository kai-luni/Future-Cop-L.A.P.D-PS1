// Categories: Platform > Sound
#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
    s32 priority;
    u8 unk0C[0x18];
    struct SoundVoice *bankNext;
    struct SoundVoice *bankPrev;
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

/**
 * @brief Unlinks a SoundVoice node from its SoundBank doubly-linked voice list in D_800A7938.
 *
 * Confirmed: resolves bank = &D_800A7938[voice->bank] (+0x00), bridges voice->bankPrev (+0x2C) or bank->head (+0x18) to voice->bankNext (+0x28), updates voice->bankNext->bankPrev when non-null, and decrements bank->voices (+0x12). Called when stopping a voice or re-bucketing by priority.
 *
 * @param voice SoundVoice node (+0x00 bank index, +0x28 bankNext, +0x2C bankPrev) to unlink.
 *
 * @par Globals
 * D_800A7938 - SoundBank table (+0x12 voice count decremented, +0x18 head updated when voice->bankPrev is null).
 *
 * @par Control flow
 * Straight-line doubly-linked list removal with two null checks (voice->bankPrev != 0 and voice->bankNext != 0).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Mutates neighbor bankNext/bankPrev links, D_800A7938[voice->bank].head (+0x18), and decrements .voices (+0x12).
 *
 * @par Callers
 * Confirmed: func_80046698 (0x800467D4 in asm/us/main.s), func_800462D0 (0x80046588 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80046804(struct SoundVoice *voice)
{
    struct SoundBank *bank;

    bank = &D_800A7938[voice->bank];
    if (voice->bankPrev != 0) {
        voice->bankPrev->bankNext = voice->bankNext;
    } else {
        bank->head = voice->bankNext;
    }
    if (voice->bankNext != 0) {
        voice->bankNext->bankPrev = voice->bankPrev;
    }
    bank->voices--;
}
