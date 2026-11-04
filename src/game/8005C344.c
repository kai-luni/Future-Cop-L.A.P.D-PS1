#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Seg {
    struct Seg *next;
    struct Vec3 pos;
    s16 radius;
    u8 unk12[6];
    u16 flags;
};

struct Tag {
    u8 unk00[4];
    u8 type;
};

struct Owner {
    u8 unk00[0x20];
    struct Tag *unk20;
    u8 unk24[0x44];
    struct Seg *segs;
    u8 unk6C[0x24];
    s16 step;
};

struct Msg {
    void *unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    struct Vec3 vec;
    u8 unk1C[0x2D];
    u8 unk49;
    u8 unk4A[6];
};

struct Target;

struct Cand {
    u8 unk00[4];
    s32 (*handler)(s32 id, struct Cand *self, void *arg, s32 arg3);
    s32 unk08;
    u8 unk0C[8];
    u16 unk14;
    u8 unk16[0x46];
    struct Vec3 pos;
    u8 unk68[6];
    s16 radius;
};

struct Res {
    u8 unk00[0x10];
    u8 flags;
};

struct Tgt {
    u8 unk00[0x14];
    u16 unk14;
    u8 kind;
    u8 unk17[0x95];
    struct Tgtx *unkAC;
    u8 unkB0[4];
    struct Tgt *unkB4;
};

