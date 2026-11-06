#include "game/types.h"

struct Obj41880;

struct UnkAC {
    u8 pad[0x40];
    s32 unk40;
};

struct Game41880 {
    u8 pad00[0xAC];
    struct UnkAC *unkAC;
    u8 padB0[0x14];
    struct Obj41880 *unkC4;
};

struct Model41880 {
    u8 pad[0x34];
    u8 *unk34;
};

union UnkA0 {
    u8 byte;
    s32 array[3];
};

struct Obj41880 {
    u8 pad00[0x8];
    s32 unk08;
    u8 unk0C;
    u8 pad0D[0x14];
    u8 unk21;
    s8 unk22;
    u8 pad23[1];
    u32 unk24;
    struct Model41880 *unk28;
    u8 unk2C[0x24];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 unk5C[0x14];
    s16 unk70;
    u8 pad72[0x2E];
    union UnkA0 unkA0;
    s32 unkAC[3];
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    s16 unkC4;
    u8 unkC6;
    u8 padC7;
    s8 unkC8;
    s8 unkC9;
    s8 unkCA;
    s8 unkCB;
    u8 unkCC;
    u8 unkCD;
    u8 padCE[2];
};

struct Data41C6C {
    u8 pad00[0x24];
    s16 unk24;
    s16 unk26;
    s16 unk28;
    u8 unk2A;
};

struct Arg2_41C6C {
    u8 pad00[8];
    struct Data41C6C *unk8;
};

extern struct Game41880 *D_8009FA58[2];

extern void func_80041728(struct Obj41880 *arg0);
extern void func_800457B8(s32 arg0, s32 arg1);
extern void func_80047284(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_80010750(void *arg0);
extern void func_80010234(void *arg0, void *arg1, s32 arg2);
extern void func_80041488(struct Obj41880 *arg0, s32 arg1, s32 arg2);
extern void func_80063194(s32 arg0);
extern s32 func_8002AD58(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_800188D0(s32 arg0, s32 arg1, s32 arg2, void *arg3);
extern s32 func_80066F78(void *arg0, s32 arg1, s32 arg2, s32 arg3);
extern s32 func_8006314C(s32 arg0, u8 arg1);

s32 func_80041880(struct Obj41880 *arg0, s32 arg1)
{
    struct Game41880 *game;
    s32 i;
    u8 flag;
    s32 oldIndex;

    if (arg0->unk0C != 0) {
        func_80041728(arg0);
    }
    if (arg0->unkC6 & 2) {
        goto end;
    }
    if (arg0->unkC4 > 0) {
        arg0->unkC4--;
        goto end;
    }

    flag = 0;
    {
        register struct Game41880 **p __asm__("$4");
        i = 0;
        p = D_8009FA58;
        for (; i < 2; i++, p++) {
            game = *p;
            if (game != 0 && arg0 == game->unkC4) {
                if (game->unkAC->unk40 == 0) {
                    flag = 1;
                    break;
                }
            }
        }
    }

    if ((arg0->unkC6 & 5) != 0) {
        if (arg0->unkC0 >= 0) {
            if (flag == 0) goto end;
            if (arg0->unkC6 & 4) goto end;
            goto set_negative;
        }
    } else {
        if (arg0->unkC0 > 0) {
            arg0->unkC0 -= arg1;
            if (arg0->unkC0 > 0) goto end;
            goto set_negative;
        } else if (arg0->unkC0 == 0) {
            if (flag == 0) goto end;
        set_negative:
            arg0->unkC0 = -1;
        }
    }

    arg0->unk21 = arg0->unkCC;
    oldIndex = arg0->unkCA;
    if (arg0->unkCB == 1) {
        arg0->unk54 += arg1 * arg0->unkB8;
        if (arg0->unk54 < arg0->unkA0.array[oldIndex]) {
            goto after_index;
        }
        arg0->unkCA = oldIndex + 1;
        if ((s8)(oldIndex + 1) < arg0->unkC8) {
            goto after_index;
        }
        arg0->unkCA = oldIndex - 1;
        arg0->unkCB = 2;
    } else {
        arg0->unk54 -= arg1 * arg0->unkBC;
        if (arg0->unkA0.array[oldIndex] < arg0->unk54) {
            goto after_index;
        }
        arg0->unkCA = oldIndex - 1;
        if ((s8)(oldIndex - 1) >= 0) {
            goto after_index;
        }
        arg0->unkCA = oldIndex + 1;
        arg0->unkCB = 1;
    }

after_index:
    if (oldIndex != arg0->unkCA) {
        arg0->unk54 = arg0->unkA0.array[oldIndex];
        if (arg0->unkC6 & 5) {
            arg0->unkC0 = 1;
        } else {
            arg0->unkC0 = arg0->unkAC[oldIndex];
        }
        arg0->unkC9 = oldIndex;
        if (arg0->unk21 != 0) {
            arg0->unk21 = 0;
            if (arg0->unk24 & 0x400000) {
                func_800457B8(arg0->unk08, arg0->unk22);
                arg0->unk24 &= ~0x400000;
            }
        }
        if (arg0->unkCD != 0) {
            func_80047284(arg0->unkCD, arg0->unk50, arg0->unk58, -1);
        }
    }

    func_80010750(arg0->unk2C);
    func_80010234(arg0->unk28->unk34 + (((s32)arg0->unk70 << 4) + 8), arg0->unk5C, 1);
end:
    return 0;
}

