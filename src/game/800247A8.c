#include "game/types.h"

struct Unk247A8 {
    u32 unk0;
    s32 unk4;
};

extern struct Unk247A8 D_800A0AA0[2];
extern struct Unk247A8 D_800A0AB0[2];
extern struct Unk247A8 D_800A0AC0[2];
extern struct Unk247A8 D_800A0AD0[2];
extern struct Unk247A8 D_800A0AE0[2];
extern struct Unk247A8 D_800A0AF0[2];
extern struct Unk247A8 D_800A0B00[2];
extern struct Unk247A8 D_800A0B10[2];
extern struct Unk247A8 D_800A0B20[2];

extern u8 D_8009FA98[];

extern s32 D_8009F660;
extern s32 D_8009F9EC;
extern s32 D_8009FA30;
extern u16 D_8009F65E;
extern u16 D_8009F65C;
extern u16 D_8009FA44;
extern s32 D_8009F664;

void func_800247A8(void)
{
    s32 i;

    D_8009F660 = 0x40000;
    D_8009F9EC = 0;
    D_8009FA30 = 0;
    D_8009F65E = 0;
    D_8009F65C = 0;
    *(u16 *)0x8009FA46 = 0;
    D_8009FA44 = 0;
    D_8009F664 = 2;

    for (i = 0; i < 2; i++) {
        D_800A0AA0[i].unk0 = 0x40000;
        D_800A0AA0[i].unk4 = 2;
        D_800A0AB0[i].unk0 = 0x40000000;
        D_800A0AB0[i].unk4 = 2;
        D_800A0AC0[i].unk0 = 0x80000000;
        D_800A0AC0[i].unk4 = 2;
        D_800A0AD0[i].unk0 = 0x10000000;
        D_800A0AD0[i].unk4 = 2;
        D_800A0AE0[i].unk0 = 0x20000000;
        D_800A0AE0[i].unk4 = 2;
        if (D_8009FA98[i] & 2) {
            D_800A0AF0[i].unk0 = 0x1000000;
            D_800A0B00[i].unk0 = 0x8000000;
            D_800A0B10[i].unk0 = 0x4000000;
            D_800A0B20[i].unk0 = 0x400000;
        } else {
            D_800A0AF0[i].unk0 = 0x4000000;
            D_800A0B00[i].unk0 = 0x400000;
            D_800A0B10[i].unk0 = 0x1000000;
            D_800A0B20[i].unk0 = 0x8000000;
        }
        D_800A0AF0[i].unk4 = 2;
        D_800A0B00[i].unk4 = 2;
        D_800A0B10[i].unk4 = 2;
        D_800A0B20[i].unk4 = 2;
    }
}
