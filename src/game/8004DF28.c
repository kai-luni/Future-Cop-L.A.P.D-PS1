#include "game/types.h"

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Pos {
    u8 pad00[0x50];
    s32 unk50;
    u32 pad54;
    s32 unk58;
};

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 pad0C[0x0A];
    u8 unk16;
    u8 pad17[0x0A];
    u8 unk21;
    s8 unk22;
    u8 unk23;
    s32 flags;
    u8 pad28[0x28];
    s32 unk50;
    u32 pad54;
    s32 unk58;
};

extern struct Entity *D_8009FA40;
extern s32 D_8009F7A4;
extern s32 D_8009FAA4[];
extern struct Pos *D_8009FA58[];
extern struct Pos *D_8009F9A0[];
extern struct Entity *D_800A8F98[];
extern s32 D_800A8F90[];
extern s32 D_800A8F94[];

extern s32 func_80045678(s32 sound, s32 id, s32 arg2, s32 arg3, s32 arg4);
extern void func_8004572C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 index);
extern void func_800457B8(s32 arg0, s32 index);
extern void func_8004DEE0(struct Entity *entity);

void func_8004DF28(void)
{
    register struct Entity *node __asm__("$16");
    register struct Entity *best __asm__("$22");
    register struct Entity **out __asm__("$19");
    register char *outHigh __asm__("$2");
    s32 saved50;
    s32 saved58;
    register s32 threshold __asm__("$17");
    register s32 count __asm__("$18");
    register s32 flag __asm__("$23");
    register s32 bestDist __asm__("$21");
    register s32 mask __asm__("$20");
    register s32 adx __asm__("$5");
    register s32 adz __asm__("$4");
    register s32 mn __asm__("$6");
    register s32 dist __asm__("$5");
    register s32 temp __asm__("$2");
    register s32 sign __asm__("$3");
    s32 res;

    register struct Pos *a __asm__("$2");
    register struct Pos *b __asm__("$3");

    best = 0;
    bestDist = 0x40000000;
    a = D_8009FA58[0];
    b = D_8009F9A0[0];
    saved50 = a->unk50;
    saved58 = a->unk58;
    if (b != 0) {
        saved50 = b->unk50;
        saved58 = b->unk58;
    }
    flag = 0;
    threshold = 0x38000;

    for (;;) {
        if (D_8009F7A4 > D_8009FAA4[0]) {
            goto skip;
        }
        flag = 1;
        D_8009F7A4 = D_8009FAA4[0] + 0x12C;
    skip:
        outHigh = (char *)0x800B0000;
        __asm__("" : "=r"(outHigh) : "0"(outHigh));
        out = (struct Entity **)(outHigh - 0x7068);
        node = D_8009FA40;
        count = 0;
        if (node != 0) {
        mask = 0x400000;
        do {
            if (node->id != 0) {
                temp = node->unk50;
                __asm__("" : "=r"(temp) : "0"(temp));
                adx = temp - saved50;
                sign = adx >> 31;
                temp = sign + adx;
                __asm__("" : "=r"(temp) : "0"(temp));
                adx = temp ^ sign;
                __asm__("" : "=r"(adx) : "0"(adx));
                temp = node->unk58;
                __asm__("" : "=r"(temp) : "0"(temp));
                adz = temp - saved58;
                sign = adz >> 31;
                temp = sign + adz;
                __asm__("" : "=r"(temp) : "0"(temp));
                adz = temp ^ sign;
                mn = adx;
                if (adz < adx) {
                    mn = adz;
                }
                dist = adx + adz;
                temp = mn / 2;
                __asm__("" : "=r"(temp) : "0"(temp));
                dist -= temp;
                if ((node->flags & 8) || (dist < threshold)) {
                    node->flags = (node->flags | 0x20000000) & 0xFDFFFFFF;
                    if (dist <= 0x21FFF) {
                        if (flag && (node->flags & 0x100000) && node->unk16 != 0x24 &&
                            dist < bestDist) {
                            bestDist = dist;
                            best = node;
                        }
                        if (*(volatile u8 *)&node->unk21 != 0) {
                            if (!(node->flags & mask)) {
                                res = func_80045678(node->unk21, node->id, node->unk50,
                                                    node->unk58, -1);
                                node->unk22 = res;
                                if ((s8)res >= 0) {
                                    node->flags |= mask;
                                }
                            } else {
                                func_8004572C(node->id, node->unk50, node->unk58, -1,
                                              node->unk22);
                            }
                        }
                    } else {
                        if (node->flags & mask) {
                            func_800457B8(node->id, node->unk22);
                            node->flags &= 0xFFBFFFFF;
                        }
                    }
                    temp = 0x800B0000;
                    __asm__("" : "=r"(temp) : "0"(temp));
                    if (count < *(s32 *)(temp - 0x706C)) {
                        goto add;
                    }
                    threshold -= 0x4000;
                    if (threshold > 0x20000) {
                        goto again;
                    }
                    goto done;
                add:
                    count++;
                    *out++ = node;
                } else {
                    node->flags &= 0xDFFFFFFF;
                    if (node->flags & mask) {
                        func_800457B8(node->id, node->unk22);
                        node->flags &= 0xFFBFFFFF;
                    }
                }
            }
            node = node->next;
        } while (node != 0);
        }
        break;
    again:
        ;
    }
done:
    temp = 0x800B0000;
    __asm__("" : "=r"(temp) : "0"(temp));
    *(s32 *)(temp - 0x7070) = count;
    if (best != 0) {
        func_8004DEE0(best);
    }
}
