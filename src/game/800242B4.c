#include "game/types.h"

struct SoundBankEntry {
    u8 key;
    u8 unk[3];
};

struct SoundBank {
    struct SoundBankEntry entries[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 used;
    u8 unk15;
    u8 key;
    u8 unk17;
    s32 head;
};

extern struct SoundBank D_800A7938[];

s32 func_800242B4(s32 bankIndex, u8 key)
{
    struct SoundBank *bank;
    s32 i;
    register s32 result __asm__("$6");

    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
    bank = &D_800A7938[bankIndex];
    for (i = 0; i < bank->count; i++) {
        if (bank->entries[i].key == key) {
            bank->count--;
            if (i != bank->count) {
                bank->entries[i] = bank->entries[bank->count];
            }
            result = 1;
            break;
        }
    }
    return result;
}
