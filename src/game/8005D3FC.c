#include "game/types.h"

struct Entity;

struct Sub {
    s32 x;
    s32 y;
    s32 scale;
    s16 angle;
    u8 pad_0E[2];
    s16 velocity;
    u8 pad_12[2];
    s32 timer;
    u8 enabled;
    u8 state;
    u8 pad_1A[2];
};

struct Block {
    void *unk00;
    struct Entity *unk04;
    s32 unk08;
    s32 unk0C;
    u8 unk10[4];
    s32 unk14;
    u8 unk18[0x30];
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E[2];
    s16 unk50;
    s16 unk52;
    s32 unk54;
    struct Sub subs[12];
    u8 unk1A8;
    u8 unk1A9;
    u8 unk1AA;
    u8 unk1AB;
    u8 unk1AC;
    u8 unk1AD;
    u8 unk1AE;
    u8 unk1AF;
};

struct Entity {
    u8 unk00[8];
    s32 unk08;
    u8 unk0C[0xA];
    u8 unk16;
    u8 unk17[0x57];
    u16 unk6E;
    u8 unk70[0x3C];
    u8 *unkAC;
};

extern struct Block *D_800A22F0[4];

extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_8006C84C(s32 arg0, s32 arg1);
extern void func_8005CEA8(struct Sub *state);

struct Block *func_8005D3FC(struct Entity *ent, s32 idx)
{
    register struct Block **p __asm__("$16");
    struct Block **base;
    struct Block **q;
    s32 c7F;
    s32 c60;
    s32 c40;
    s32 i;

    c7F = 0x7F;
    c60 = 0x60;
    c40 = 0x40;

    base = D_800A22F0;
    __asm__("" : "+r"(base));
    p = base;
    do {
        if (*p == 0) {
            *p = func_80017EBC(0x1B0, 2);

            if (ent->unk16 == 1) {
                if (*(s16 *)(ent->unkAC + (idx << 1) + 0x8A) > 0) {
                    (*p)->unk00 = func_8006C84C(0x100, 9);
                    (*p)->unk1AB = c7F;
                    (*p)->unk1AC = c60;
                    (*p)->unk1AD = 0;
                    (*p)->unk1A8 = c40;
                    (*p)->unk1A9 = c40;
                    (*p)->unk1AA = c60;
                } else {
                    (*p)->unk00 = func_8006C84C(0x100, 9);
                    (*p)->unk1AB = c60;
                    (*p)->unk1AC = 0x30;
                    (*p)->unk1AD = 0;
                    (*p)->unk1A8 = c40;
                    (*p)->unk1A9 = c40;
                    (*p)->unk1AA = c40;
                }
            } else {
                (*p)->unk00 = func_8006C84C(0x200, 0xD);
                (*p)->unk1AB = c7F;
                (*p)->unk1AC = c7F;
                (*p)->unk1AD = c7F;
                (*p)->unk1A8 = c60;
                (*p)->unk1A9 = 0x30;
                (*p)->unk1AA = 0;
            }

            (*p)->unk04 = ent;
            (*p)->unk08 = ent->unk08;
            (*p)->unk52 = 0;
            (*p)->unk0C = 0;
            (*p)->unk4C = ent->unk6E * 2;
            (*p)->unk48 = 0;
            (*p)->unk4A = 0;
            (*p)->unk50 = 0;
            i = 0;
            (*p)->unk14 = 0;
            q = p;
            (*p)->unk54 = idx;
            (*p)->unk1AE = 0;

            for (; i < 12; i++) {
                func_8005CEA8((struct Sub *)((u8 *)*q + 0x58 + i * 0x1C));
            }

            return *p;
        }
        p++;
    } while ((s32)p < (s32)(base + 4));

    return 0;
}
