// Categories: Game Engine > Audio
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

extern struct SoundVoice *gVoicePriorityList __asm__("D_8009F744");
extern s32 D_8009F748;
extern struct SoundVoice *gVoiceList __asm__("D_8009F74C");
extern s32 D_8009F750;
extern struct SoundVoice *D_8009F754;
extern s32 D_8009F758;

extern void func_800469A0(struct SoundVoice *voice);

/**
 *  @brief Inserts a sound voice into the list selected by its pan field.
 *
 *  Pan values 0 and 2 select separate head-inserted lists; other values use
 *  a list ordered by ascending priority, with equal priorities kept ahead.
 *  @param voice Voice to link and optionally pass for further processing.
 *  @par Globals
 *  D_8009F74C and D_8009F754 are special list heads; D_8009F744 is the ordered head.
 *  D_8009F750, D_8009F758, and D_8009F748 count their respective lists.
 *  @par Control flow
 *  A switch selects the list; the default branch scans for the insertion point.
 *  @par Calls
 *  func_800469A0 processes the voice when its bank is not 0xFF.
 *  @par Side effects
 *  Changes voice links, a list head or neighboring links, and a list count.
 *  @par Callers
 *  Confirmed: func_80046B04.
 */
void func_80046870(struct SoundVoice *voice)
{
    struct SoundVoice *prev;
    struct SoundVoice *next;

    switch (voice->pan) {
    case 0:
        if (gVoiceList != 0) {
            gVoiceList->prev = voice;
        }
        voice->prev = 0;
        voice->next = gVoiceList;
        gVoiceList = voice;
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
        for (next = gVoicePriorityList; next != 0; next = next->next) {
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
            gVoicePriorityList = voice;
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
