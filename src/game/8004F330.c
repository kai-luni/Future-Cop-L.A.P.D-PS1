// Categories: Gameplay > Path Movement | AI > Target Selection
#include "game/types.h"

struct Node4F330 {
    u32 word0;
    u16 word4;
    s16 unk6;
    u8 pad8[2];
    s16 unkA;
};
struct Ref4F330 {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x44];
    s32 unk50;
    u8 pad54[4];
    s32 unk58;
};
struct Obj4F330 {
    u8 *unk00;
    u8 pad04[0x2A];
    s16 unk2E;
    u8 pad30[0xA];
    s16 unk3A;
    u32 unk3C;
    u8 pad40[2];
    s16 unk42;
    u8 pad44[4];
    s16 unk48;
    s16 unk4A;
    u8 pad4C[0x10];
    struct Ref4F330 *unk5C;
    s32 unk60;
};
extern void *jtbl_800119A8[];
extern s32 func_80019668(void);
extern void gCheckTargetRangeAndCone(void *, void *, s32, s32) __asm__("func_8004E5F0");
extern struct Ref4F330 *gFindTargetEntity(void *, s32, s32, s32, s32, s32, s32) __asm__("func_8004E80C");
extern s32 gGteLeadingZeros(u32) __asm__("func_80095C14");
s32 gSelectNextPathNode(struct Obj4F330 *arg0, struct Node4F330 *arg1, s32 arg2) __asm__("func_8004F330");


/**
 * @brief Selects the next connected map node for a path follower using target-directed or random scoring.
 *
 * Unpacks four packed neighbor indices from 12-byte node arg2 in arg0->unk00 + 4, validates or acquires a target for modes 1..5 and 9..13, scores candidates via jtbl_800119A8, and picks the highest-scoring neighbor.
 *
 * @param arg0 Path state (+0x00 map, +0x2E range, +0x3A random skip, +0x3C/+0x3F flags/mode, +0x42 target node/val, +0x48/+0x4A recent nodes, +0x5C/+0x60 target cache).
 * @param arg1 Seeker object passed to func_8004E5F0 and func_8004E80C.
 * @param arg2 Current map node index.
 * @return Winning neighbor index, -1 if only blocked/unk48 candidates exist, or -2 if all score -4.
 *
 * @par Globals
 * None (dispatches scoring modes via jump table jtbl_800119A8).
 *
 * @par Control flow
 * Low nibble of +0x3F sets mode (reverts to 0 on PRNG < +0x3A or failed target lookup). Loops over 4 unpacked neighbors (three 10-bit fields in +0, one >> 6 in +4): rejects 0x3FF (-4), +0x48 (-2) and +0x4A (-1) unless +0x3C & 1, and node byte +0 & 2 (-3). Modes 1..5/9..13 seek/flee target +0x5C and modes 6/14 aim at node +0x42 via 2D dot product scaled with func_80095C14 and >> (+4 & 0xF). Picks max >= -1 backwards with random tie-breaking.
 *
 * @par Calls
 * func_80019668 (random skip, mode-0 scores, tie-break), func_8004E5F0 (called on matching cached id before clearing unk5C), func_8004E80C (target acquisition), func_80095C14 (GTE leading-bit scaling of dot products).
 *
 * @par Side effects
 * Clears or updates arg0->unk5C and arg0->unk60 during target lookup.
 *
 * @par Callers
 * Confirmed: func_8004F764 (0x8004F790, segment hand-off) and func_80050E0C (0x80050EDC, mover setup).
 */
