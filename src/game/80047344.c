#include "game/types.h"

struct Unk7938 {
    u8 unk00[0x10];
    u8 active;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    s32 unk18;
};

struct SoundObj {
    u16 id;
    u8 pad[2];
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    u8 unk08[4];
};

extern u8 D_8009F75C;
extern struct Unk7938 D_800A7938[];
extern u8 D_800A1830[];
extern u8 D_800A1880[];
extern u8 D_800A188A[];

extern s32 func_800472E8(s32 key);

s32 func_80047344(struct SoundObj *self, s32 key)
{
    s32 slot;
    s32 i;
    s32 value;
    struct Unk7938 *entry;

    slot = func_800472E8(key & 0xFF);
    if (slot != -1 || D_8009F75C >= 0x6E) {
        return slot;
    }

    slot = D_8009F75C;
    D_8009F75C = slot + 1;
    entry = &D_800A7938[slot];

    self->id = slot;
    if (self->unk04 == 0) {
        self->unk04 = 1;
    } else if (self->unk04 > 0xA) {
        self->unk04 = 0xA;
    }
    entry->unk13 = self->unk04;

    if (self->unk05 > 4) {
        self->unk05 = 0x4;
    }
    entry->unk14 = self->unk05;
    entry->unk15 = self->unk06;
    entry->unk16 = key;

    for (i = 0; i < 4; i++) {
        value = self->unk08[i];
        if (value > 0) {
            if (value >= 0x46) {
                if (value < 0x82) {
                    D_800A188A[value] = slot;
                }
            } else {
                if (self->unk07 == 1) {
                    D_800A1830[value] = slot;
                } else {
                    D_800A1880[value] = slot;
                }
            }
        }
    }
    return slot;
}
