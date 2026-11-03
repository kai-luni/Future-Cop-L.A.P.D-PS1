#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk331CCGlobal {
    u8 pad00[0x4C];
    s32 unk4C;
    u8 pad50[0x18];
    s32 unk68;
    u8 pad6C[8];
    s32 unk74;
    s32 unk78;
    u8 pad7C[0x20];
    s32 unk9C;
    s32 unkA0[4];
    s32 unkB0[12];
    s32 unkE0;
};

struct Unk331CCEntry {
    u8 pad00[0xC];
    struct Vec3 unkC;
    s16 unk18;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
    u8 unk1D;
    s16 unk1E;
    s16 unk20;
    u8 pad22[2];
    s32 unk24;
    s16 unk28[16];
    s32 unk48;
    s32 unk4C;
    u8 pad50[6];
    u8 unk56;
    u8 pad57[1];
    s16 unk58;
    u8 pad5A[0x24];
    u8 unk7E;
    u8 pad7F[1];
    s16 unk80;
    u8 pad82[0x1E];
    u8 unkA0[0x14];
};

struct Unk331CCObj {
    u8 pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 pad20[0x30];
    struct Vec3 unk50;
    u8 pad5C[0x4A];
    u8 unkA6;
    u8 padA7;
    s16 unkA8;
    u8 padAA[0x4A];
    s32 unkF4;
    u16 unkF8;
    u8 unkFA;
    u8 padFB[0x39];
    struct Unk331CCEntry *unk134;
};

struct Unk331CCSrc {
    u8 pad00[0x26];
    u16 unk26;
    u8 pad28[0x2C];
    u8 unk54;
    u8 pad55[3];
    u8 unk58[2];
    u16 unk5A;
};

struct Unk331CCSrcOuter {
    u8 pad00[8];
    struct Unk331CCSrc *unk8;
};

struct Unk331CCPlayer {
    u8 pad00[0x50];
    struct Vec3 unk50;
};

extern struct Unk331CCGlobal *D_8009FA94;
extern struct Unk331CCPlayer *D_8009FA58;
extern struct Unk331CCObj *D_8009F9EC;

extern void func_80032180(s32, struct Unk331CCObj *, struct Unk331CCSrcOuter *, s32);
extern struct Unk331CCEntry *func_80017EBC(s32, s32);
extern void func_80060668(void *, void *);
extern s32 func_80019668(void);
extern void func_800330E8(struct Unk331CCObj *);
extern void func_8005A09C(void *);
extern void func_8005A0C4(void *, s32, s32);

s32 func_800331CC(s32 arg0, struct Unk331CCObj *obj, struct Unk331CCSrcOuter *arg2, s32 arg3)
{
    struct Unk331CCSrc *src;
    s32 i;
    s32 v;
    u32 f;

    D_8009FA94->unkE0 = 0;
    if (D_8009FA94->unk4C < 0) {
        D_8009FA94->unk4C = 0;
    }
    if (D_8009FA94->unk4C >= 10) {
        D_8009FA94->unk4C = 9;
    }
    src = arg2->unk8;
    f = (src->unk26 | 0x20) & 0xFFF8;
    src->unk26 = f;
    if (D_8009FA94->unk4C > 0) {
        src->unk26 = f | 2;
    } else {
        src->unk26 = f | 6;
    }
    func_80032180(arg0, obj, arg2, arg3);
    obj->unk134 = func_80017EBC(0xB4, 2);

    {
        u32 g;
        s32 w;

        g = (src->unk5A | 0x20) & 0xFFF8;
        src->unk5A = g;
        w = D_8009FA94->unk4C;
        if (w < 3) {
            src->unk5A = g | 6;
        } else if (w < 7) {
            src->unk5A = g | 2;
        }
    }
    func_80060668((u8 *)src + 0x58, (u8 *)obj->unk134 + 0x50);
    func_80060668((u8 *)src + 0x68, (u8 *)obj->unk134 + 0x78);

    {
        s32 w;

        w = D_8009FA94->unk4C;
        if (w < 3) {
            obj->unkF4 -= (3 - w) << 11;
        } else {
            obj->unkF4 += (w - 3) * 1056;
        }
    }
    obj->unk134->unk24 = obj->unkF4;
    obj->unk134->unk20 = obj->unkF8;

    {
        s32 w;

        w = D_8009FA94->unk4C;
        if (w < 6) {
            obj->unk1E -= ((7 - w) >> 1) * ((s16)obj->unk1E >> 2);
            obj->unk1C = obj->unk1E;
        }
    }
    {
        s32 w;

        w = D_8009FA94->unk4C;
        if (w >= 8) {
            obj->unk1E += (w - 7) * ((s16)obj->unk1E >> 3);
            obj->unk1C = obj->unk1E;
        }
    }
    obj->unk134->unk1E = ((s16)obj->unk1E >> 1) - (func_80019668() & 0x3F);
    obj->unk134->unk1B = src->unk54;
    obj->unk134->unk1D = obj->unkFA;
    obj->unk134->unk1A = 2;
    D_8009FA94->unk9C = 2;
    obj->unk134->unk18 = 0;
    obj->unk134->unk56 = obj->unk134->unk7E = 4;
    obj->unkA6 = 4;
    obj->unkA8 = obj->unk134->unk58 = obj->unk134->unk80 = obj->unk134->unk1B;
    obj->unk134->unk1C = func_80019668() & 1;

    for (i = 3; i >= 0; i--) {
        D_8009FA94->unkA0[i] = 0;
    }
    for (i = 0x2C; i < 0x38; i++) {
        ((s32 *)D_8009FA94)[i] = 0;
    }

    if (D_8009FA94->unk4C > 0) {
        s32 r = (func_80019668() & 3) + 9;
        v = r - D_8009FA94->unk4C;
        if (v >= 11) {
            v = 10;
        }
        D_8009FA94->unkB0[0] = v;
        D_8009FA94->unk74 = v;
        if (v < 10) {
            D_8009FA94->unkB0[1] = 10 - v;
            D_8009FA94->unk78 = 10 - v;
        }
    } else {
        D_8009FA94->unkB0[0] = 0;
        D_8009FA94->unk74 = 0;
        D_8009FA94->unkB0[1] = 0;
        D_8009FA94->unk78 = 0;
    }
    D_8009FA94->unk68 = 0;
    obj->unk134->unkC = obj->unk50;
    *(struct Vec3 *)obj->unk134 = D_8009FA58->unk50;
    func_800330E8(obj);
    for (i = 0; i < 16; i++) {
        obj->unk134->unk28[i] = 0;
    }
    obj->unk134->unk48 = 0;
    obj->unk134->unk4C = 0;
    D_8009F9EC = obj;
    func_8005A09C(obj->unk134->unkA0);
    func_8005A0C4(obj->unk134->unkA0, obj->unk1C, obj->unk1E);
    return 0;
}
