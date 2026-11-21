// Categories: Platform > Audio
#include "game/types.h"

struct SoundVoice {
    u8 unk00[0x0D];
    u8 index;
    u8 unk0E[2];
    s32 unk10;
    u8 unk14[8];
    struct SoundVoice *next;
    u8 unk20[0x0C];
};

struct Unk453B8Bank {
    s32 unk00;
    u8 unk04[0x10];
};

struct Unk453B8Stream {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

extern struct SoundVoice D_800AACC8[];
extern struct Unk453B8Bank gPlayerSlots[] __asm__("D_800A10E0");
extern struct Unk453B8Stream gStreamTable[] __asm__("D_800A1810");
extern struct SoundVoice *gVoiceFreeList __asm__("D_8009F73C");
extern s32 gFreeSoundVoiceCount __asm__("D_8009F740");
extern struct SoundVoice *gVoicePriorityList __asm__("D_8009F744");
extern struct SoundVoice *gPrioritySoundVoiceCount __asm__("D_8009F748");
extern struct SoundVoice *gVoiceList __asm__("D_8009F74C");
extern struct SoundVoice *gClass0SoundVoiceCount __asm__("D_8009F750");
extern struct SoundVoice *gClass2SoundVoiceList __asm__("D_8009F754");
extern struct SoundVoice *gClass2SoundVoiceCount __asm__("D_8009F758");
extern s32 gPendingSoundVoiceReleaseCount __asm__("D_8009F32C");
extern u8 gVolumeLimitSelector __asm__("D_8009F738");

extern void gResetSoundBankTables(void) __asm__("func_80044EF8");
extern void func_8004582C(void);
extern s32 gAllocReservedStreamVoice(void) __asm__("func_80046AD8");

/**
 *  @brief Initializes voice, bank and stream state for the audio subsystem.
 *
 *  The voice array becomes a 24-element linked list, bank markers are
 *  cleared, and two stream records receive values from a helper.
 *  @par Globals
 *  D_800AACC8 holds voices; D_8009F73C and D_8009F740 hold the list and count;
 *  D_8009F744, D_8009F748, D_8009F74C, D_8009F750, D_8009F754 and
 *  D_8009F758 are cleared. D_800A10E0 and D_800A1810 hold banks and streams;
 *  D_8009F32C and D_8009F738 are also cleared.
 *  @par Control flow
 *  Links 24 voices, clears six bank markers, and initializes two streams.
 *  @par Calls
 *  func_80044EF8 and func_8004582C initialize related state;
 *  func_80046AD8 supplies a value for each stream.
 *  @par Side effects
 *  Rewrites the listed global audio state and invokes initialization helpers.
 *  @par Callers
 *  Confirmed: func_80044F8C.
 */
void func_800453B8(void)
{
    u32 i;
    struct Unk453B8Bank *bank;
    struct Unk453B8Stream *stream;
    struct SoundVoice *voice;

    gVoiceFreeList = D_800AACC8;
    voice = D_800AACC8;
    gFreeSoundVoiceCount = 0x18;
    gVoicePriorityList = 0;
    gPrioritySoundVoiceCount = 0;
    gVoiceList = 0;
    gClass0SoundVoiceCount = 0;
    gClass2SoundVoiceList = 0;
    gClass2SoundVoiceCount = 0;
    for (i = 0; i < 0x18; i++) {
        voice->index = i;
        voice->next = voice + 1;
        voice->unk10 = 0;
        voice++;
    }
    voice[-1].next = 0;
    gResetSoundBankTables();
    func_8004582C();
    bank = gPlayerSlots;
    for (i = 0; i < 6; i++, bank++) {
        bank->unk00 = 0;
    }
    gPendingSoundVoiceReleaseCount = 0;
    i = 0;
    stream = gStreamTable;
    while (i < 2) {
        stream->unk08 = gAllocReservedStreamVoice();
        stream->unk00 = 0;
        i++;
        stream++;
    }
    gVolumeLimitSelector = 0;
}