struct Tgtx {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Msg2 {
    s32 a;
    u16 b;
    u8 c;
    u8 pad;
    void *d;
};

struct Target {
    u8 unk00[8];
    struct Tgt *unk08;
    u8 unk0C[0xC];
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F[9];
    u8 flags;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    struct Vec3 pos;
};

struct CandList {
    s32 count;
    s32 unk04;
    struct Cand *items[1];
};

extern struct CandList D_800A8F90;
extern struct Tgt *D_8009F374;

#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))
extern s32 func_800684AC(struct Vec3 *a, struct Vec3 *b, struct Vec3 *out, s32 c, s32 d);
extern s32 func_80019FCC(struct Vec3 *v);
extern s32 func_80095900(s32 v);
extern void func_8005C184(struct Vec3 *v, s32 a, struct Target *t);
extern void func_8001A180(struct Vec3 *v);
extern s32 func_8001AB00(struct Vec3 *a, struct Vec3 *b);
extern u8 func_8005C2C8(struct Target *t, struct Cand *c);
extern s32 func_8003BF50(struct Tgt *t);
extern struct Res *func_800402DC(void *owner, u8 column, struct Msg *msg, s32 row, s32 arg4);

#define FMUL(d, a, b) do { d = (a); __asm__("mult %0,%1\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "+r"(d) : "r"(b) : "$8", "$9"); } while (0)

#define ASM(d, b) __asm__("mult %0,%1\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "+r"(d) : "r"(b) : "$8", "$9")

void func_8005C344(struct Owner *arg0, struct Target *tgt)
{
    struct Msg msg;
    struct Vec3 dir;
    struct Vec3 diff;
    union {
        struct Vec3 v;
        struct Msg2 m;
    } t;
    struct Seg *head;
    struct Seg *tail;
    struct Seg *p;
    struct Cand *cand;
    struct Res *res;
    struct Tgt *kind;
    struct Tgtx *x;
    s32 s1;
    s32 s4;
    s32 i;
    s32 f;
    s32 lim;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 ax;
    s32 ay;
    s32 az;
    s32 bx;
    struct Owner *owner;

    if (arg0->unk20->type != 0x57) {
        return;
    }
    owner = arg0;
    head = tail = owner->segs;
    if (head == 0) {
        return;
    }
    while (tail->next != 0) {
        tail = tail->next;
    }
    if (head->next != 0) {
        if (func_800684AC(&tgt->pos, &tail->pos, &t.v, 0, 1) != 0) {
            bx = tgt->pos.x;
            ay = t.v.y;
            msg.vec.y = ay - tgt->pos.y;
            ax = t.v.x;
            az = t.v.z;
            msg.vec.z = az - tgt->pos.z;
            msg.vec.x = ax - bx;
            s1 = (func_80095900(func_80019FCC(&msg.vec)) << 2) >> 4;
            dx = head->pos.x - tgt->pos.x;
            msg.vec.y = head->pos.y - tgt->pos.y;
            msg.vec.z = head->pos.z - tgt->pos.z;
            msg.vec.x = dx;
            s1 -= (func_80095900(func_80019FCC(&msg.vec)) << 2) >> 4;
            if (s1 < 0) {
                if (tgt->flags & 2) {
                    head->flags |= 1;
                    return;
                }
                head->pos = t.v;
                head->next->flags |= 1;
                func_8005C184(&t.v, head->radius << 4, tgt);
                return;
            }
            p = owner->segs;
            while (p != 0) {
                if (s1 < 0) {
                    break;
                }
                s1 -= owner->step;
                p = p->next;
            }
            if (p != 0) {
                tail = p;
                if (tail->next != 0) {
                    tail->next->flags |= 1;
                    func_8005C184(&t.v, head->radius << 4, tgt);
                }
                tail->pos = t.v;
            }
        }
    } else {
        if (func_800684AC(&tgt->pos, &head->pos, &t.v, 0, 1) != 0) {
            if (tgt->flags & 2) {
                head->flags |= 1;
            } else {
                head->pos = t.v;
            }
            func_8005C184(&t.v, head->radius << 4, tgt);
        }
    }
    if (tgt->flags & 8) {
        return;
    }
    if (head == tail) {
        return;
    }
    for (i = 0; i < D_800A8F90.count; i++) {
        cand = D_800A8F90.items[i];
        if (tgt->unk08->unk14 == cand->unk14) {
            continue;
        }
        if (cand->unk08 <= 0) {
            continue;
        }
        s4 = cand->radius + tail->radius;
        dir.x = tail->pos.x - head->pos.x;
        dir.y = tail->pos.y - head->pos.y;
        dir.z = tail->pos.z - head->pos.z;
        s4 = (s4 * s4) >> 8;
        lim = 0x14000;
        if (ABS(dir.x) < lim && ABS(dir.y) < lim && ABS(dir.z) < lim) {
        func_8001A180(&dir);
        diff.x = cand->pos.x - tail->pos.x;
        diff.y = cand->pos.y - tail->pos.y;
        diff.z = cand->pos.z - tail->pos.z;
        s1 = func_8001AB00(&diff, &dir);
        if (s1 < 0) {
            goto neg;
        }
        msg.vec = tail->pos;
    again:
        if (func_80019FCC(&diff) < s4) {
            if (func_8005C2C8(tgt, cand) == 0) {
                t.m.c = tgt->unk2B;
                t.m.b = tgt->unk1C;
                t.m.a = tgt->unk08->unk14;
                t.m.d = tgt->unk08;
                cand->handler(0xE, cand, &t.m, 0);
                msg.unk49 = 5;
                msg.unk00 = (void *)(tail->radius << 4);
                D_8009F374 = tgt->unk08;
                f = func_8003BF50(tgt->unk08);
                res = func_800402DC(0, tgt->unk1E, &msg, f, 0);
                if (res != 0) {
                    kind = tgt->unk08;
                    if (kind != 0) {
                        if (kind->kind == 1) {
                            x = kind->unkAC;
                            if (x != 0) {
                                if (x->unkC1 == 0) {
                                    res->flags |= 0x8;
                                } else {
                                    res->flags |= 0x10;
                                }
                            }
                        } else if (kind->kind == 0x12) {
                            x = kind->unkB4->unkAC;
                            if (x != 0) {
                                if (x->unkC1 == 0) {
                                    res->flags |= 0x8;
                                } else {
                                    res->flags |= 0x10;
                                }
                            }
                        }
                    }
                }
                D_8009F374 = 0;
            }
        }
        goto cont;
    neg:
            t.v.x = cand->pos.x - head->pos.x;
            t.v.y = cand->pos.y - head->pos.y;
            t.v.z = cand->pos.z - head->pos.z;
            if (func_8001AB00(&t.v, &dir) <= 0) {
                goto cont;
            }
            {
                s32 m;
                s32 px;
                s32 py;
                s32 pz;
                m = dir.x;
                px = tail->pos.x;
                ASM(m, s1);
                px += m;
                msg.vec.x = px;
                m = dir.y;
                py = tail->pos.y;
                ASM(m, s1);
                py += m;
                msg.vec.y = py;
                m = dir.z;
                pz = tail->pos.z;
                ASM(m, s1);
                pz += m;
                msg.vec.z = pz;
                diff.x = cand->pos.x - px;
                diff.y = cand->pos.y - py;
                diff.z = cand->pos.z - pz;
            }
            goto again;
        }
    cont:;
    }
}

struct Mid {
    u8 unk00[0x48];
    struct Target **unk48;
};

struct Obj2 {
    u8 unk00[0xF];
    u8 unkF;
    u8 unk10;
    u8 unk11[0xB];
    struct Mid *unk1C;
    struct Tag *unk20;
    s32 (*handler)(s32 id, struct Obj2 *self, s32 a, s32 b);
    u8 unk28[0x66];
    u16 unk8E;
};
