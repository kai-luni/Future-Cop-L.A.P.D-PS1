#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk22C0Sub {
    u8 pad00[4];
    void (*unk04)(s32, void *, void *, s32);
    s32 unk08;
};

struct Unk22C0 {
    struct Unk22C0Sub *unk00;
    struct Unk22C0Sub *unk04;
    s32 unk08;
    s32 unk0C;
    struct Vec3s32 unk10;
    struct Vec3s32 unk1C;
    s16 unk28;
    u16 unk2A;
    s16 unk2C;
};

struct Local22C0 {
    u32 unk00;
    u16 unk04;
    u8 unk06;
    u8 pad07;
    void *unk08;
};

struct Unk73500;

extern struct Unk22C0 D_800A22C0;

extern struct Unk73500 *func_80073500(struct Vec3s32 *position, s16 arg1, s16 arg2, s16 arg3);
extern void func_8007377C(void *arg0, void *arg1, s32 arg2, s32 arg3,
                          s32 arg4, s32 arg5, s32 arg6, s32 arg7,
                          s32 arg8, s32 arg9, s32 arg10, s32 arg11,
                          s32 arg12, s32 arg13);
extern s32 func_8001A4E8(s32 *a, s32 *b);

void func_8005B3D4(void *arg0)
{
    struct Local22C0 local;

    if (D_800A22C0.unk28 < 0) {
        return;
    }

    if (D_800A22C0.unk00 != 0) {
        if (D_800A22C0.unk08 != D_800A22C0.unk00->unk08) {
            D_800A22C0.unk00 = 0;
        }
    }

    if (D_800A22C0.unk04 != 0) {
        if (D_800A22C0.unk0C != D_800A22C0.unk04->unk08) {
            D_800A22C0.unk04 = 0;
        }
    }

    D_800A22C0.unk28 -= (s32)arg0;
    if (D_800A22C0.unk28 >= 0) {
        return;
    }

    if (D_800A22C0.unk04 == 0) {
        return;
    }
    if (D_800A22C0.unk00 == 0) {
        return;
    }

    func_8007377C(
        func_80073500(&D_800A22C0.unk1C, 0, 0x100, 0),
        func_80073500(&D_800A22C0.unk10, 0, 0x100, 0),
        4, 1, 0xFF, 0x60, 0, 0x800, 0, 0, 0, 0, 0, 1);

    if (func_8001A4E8((s32 *)&D_800A22C0.unk10,
                      (s32 *)((u8 *)D_800A22C0.unk00 + 0x5C)) >=
        (D_800A22C0.unk2C + *(s16 *)((u8 *)D_800A22C0.unk00 + 0x6E)) * 16) {
        return;
    }

    local.unk06 = 8;
    local.unk04 = D_800A22C0.unk2A;
    local.unk00 = *(u16 *)((u8 *)D_800A22C0.unk04 + 0x14);
    local.unk08 = D_800A22C0.unk04;

    D_800A22C0.unk00->unk04(0xE, D_800A22C0.unk00, &local, 0);
}
