#include "game/types.h"

struct Sample {
    u8 a, b, c, d;
};

struct SoundBank {
    struct Sample samples[4];
    u8 count;
    u8 pad[3];
    u8 enabled;
    u8 pad2[3];
    s32 head;
};

extern struct SoundBank D_800A7938[];
extern s32 func_80047344(void *self, s32 key);

s32 func_800241D0(s32 key, s32 a, s32 c, s32 b, void *self)
{
    s32 slot;
    s32 result;
    struct SoundBank *bank;

    slot = func_80047344(self, key & 0xFF);
    result = 0;
    if (slot != -1) {
        bank = &D_800A7938[slot];
        if (bank->enabled != 0 && bank->count < 4) {
            bank->samples[bank->count].a = a;
            bank->samples[bank->count].c = c;
            bank->samples[bank->count].b = b;
            bank->count++;
            result = 1;
        }
    }
    return result;
}
