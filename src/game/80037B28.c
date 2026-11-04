#include "game/types.h"

extern s32 func_8001AB00(s32 *a, s32 *b);
extern s32 func_8001A100(s32 *vector);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

typedef struct SubObject {
    s32 unk00;      // 0x00 (0xDC)
    u8 pad04[0x18]; // 0x04 to 0x1C
    s32 unk1C;      // 0x1C (0xF8)
    s16 unk20;      // 0x20 (0xFC)
    u8 pad22[0x0E]; // 0x22 to 0x30
    s16 unk30;      // 0x30 (0x10C)
    s16 unk32;      // 0x32 (0x10E)
    u8 pad34[0x08]; // 0x34 to 0x3C
    u32 unk3C;      // 0x3C (0x118)
    s16 unk40;      // 0x40 (0x11C)
    u8 pad42[0x02]; // 0x42 to 0x44
    s16 unk44;      // 0x44 (0x120)
    s16 unk46;      // 0x46 (0x122)
    s16 unk48;      // 0x48 (0x124)
} SubObject;

typedef struct Object {
    u8 pad00[0x16];
    u8 unk16;              // 0x16
    u8 pad17[0x0D];        // 0x17 to 0x24
    s32 unk24;             // 0x24
    u8 pad28[0x34];        // 0x28 to 0x5C
    s32 x;                 // 0x5C
    s32 y;                 // 0x60
    s32 z;                 // 0x64
    u8 pad68[0x04];        // 0x68 to 0x6C
    s16 unk6C;             // 0x6C
    u8 pad6E[0x0A];        // 0x6E to 0x78
    s32 unk78[3];          // 0x78
    s32 unk84;             // 0x84
    struct Object *target; // 0x88
    u8 pad8C[0x04];        // 0x8C to 0x90
    s16 unk90;             // 0x90
    u8 pad92[0x4A];        // 0x92 to 0xDC
    SubObject subDC;       // 0xDC
} Object;

void func_80037B28(s32 a0, Object *self, s32 unused_arg2) {
    s32 diff[3];
    Object *target;
    register SubObject *sub2 __asm__("$5");
    register SubObject *sub1 __asm__("$7");
    register s32 a2 __asm__("$6");
    register s32 a1 __asm__("$5");
    register s32 v1 __asm__("$3");
    register s32 v0 __asm__("$2");
    u32 flags;
    s32 s0;

    flags = self->subDC.unk3C;
    if (self->unk90 >= 0x4000) {
        return;
    }
    if (flags & 0x48) {
        return;
    }
    if ((flags >> 30) == 1) {
        return;
    }

    target = self->target;
    diff[0] = target->x - self->x;
    v1 = target->y;
    v1 -= self->y;
    a1 = v1 >> 31;
    diff[1] = v1;
    diff[2] = target->z - self->z;
    v1 = a1 + v1;

    a2 = v1 ^ a1;
    a2 -= (target->unk6C - self->unk6C) << 4;
    v0 = a2 < 0x1401;
    if (!v0) {
        return;
    }

    s0 = func_8001AB00(self->unk78, diff);
    a2 = func_8001A100(diff);

    if (a2 >= 16) {
        if (s0 > 0) {
            if (a2 > 0) {
                v1 = a2 >> 1;
                MULU16(v0, s0, 0x80000000u / (u32)v1);
                goto set_s0;
            }
            v1 = -a2;
            v1 >>= 1;
            MULU16(v0, s0, 0x80000000u / (u32)v1);
            v0 = -v0;
            goto set_s0;
        }
        a0 = -s0;
        if (a2 > 0) {
            v1 = a2 >> 1;
            MULU16(v0, a0, 0x80000000u / (u32)v1);
            v0 = -v0;
            goto set_s0;
        }
        v1 = -a2;
        v1 >>= 1;
        MULU16(v0, a0, 0x80000000u / (u32)v1);
set_s0:
        s0 = v0;
    } else {
        s0 = 0x10000;
    }

    sub2 = &self->subDC;

    if (s0 <= 0xF600) {
        if (self->unk90 >= 0x2000) {
            return;
        }
        if (s0 <= 0x8000) {
            return;
        }
    }

    sub1 = &target->subDC;
    if (target->unk24 < 0) {
        if (target->subDC.unk00 != self->subDC.unk00) {
            goto match_DE0;
        }
        if (sub1->unk44 != sub2->unk44) {
            goto label_DC0;
        }
        if ((sub1->unk20 << 8) - sub1->unk1C < (sub2->unk20 << 8) - sub2->unk1C) {
            goto match_DE0;
        }
        return;

label_DC0:
        if (sub2->unk44 == sub1->unk48) {
            goto match_DE0;
        }
        if (sub2->unk46 != sub1->unk44) {
            sub2->unk40 = 1;
            return;
        }
match_DE0:
        sub2->unk32 = target->unk84 >> 1;
        if (sub2->unk30 < (s16)sub2->unk32) {
            sub2->unk32 = (u16)sub2->unk30;
        }
        sub2->unk40 = 300;
        if (sub1->unk40 > 0) {
            sub1->unk40 = 1;
        }
        return;
    }

    if (s0 > 0xF5FF) {
        v0 = a2 < 0x4000;
        if (v0) {
            goto label_E64;
        }
    }
    sub2->unk32 = (self->unk84 >> 1) + ((s16)sub2->unk30 >> 2);
    goto set_40;

label_E64:
    if (self->unk16 == 26) {
        sub2->unk32 = (s16)sub2->unk30 >> 2;
    } else {
        sub2->unk32 = 0;
    }
set_40:
    sub2->unk40 = 300;
}
