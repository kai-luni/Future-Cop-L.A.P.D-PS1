// Categories: Gameplay > Entity Script > Path Sequence | Graphics > Model Renderer
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

/**
 * @brief Advances an entity along its waypoint sequence.
 *
 * Moves the travelled distance by the segment speed, switches the stop index
 * when a threshold is crossed (ping-ponging between the ends), applies the dwell
 * timer of the new segment, fires the segment events, and then refreshes the
 * basis matrix and the frame vertex.
 *
 * @param arg0 Sequence entity; thresholds at +0xA0, dwell times at +0xAC,
 *        speeds at +0xB8/+0xBC, timer at +0xC0, hold at +0xC4, flags at +0xC6,
 *        count at +0xC8, index at +0xCA, direction at +0xCB.
 * @param arg1 Frame delta in the same units as the speeds.
 * @return Always 0.
 *
 * @par Confirmed
 * obj->unk0C != 0 -> func_80041728(obj) runs the queued script commands first.
 * obj->unkC6 bit 2 skips the whole body; obj->unkC4 > 0 is decremented and also
 * skips it.
 * flag is set when obj is the unkC4 entity of one of the two D_8009FA58 games and
 * that game's unkAC->unk40 is 0.
 * Advance gating: with (unkC6 & 5) != 0 the step only happens once unkC0 is
 * negative and flag is set and bit 4 is clear; otherwise unkC0 is a dwell timer
 * that is decremented and, on expiry or when it was already 0 with flag set, is
 * set to -1 to arm the move.
 * Direction 1: unk54 += arg1 * unkB8; the index only advances when unk54 >=
 * thresholds[oldIndex], and the sequence reverses (unkCB = 2, index clamped to
 * old - 1) when the next index would reach unkC8.
 * Direction 2: unk54 -= arg1 * unkBC, index decrements when
 * thresholds[oldIndex] >= unk54, and the sequence reverses (unkCB = 1, index back
 * to old + 1) below 0.
 * On an index change: unk54 is snapped to thresholds[oldIndex], the dwell timer
 * becomes 1 for the (unkC6 & 5) mode or unkAC[oldIndex] otherwise, unkC9 records
 * the previous index, and if unk21 (saved from unkCC) was set it is cleared and
 * bit 0x400000 of unk24 triggers func_800457B8(unk08, unk22) before being
 * cleared; unkCD != 0 calls func_80047284(unkCD, unk50, unk58, -1).
 * The tail always runs: func_80010750(unk2C) loads the basis and
 * func_80010234(model + (unk70 << 4) + 8, unk5C, 1) transforms the frame vertex.
 *
 * @par Likely
 * A patrol/patrol-platform mover with up to three stops (the +0xA0 array is
 * three words wide in func_80041488), dwell times per stop and ping-pong motion;
 * the func_800457B8/func_80047284 calls Likely start or stop a sound and notify a
 * partner object when the segment changes.
 *
 * @par Unknown
 * Why unkC6 bit 5 changes the dwell timer to a constant 1, and the meaning of
 * unk50/unk58 passed to func_80047284.
 *
 * @par Callers
 * func_80041B6C (sole direct caller).
 */
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

