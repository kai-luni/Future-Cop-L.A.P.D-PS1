// Categories: Game Engine > Audio
#include "game/types.h"

struct Unk07A8 {
    s32 unk00;
    s32 *unk04;
    s32 unk08;
    u8 *unk0C;
};

extern struct Unk07A8 gSoundSystem __asm__("D_800A07A8");

struct Args {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

struct Entry {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
};

extern u8 *func_80021800(void *base, s32 index);
extern void gComputeStereoVoiceLevels(s32 a0, s32 a1, s32 a2, s32 *a3, s32 *a4) __asm__("func_800223B0");
extern void func_80021A70(u32 flags);

s32 gQueueBankSampleOnChannel(s32 a0, s32 a1, s32 a2, s32 a3, s32 slot, struct Args *args) __asm__("func_80021F98");

/**
 * @brief Resolves a bank sample and queues its parameters on a sound channel.
 *
 * @param a0 Bank index checked against the global bank count.
 * @param a1 Index passed to the bank lookup.
 * @param a2 Record index within the resolved data.
 * @param a3 Unused argument.
 * @param slot Channel slot to initialize and mark.
 * @param args Channel value and inputs for deriving two additional values.
 * @return 1 on successful resolution and initialization, otherwise 0.
 *
 * @par Confirmed mechanics
 * Range-checks the bank index and resolves an indexed bank record, then selects a 20-byte
 * sample at record +4. It uses sample halfword +0x0E for an input of -1, writes the sample
 * pointer and computed left/right levels into a 28-byte channel entry and passes its bit to
 * func_80021A70.
 *
 * @par Globals
 * Reads the bank table and updates the channel array in gSoundSystem.
 *
 * @par Callers
 * Direct calls from func_80024358.
 *
 * @par Role and path
 * Translates a voice request into channel fields consumed by the sound update path.
 *
 * @par Unknown
 * The fourth register argument is ignored and the sample index is not checked here.
 */
s32 gQueueBankSampleOnChannel(s32 a0, s32 a1, s32 a2, s32 a3, s32 slot, struct Args *args)
{
    s32 out1 = 0;
    s32 out2 = 0;
    s32 result = 0;
    u8 *ptr;
    s32 offset;
    struct Entry *entry;

    if (gSoundSystem.unk00 >= a0 && a0 >= 0) {
        ptr = (u8 *)gSoundSystem.unk04[a0];
        if (ptr != 0) {
            ptr = func_80021800(ptr, a1);
            if (ptr != 0) {
                offset = (a2 * 5) * 4;
                offset += 4;
                result = (s32)ptr + offset;
                gComputeStereoVoiceLevels(result, args->unk04, args->unk08, &out1, &out2);
                entry = (struct Entry *)(gSoundSystem.unk0C + slot * 28);
                entry->unk00 = result;
                if (args->unk00 == -1) {
                    entry->unk10 = *(u16 *)(result + 0xE);
                } else {
                    entry->unk10 = args->unk00;
                }
                entry->unk14 = out1;
                entry->unk18 = out2;
                func_80021A70(1 << slot);
                result = 1;
            }
        }
    }
    return result;
}
