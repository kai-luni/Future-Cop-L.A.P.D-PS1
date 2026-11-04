#include "game/types.h"

struct Unk5FF70 {
    u8  key;
    u8  pad01[1];
    u16 f02;
    u16 f04;
    u16 f06;
    u16 f08;
    u16 f0A;
    u8  pad0C[0x24];
};

struct Unk60668In {
    s16 f00;
    u16 f02;
    u8  f04;
    u8  f05;
    u16 f06;
    u16 f08;
    u16 f0A;
    u16 f0C;
    s16 f0E;
};

struct Unk60668Out {
    struct Unk5FF70 *f00;
    u16 f04;
    u8  f06;
    u8  f07;
    u16 f08;
    u16 f0A;
    s32 f0C;
    s32 f10;
    s16 f14;
    s16 f16;
    s32 f18;
    s32 f1C;
    u16 f20;
    u8  f22;
    u8  f23;
    u16 f24;
    u16 f26;
};

extern s32 func_8005FF70(s32 key);
extern s32 func_8006062C(u32 id);
extern struct Unk5FF70 D_800A2300[];

void func_80060668(struct Unk60668In *in, struct Unk60668Out *out)
{
    s32 index;
    s32 scale;
    u16 v20;

    if (in->f00 < 0) {
        out->f00 = 0;
    } else {
        index = func_8005FF70(in->f00);
        out->f00 = &D_800A2300[index];
        out->f04 = in->f02;
        out->f06 = in->f05;
        out->f08 = in->f06;
        out->f07 = in->f04;
        out->f0C = func_8006062C((s16)in->f08 / 2);
        out->f10 = func_8006062C((s16)in->f0A / 2);
        out->f0A = in->f0C;
        scale = in->f0E << 12;
        out->f14 = (s16)(scale / 218);
        out->f16 = 0;
        v20 = out->f00->f02;
        out->f18 = 0;
        out->f1C = 0;
        out->f22 = 0;
        out->f23 = 0;
        out->f20 = v20;
        out->f24 = out->f00->f0A;
        out->f26 = out->f00->f06;

    }
}