s32 gSelectNextPathNode(struct Obj4F330 *arg0, struct Node4F330 *arg1, s32 arg2)
{
    static void *const label_table[] __attribute__((used)) = {
        &&case_0, &&case_ref, &&case_ref, &&case_ref, &&case_ref, &&case_ref,
        &&case_node, &&case_default, &&case_default,
        &&case_ref, &&case_ref, &&case_ref, &&case_ref, &&case_ref, &&case_node
    };
    register struct Obj4F330 *obj __asm__("$18");
    register s32 idx __asm__("$19");
    register s32 mode __asm__("$20");
    register s32 s0 __asm__("$16");
    register s32 s1 __asm__("$17");
    register u8 b __asm__("$2");
    s32 arr[8];
    register s32 *arrBase __asm__("$21");
    s32 best;
    s32 bi;
    register s32 value __asm__("$4");
    s32 dx;
    s32 dy;

    obj = arg0;
    b = *((u8 *)obj + 0x3F);
    mode = b & 0xF;
    s1 = (s32)arg1;
    idx = arg2;
    if (mode != 0) {
        if (obj->unk3A > 0 && (func_80019668() & 0xFFF) < obj->unk3A) {
            goto disable;
        }
        s0 = mode;
        if (mode >= 9) {
            s0 = mode - 8;
        }
        if (s0 == 6) {
            goto common;
        }
        if (obj->unk5C != 0) {
            if (obj->unk60 == obj->unk5C->unk08) {
                gCheckTargetRangeAndCone((void *)s1, obj->unk5C, obj->unk2E << 4, 0xFFFF0000);
                obj->unk5C = 0;
            }
            if (obj->unk5C != 0) {
                goto common;
            }
        }
        obj->unk5C = gFindTargetEntity((void *)s1, s0, obj->unk42, 1,
                                  obj->unk2E << 4, 0xFFFF0000, 0);
        if (obj->unk5C != 0) {
            obj->unk60 = obj->unk5C->unk08;
            goto common;
        }
    disable:
        mode = 0;
    }
common:
    {
        register s32 offset __asm__("$2") = idx * 12;
        register s32 base __asm__("$3");
        __asm__ volatile("lw\t%0,0(%2)\n\taddiu\t%1,%1,4"
                         : "=r"(base), "+r"(offset) : "r"(obj) : "memory");
        s1 = base + offset;
    }
    {
        u32 w = ((struct Node4F330 *)s1)->word0;
        arr[0] = w >> 22;
        arr[1] = (w >> 12) & 0x3FF;
        arr[2] = (w >> 2) & 0x3FF;
    }
    {
        register u32 w4 __asm__("$2") = ((struct Node4F330 *)s1)->word4;
        __asm__ volatile("addiu %0, $sp, 0x20" : "=r"(arrBase));
        arr[3] = w4 >> 6;
    }
    {
        register s32 base __asm__("$2") = (s32)obj->unk00;
        __asm__ volatile("move %0, $21" : "=r"(s0));
        __asm__("" : "+r"(base));
        idx = base + 4;
    }
    do {
        if (*(s32 *)s0 == 0x3FF) {
            ((s32 *)s0)[4] = -4;
            goto next;
        }
        if (*(s32 *)s0 == obj->unk48 && !(obj->unk3C & 1)) {
            ((s32 *)s0)[4] = -2;
            goto next;
        }
        if (*(s32 *)s0 == obj->unk4A && !(obj->unk3C & 1)) {
            ((s32 *)s0)[4] = -1;
            goto next;
        }
        if (((u8 *)(idx + *(s32 *)s0 * 12))[0] & 2) {
            ((s32 *)s0)[4] = -3;
            goto next;
        }
        if ((u32)mode >= 15) {
            goto case_default;
        }
        goto *jtbl_800119A8[mode];
        case_0:
            value = func_80019668() & 0xFFFF;
            goto shift;
        case_node:
            {
                struct Node4F330 *n = (struct Node4F330 *)(idx + obj->unk42 * 12);
                dx = n->unk6 - ((struct Node4F330 *)s1)->unk6;
                dy = n->unkA - ((struct Node4F330 *)s1)->unkA;
            }
            goto shared;
        case_ref:
            {
                struct Ref4F330 *ref = obj->unk5C;
                dx = (ref->unk50 >> 8) - ((struct Node4F330 *)s1)->unk6;
                dy = (ref->unk58 >> 8) - ((struct Node4F330 *)s1)->unkA;
            }
        shared:
            {
                register struct Node4F330 *n __asm__("$2");
                register s32 x __asm__("$4");
                register s32 product __asm__("$3");
                register s32 ydiff __asm__("$2");
                register s32 y __asm__("$3");
                x = ((struct Node4F330 *)s1)->unk6;
                n = (struct Node4F330 *)(idx + *(s32 *)s0 * 12);
                __asm__("" : "+r"(n), "+r"(x));
                value = dx * (n->unk6 - x);
                ydiff = n->unkA;
                y = ((struct Node4F330 *)s1)->unkA;
                __asm__("" : "+r"(ydiff), "+r"(y));
                ydiff -= y;
                __asm__("" : "+r"(ydiff));
                product = dy * ydiff;
                __asm__("" : "+r"(product));
                value += product;
            }
            if ((u32)(mode - 9) < 5) {
                value = -value;
            }
            if (value < 0) {
                value = gGteLeadingZeros(-value);
            } else if (value != 0) {
                s32 count;
                register s32 limit __asm__("$3");
                count = gGteLeadingZeros(value);
                limit = 0x3F;
                __asm__("" : "+r"(limit));
                value = limit - count;
            } else {
                value = 0x20;
            }
            goto shift;
        case_default:
            value = -4;
        shift:
        {
            s32 v = ((u8 *)(idx + *(s32 *)s0 * 12))[4] & 0xF;
            if (v > 0 && value > 0) {
                value >>= v;
            }
        }
        ((s32 *)s0)[4] = value;
    next:
        s0 += 4;
    } while (s0 < (s32)arrBase + 16);
    obj = (struct Obj4F330 *)-4;
    idx = -1;
    for (s1 = 3, s0 = (s32)(arr + 3); s1 >= 0; s1--, s0 -= 4) {
        s32 v = *(s32 *)(s0 + 16);
        if (v >= -1 && v >= (s32)obj && (v != (s32)obj || !(func_80019668() & 1))) {
            obj = (struct Obj4F330 *)*(s32 *)(s0 + 16);
            idx = s1;
        }
    }
    if (idx < 0) {
        return (s32)obj == -4 ? -2 : -1;
    }
    {
        register s32 offset __asm__("$2") = idx * 4;
        __asm__("" : "+r"(offset));
        return *(s32 *)((s32)arr + offset);
    }
}
