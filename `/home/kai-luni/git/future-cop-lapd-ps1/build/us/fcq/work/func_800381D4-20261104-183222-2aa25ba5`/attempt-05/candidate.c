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
    u32 state;
    u32 kind;
    s32 r;
    u16 a;
    u16 b;
    s16 c;
    s16 a0;
    s32 v;

    state = arg->unk118;
    kind = state >> 30;
    if (kind == 0) {
        r = func_80019668();
        if ((r & 0xF) >= 0xC) {
            return;
        }
        arg->unk118 |= 0x40000000;
        a = arg->unk136;
        b = arg->unkA4;
        c = arg->unk130;
        arg->unkAC = 0;
        arg->unkB4 = a;
        arg->unkA4 = b | 0x100;
        arg->unkB0 = c;
        r = func_80019668() & 3;
        a0 = arg->unk136;
        v = r + 1;
        if (a0 == 0) {
            arg->unkAA = v;
            if (v & 1) {
                v = -v;
            }
        } else if (a0 >= 0x801) {
            v = -v;
        }
        arg->unkAA = v;
        arg->unk118 &= -6;
    } else if (kind != 1) {
        return;
    }
    arg->unk1C = 0;
}
