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
extern struct SoundBank D_800A7938[];

extern void func_80021DAC(s32 index);
extern void func_800181D4(void *block);
extern void func_800242B4(s32 slot, u8 program);

/**
 *  @brief Releases the first VAB entry matching a nonnegative status value.
 *
 *  Before releasing the entry, scans sound-bank samples and processes each
 *  program whose VAB field matches the requested value.
 *  @param arg0 Status and sample VAB value to match; negative values are ignored.
 *  @par Globals
 *  D_800A0A00 contains eight VAB entries; D_800A7938 contains 110 sound banks.
 *  @par Control flow
 *  Searches for the first matching entry, then scans each bank's samples backward.
 *  @par Calls
 *  func_800242B4 processes matching slot and program pairs; func_80021DAC handles
 *  a valid entry ID; func_800181D4 releases each nonnull block4 and block8.
 *  @par Side effects
 *  Resets the matching entry's ID, released block pointers, status, and pad2.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80023AB4(s32 arg0)
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
            bank = D_800A7938;
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
                func_80021DAC(entry->id);
                entry->id = m1;
            }
            if (entry->block4 != 0) {
                func_800181D4(entry->block4);
                entry->block4 = 0;
            }
            if (entry->block8 != 0) {
                func_800181D4(entry->block8);
                entry->block8 = 0;
            }
            entry->status = m1;
            entry->pad2 = 0;
            break;
        }
        entry++;
    } while (entry < base + 8);
}
