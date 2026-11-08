#include "game/types.h"

struct Entity {
    struct Entity *next;
    s32 unk04;
    s32 id;
    u8 pad0C[0x0A];
    u8 unk16;
    u8 pad17[0x0A];
    u8 unk21;
    s8 unk22;
    u8 pad23;
    s32 flags;
    u8 pad28[0x28];
    s32 x;
    u8 pad54[4];
    s32 z;
};

extern struct Entity *D_8009FA58;
extern struct Entity *D_8009FA5C;
extern struct Entity *D_8009F9A0;
extern s32 D_8009FAA4;
extern s32 D_8009F7A4;
extern struct Entity *D_8009FA40;
extern s32 D_800A8F90;
extern s32 D_800A8F94;
extern struct Entity *D_800A8F98[];
extern s32 func_80045678(s32, s32, s32, s32, s32);
extern void func_8004572C(s32, s32, s32, s32, s32);
extern void func_800457B8(s32, s32);
extern void func_8004DEE0(struct Entity *);

void func_8004E248(void)
{
    s32 flag;
    s32 x, z;
    register struct Entity *e __asm__("$16");
    register s32 limit __asm__("$17");
    register s32 count __asm__("$18");
    struct Entity **out;
    register s32 mask __asm__("$20");
    register s32 bz __asm__("$21");
    register s32 bx __asm__("$22");
    register s32 best __asm__("$23");
    struct Entity *bestEnt;
    register s32 ex __asm__("$5");
    register s32 ez __asm__("$8");
    register s32 d __asm__("$7");
    register s32 dz __asm__("$4");
    register s32 m __asm__("$6");
    register s32 v0 __asm__("$2");
    register s32 v1 __asm__("$3");

    x = D_8009FA58->x;
    bestEnt = 0;
    z = D_8009FA58->z;
    bx = D_8009FA5C->x;
    bz = D_8009FA5C->z;
    best = 0x40000000;
    if (D_8009F9A0 != 0) {
        bx = D_8009F9A0->x;
        bz = D_8009F9A0->z;
        x = bx;
        z = bz;
    }
    flag = 0;
    limit = 0x38000;
outer_head:
    if (D_8009F7A4 <= D_8009FAA4) {
        flag = 1;
        D_8009F7A4 = D_8009FAA4 + 0x12C;
    }
    out = D_800A8F98;
    count = 0;
    e = D_8009FA40;
    if (e != 0) {
        mask = 0x400000;
        do {
            if (e->id != 0) {
                ex = e->x;
                ez = e->z;
                d = ex - x;
                v1 = d >> 31;
                v0 = v1 + d;
                d = v0 ^ v1;
                dz = ez - z;
                v1 = dz >> 31;
                v0 = v1 + dz;
                dz = v0 ^ v1;
                m = d;
                if (dz < d) m = dz;
                d = d + dz;
                v0 = m / 2;
                d = d - v0;
                if (d > 0x21FFF) {
                    ex = ex - bx;
                    v1 = ex >> 31;
                    v0 = v1 + ex;
                    ex = v0 ^ v1;
                    dz = ez - bz;
                    v1 = dz >> 31;
                    v0 = v1 + dz;
                    dz = v0 ^ v1;
                    m = ex;
                    if (dz < ex) m = dz;
                    ex = ex + dz;
                    v0 = m / 2;
                    ex = ex - v0;
                    if (ex < d) d = ex;
                }
                {
                    s32 flags = e->flags;
                    s32 nf;
                    if ((flags & 8) == 0 && d >= limit) goto inactive;
                    nf = (flags | 0x20000000) & 0xFDFFFFFF;
                    e->flags = nf;
                    if (d <= 0x21FFF) {
                        if (flag != 0 && (nf & 0x100000) != 0 && e->unk16 != 0x24 && d < best) {
                            best = d;
                            bestEnt = e;
                        }
                        if (e->unk21 != 0) {
                            if ((e->flags & mask) == 0) {
                                s8 idx = func_80045678(e->unk21, e->id, e->x, e->z, -1);
                                e->unk22 = idx;
                                if (idx >= 0) e->flags |= mask;
                            } else {
                                func_8004572C(e->id, e->x, e->z, -1, e->unk22);
                            }
                        }
                    } else {
                        if (nf & mask) {
                            func_800457B8(e->id, e->unk22);
                            e->flags &= 0xFFBFFFFF;
                        }
                    }
                    v0 = 0x800B0000;
                    __asm__("" : "+r"(v0));
                    if (count >= *(s32 *)(v0 - 0x706C)) {
                        limit -= 0x4000;
                        if (limit > 0x20000) goto outer_head;
                        goto done;
                    }
                    count++;
                    *out++ = e;
                    goto next;
                inactive:
                    v0 = flags & 0xDFFFFFFF;
                    e->flags = v0;
                    __asm__("" : "+r"(v0) : : "memory");
                    if (v0 & mask) {
                        func_800457B8(e->id, e->unk22);
                        e->flags &= 0xFFBFFFFF;
                    }
                }
            next:;
            }
        } while ((e = e->next) != 0);
    }
done:
    v0 = 0x800B0000;
    __asm__("" : "+r"(v0));
    *(s32 *)(v0 - 0x7070) = count;
    if (bestEnt != 0) func_8004DEE0(bestEnt);
}
