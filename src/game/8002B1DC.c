#include "game/types.h"

typedef struct Target Target;

typedef struct {
    u8 flags;
    u8 pad01[5];
    s16 f06;
    s16 f08;
    s16 f0A;
} Node;

struct Target {
    u8 pad00[0x08];
    s32 unk08;
    u8 pad0C[0x14 - 0x0C];
    u16 unk14;
    u8 pad16[0x19 - 0x16];
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    u8 pad28[0xB8 - 0x28];
    Target *unkB8;
    s32 unkBC;
    u8 padC0[0xDC - 0xC0];
    u8 *unkDC;
    u8 padE0[0x10C - 0xE0];
    s16 unk10C;
    s16 unk10E;
    u8 pad110[0x118 - 0x110];
    u32 unk118;
    u8 pad11C[2];
    s16 unk11E;
    s16 unk120;
    u8 pad122[0x158 - 0x122];
    u16 unk158;
    u8 pad15A[0x15F - 0x15A];
    u8 unk15F;
    u8 unk160;
};

extern void func_8002B1AC(Target *s0);
extern void func_8002B0EC(Target *s0, s32 a1);
extern void func_8003B898(u8 index, Target *entity, s32 arg2, u16 flags, s32 mode);
extern void *jtbl_80010FCC[];
extern void *jtbl_80010FEC[];

s32 func_8002B1DC(Target *s0, s32 a1, Target *a2)
{
    u8 *base;
    Node *cur;
    s16 count;
    s16 curIdx;
    s32 i;
    s32 best;
    s32 bestIdx;
    register s32 d1 __asm__("$3");
    register s32 d2 __asm__("$2");
    register s32 sgn __asm__("$4");
    register s32 ca __asm__("$5");
    register s32 rv __asm__("$2");
    s32 dist;

    if ((u8)(s0->unk15F - 7) < 2) {
        return 0;
    }

    {
        static void *const outer[] = {&&L_case0, &&L_case1, &&L_ret1,
                                      &&L_case5, &&L_case6, &&L_case7};
        if ((u32)a1 < 8) {
            goto *jtbl_80010FCC[a1];
        }
    }
    goto L_ret1;

L_case0:
    s0->unk15F = 0;
    if ((s0->unk160 & 1) == 0) {
        s0->unk10C = (s16)(u16)s0->unk158 >> 2;
    }
    if (s0->unk10E > s0->unk10C) {
        *(u16 *)&s0->unk10E = *(u16 *)&s0->unk10C;
    }
    s0->unk118 &= 0xF0FFFFFF;
    goto L_ret1;

L_case1:
    if (a2 == 0) {
        return 0;
    }
    if (s0->unk15F == 2) {
        return 0;
    }
    if (s0->unk1C < 5) {
        return 0;
    }
    if (s0->unkB8 != 0) {
        if (s0->unkB8 == a2) {
            goto L_bodyB;
        }
        if (s0->unk15F == 1) {
            goto L_bodyB;
        }
    }
    s0->unkB8 = a2;
    s0->unkBC = a2->unk08;
    {
        static void *const inner[] = {&&L_bodyA, &&L_bodyB};
        u8 kind = ((u8 *)&s0->unk118)[3] & 0xF;
        if (kind < 15) {
            goto *jtbl_80010FEC[kind];
        }
    }
    goto L_bodyB;

L_bodyA:
    s0->unk118 = (s0->unk118 & 0xF0FFFFFF) | 0x02000000;
    *(s16 *)&s0->unk11E = a2->unk14 & 0x7FFF;

L_bodyB:
    s0->unk15F = 1;
    func_8002B1AC(s0);
    return 1;

L_case5:
    *(u16 *)&s0->unk10E = 0;
    goto L_ret1;

L_case6:
    base = s0->unkDC;
    best = 0x7FFF0000;
    bestIdx = -1;
    count = *(s16 *)(base + 2);
    curIdx = s0->unk120;
    cur = (Node *)(base + (12 * curIdx + 4));
    for (i = count - 1; i >= 0; i--) {
        u8 *row = base + 12 * i;
        if ((row[4] & 1) && !(row[4] & 2)) {
            Node *n = (Node *)(base + (12 * i + 4));
            d1 = n->f06;
            ca = cur->f06;
            d1 -= ca;
            d2 = n->f0A;
            sgn = cur->f0A;
            d2 -= sgn;
            sgn = d1 >> 31;
            d1 = (sgn + d1) ^ sgn;
            sgn = d2 >> 31;
            d2 = (sgn + d2) ^ sgn;
            dist = d1 + d2;
            if (dist < best) {
                best = dist;
                bestIdx = i;
            }
        }
    }
    if (bestIdx < 0) {
        return 0;
    }
    rv = s0->unk118 & 0xF0FFFFFF;
    rv |= 0x06000000;
    s0->unk15F = 6;
    *(s16 *)&s0->unk11E = bestIdx;
    s0->unk118 = rv;
    func_8002B1AC(s0);
    return 1;

L_case7:
    s0->unk15F = 7;
    func_8002B0EC(s0, 0x3A);
    s0->unk24 |= 1;
    func_8003B898(s0->unk20, s0, 0, (s0->unk19 >> 3) & 0x1E, 0);

L_ret1:
    return 1;
}
