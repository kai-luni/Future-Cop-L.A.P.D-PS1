#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    s32 unk24;
    s32 unk28[2];
    s32 unk30;
    u8 pad34[0xE0];
    s32 unk114;
    s32 unk118;
    s32 unk11C;
} State;

typedef struct {
    u16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Local;

extern State *D_8009F4BC;
extern u32 D_8009F4C0;
extern u32 D_8009F4C8;
extern u32 D_8009F4D0;

extern s32 func_80095FE8(u16 *value);
extern void func_80096070(s32 destination, u32 size);
extern s32 func_800960CC(s32 mode);
extern void func_80095E90(s32 option);
extern void func_80095FF4(s32 arg0, s32 arg1);
extern void func_80089BBC(u16 *arg0, s32 arg1);
extern void func_80092F38(Local *local, s32 arg1);

void func_80085630(u8 *data, u32 count, s32 volume)
{
    Local local;
    u32 magic;
    u32 s4;
    u32 s3;
    u32 s2;
    s32 s1;
    u32 i;
    u32 result;
    u16 *src;
    u8 *p;
    State *st;

    magic = *(u32 *)data;
    s4 = (magic >> 24) | ((magic >> 8) & 0xFF00) |
         ((magic & 0xFF00) << 8) | (magic << 24);

    p = data + 8;
    s3 = (*(u16 *)(p + 2) >> 8) |
         ((*(u16 *)(p + 2) & 0xFF) << 8);
    s2 = (*(u16 *)(data + 8) >> 8) |
         ((*(u16 *)(data + 8) & 0xFF) << 8);

    if (D_8009F4BC->unk114 != 0) {
        local.unk2 = volume + 0x100;
    } else {
        local.unk2 = volume;
    }

    {
        register u32 rate __asm__("$2") = D_8009F4C8;
        local.unk6 = s3;
        local.unk0 = (count * rate) >> 1;
        local.unk4 = rate << 3;
    }

    src = (u16 *)(p + 8);
    result = func_80095FE8(src);
    if (D_8009F4D0 < result) {
        D_8009F4D0 = result;
    }

    st = D_8009F4BC;
    if (st->unk118 == 0) {
        if (s4 == 0x4D444543) {
            func_80089BBC(src, st->unk28[st->unk30]);
        }
        *(volatile s32 *)&D_8009F4BC->unk11C = 0;
    } else {
        s1 = st->unk24;
        func_80095FF4(st->unk28[1 - st->unk30], D_8009F4C0);
        func_80096070(s1, ((s3 * (D_8009F4C8 << 3)) >> 1) * (s2 >> 4));
        if (s4 == 0x4D444543) {
            func_80089BBC(src, D_8009F4BC->unk28[D_8009F4BC->unk30]);
        }
        func_800960CC(0);
        for (i = 0; i < s2; i += 0x10) {
            func_80092F38(&local, s1);
            s1 += (local.unk4 * local.unk6) << 1;
            { u32 step = D_8009F4C8 << 3; local.unk0 += step; }
        }
        D_8009F4BC->unk11C = 1;
    }

    func_80095E90(1);

    D_8009F4BC->unk118++;
    D_8009F4BC->unk30 = 1 - D_8009F4BC->unk30;
}
