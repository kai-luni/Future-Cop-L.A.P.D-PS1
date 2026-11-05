#include "game/types.h"

struct Unk381D4 {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[0x86];
    u16 unkA4;
    u8 padA6[0x4];
    s16 unkAA;
    s32 unkAC;
    s32 unkB0;
    u16 unkB4;
    u8 padB6[0x62];
    u32 unk118;
    u8 pad11C[0x14];
    s16 unk130;
    u8 pad132[0x4];
    u16 unk136;
};

extern s32 func_80019668(void);

void func_800381D4(struct Unk381D4 *arg)
{
    s32 v;

    switch (arg->unk118 >> 30) {
    case 0:
        if (((u32)func_80019668() & 0xF) < 0xC) {
            arg->unk118 |= 0x40000000;
            arg->unkAC = 0;
            arg->unkB4 = arg->unk136;
            arg->unkA4 |= 0x100;
            arg->unkB0 = arg->unk130;
            v = (func_80019668() & 3) + 1;
            if ((s16)arg->unk136 != 0) {
                if ((s16)arg->unk136 >= 0x801) {
                    v = -v;
                }
            } else if (v & 1) {
                v = -v;
            } else {
                arg->unkAA = v;
            }
            arg->unkAA = v;
            arg->unk118 &= -6;
        } else {
            break;
        }
    case 1:
        arg->unk1C = 0;
        break;
    }
}
