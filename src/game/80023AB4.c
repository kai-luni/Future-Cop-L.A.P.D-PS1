// Categories: Game Engine > Audio | Resources > Sound Banks
#include "game/types.h"

struct SoundSample {
    u8 program;
    u8 unk1;
    s8 vab;
    u8 unk3;
};

struct SoundBank {
    struct SoundSample samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    void *head;
};

struct VabEntry {
    s8 id;
    s8 status;
    s8 pad2;
    s8 pad3;
    void *block4;
    void *block8;
    void *blockC;
};

extern struct VabEntry gObjectSlots[] __asm__("D_800A0A00");
extern struct SoundBank gSoundEntryTable[] __asm__("D_800A7938");

extern void gUnregisterSoundBank(s32 index) __asm__("func_80021DAC");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void func_800242B4(s32 slot, u8 program);

void gUnloadCvkSoundBank(s32 arg0) __asm__("func_80023AB4");

/**
 * @brief Unloads the first CVK bank slot matching a nonnegative group ID.
 *
 * @param arg0 Status and sample VAB value to match; negative values are ignored.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Scans eight slots by signed byte +1. For the first match, scans the 110 program records
 * backward within each sample list and removes samples with that group byte. Releases bank byte
 * +0 if valid, frees both nonnull slot pointers, then writes -1 to +1 and zero to +2.
 *
 * @par Globals
 * Reads and updates gObjectSlots and gSoundEntryTable.
 *
 * @par Callers
 * Direct calls from gReleaseCvkSoundBanksAndVoices (func_80023BF8).
 *
 * @par Role and path
 * Called during CVK teardown to drop a group's samples, sound-bank registration and backing
 * nodes.
 *
 * @par Unknown
 * Only the first matching slot is retired.
 */
void gUnloadCvkSoundBank(s32 arg0)
{
    register s32 arg __asm__("$21") = arg0;
    register s32 m1 __asm__("$22");
    register struct VabEntry *entry __asm__("$19");
    struct SoundBank *bank;
    u32 slot;
    struct SoundSample *samp;
    struct VabEntry *base;
    s32 idx;

    if (arg < 0) {
        return;
    }

    m1 = -1;
    base = gObjectSlots;
    entry = base;

    do {
        if (entry->status == arg) {
            bank = gSoundEntryTable;
            slot = 0;
            do {
                idx = bank->count - 1;
                if (idx >= 0) {
                    samp = (struct SoundSample *)(idx * 4 + (s32)bank);
                    do {
                        if (samp->vab == arg) {
                            func_800242B4(slot, samp->program);
                        }
                        idx--;
                        samp--;
                    } while (idx >= 0);
                }
                slot++;
                bank++;
            } while (slot < 0x6E);

            if (entry->id != m1) {
                gUnregisterSoundBank(entry->id);
                entry->id = m1;
            }
            if (entry->block4 != 0) {
                gFreeBlock(entry->block4);
                entry->block4 = 0;
            }
            if (entry->block8 != 0) {
                gFreeBlock(entry->block8);
                entry->block8 = 0;
            }
            entry->status = m1;
            entry->pad2 = 0;
            break;
        }
        entry++;
    } while (entry < base + 8);
}
