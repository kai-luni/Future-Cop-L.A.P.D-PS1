#include "game/types.h"

struct Mdl {
    u8 pad00[0xC];
    u16 frames;
    u8 pad0E[0x26];
    void *unk34;
};

struct Obj {
    struct Mdl *model;
    s32 unk04;
    s32 pad08;
    s32 unk0C;
    u8 pad10[0xC];
    s32 unk1C;
    u8 pad20[4];
    s32 unk24;
    u8 pad28[0xC];
    u8 unk34[0x14];
    s16 unk48;
    u8 pad4A[2];
    u8 flags;
    u8 unk4D;
    u16 unk4E;
    s16 unk50;
    s16 unk52;
    u8 pad54[2];
    s8 unk56;
    u8 unk57;
    struct Obj *next;
};

struct Slot {
    struct Obj *head;
    s32 unk04;
};

extern struct Slot *D_8009F938;
extern s32 D_8009F93C;
extern s32 D_8009FAA4;

extern s32 func_800196C4(void);
extern void func_800175F8(struct Obj *o, s32 frame);
extern void func_8001846C(s32 angle, s32 *sine, s32 *cosine);
extern void func_80010750(void *p);
extern void func_80010234(void *a, void *b, s32 c);
extern void func_80017508(struct Obj *o, u32 *flags, s32 arg);
extern void func_8008363C(struct Obj *o);

void func_80084398(s32 x, s32 y, u32 *p)
{
    u32 local;
    u32 *q;
    s32 a;
    s32 b;
    s32 idx;
    u32 mask;
    register struct Obj *o __asm__("$16");
    s32 f;
    s32 c;
    s32 n;
    s32 s;
    s32 t;
    s32 d;
    s32 e;
    s32 m;
    s32 v;
    u8 w;
    s32 sn;
    s32 cs;
    s32 h;
    s32 k;
    s32 t2;

    if (D_8009F938 == 0) {
        return;
    }
    q = p;
    local = 0;
    for (b = 0; b < 5; b++) {
        for (a = 0; a < 5; a++) {
            mask = *q;
            q += 4;
            if (mask == 0) {
                continue;
            }
            idx = x + a + D_8009F93C * (y + b);
            o = D_8009F938[idx].head;
            if (o == 0) {
                continue;
            }
            do {
                if (!(o->flags & 0x10)) {
                    t = D_8009FAA4;
                    D_8009F938[idx].unk04 = t;
                    if (mask & o->unk4E) {
                        if (!(o->flags & 8) && o->unk4D != 0) {
                            if (o->flags & 0x20) {
                                s = o->unk48;
                                w = o->unk4D - (*(u8 *)&D_8009FAA4 & 7);
                                o->unk4D = w;
                                if ((u8)(w - 1) < 0xF0) {
                                } else {
                                    if (s == 1) {
                                        s = 0;
                                        w = func_800196C4() & 0x7F;
                                    } else {
                                        s = 1;
                                        w = func_800196C4() & 0xF;
                                    }
                                    o->unk4D = w + 1;
                                }
                            } else if (o->flags & 4) {
                                n = o->model->frames;
                                s = ((t % ((o->unk4D << 1) * (n - 1))) / o->unk4D);
                                if (s >= n) {
                                    k = n - 2;
                                    s = k - (s - n);
                                }
                            } else {
                                s = ((u32)t % (o->unk4D * o->model->frames)) / o->unk4D;
                            }
                            if (s != o->unk48) {
                                func_800175F8(o, s);
                            }
                        }
                        d = o->unk56;
                        if (d != 0) {
                            if (d >= 0) {
                                e = ((D_8009FAA4 & 0xFFF) * d) & 0xFFF;
                            } else {
                                e = ~((D_8009FAA4 & 0xFFF) * -d) & 0xFFF;
                            }
                            if (o->flags & 0x80) {
                                m = o->unk52;
                                if (m != 0) {
                                    h = e << 1;
                                    e = (m * h) / 4096;
                                    if (m < e) {
                                        e = e - m;
                                        if ((m >> 1) < e) {
                                            e = m - e;
                                        }
                                        e = e << 1;
                                    } else {
                                        if ((m >> 1) < e) {
                                            e = m - e;
                                        }
                                        e = -(e << 1);
                                    }
                                    e = e + o->unk50;
                                }
                            }
                            if (e != 0) {
                                func_8001846C(e, &sn, &cs);
                                o->unk1C = sn;
                                t2 = -sn;
                                v = cs;
                            } else {
                                v = 0x10000;
                                o->unk1C = 0;
                                t2 = 0;
                            }
                            o->unk24 = v;
                            o->unk04 = v;
                            o->unk0C = t2;
                            if (o->unk57) {
                                register s32 frame __asm__("$4");
                                func_80010750(&o->unk04);
                                frame = o->unk48;
                                frame <<= 4;
                                __asm__("" : "+r"(frame));
                                frame += 8;
                                func_80010234((u8 *)o->model->unk34 + frame,
                                              &o->unk34, 1);
                            }
                        }
                        func_80017508(o, &local, 1);
                        if (o->unk57) {
                            func_8008363C(o);
                        }
                    }
                }
                o = o->next;
            } while (o != 0);
        }
    }
}
