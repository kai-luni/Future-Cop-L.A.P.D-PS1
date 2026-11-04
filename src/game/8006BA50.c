#include "game/types.h"

struct Unk6BA50Node {
    struct Unk6BA50Node *next;
    u8 unk04[4];
    s32 unk08;
    u8 unk0C[8];
    u16 unk14;
    u8 unk16;
    u8 unk17[5];
    s16 unk1C;
    u8 unk1E[0xA6];
    s32 unkC4;
};

struct Unk6BA50Ctl {
    u8 unk00[0x78];
    s32 unk78[3];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
    u8 unk93[3];
    u8 unk96[2];
    struct Unk6BA50Node *unk98;
    s32 unk9C;
    u8 unkA0[0x21];
    s8 unkC1;
};

struct Unk6BA50Obj {
    u8 unk00[0xAC];
    struct Unk6BA50Ctl *ctl;
};

struct Unk6BA50Entry {
    u8 type;
    u8 unk01[3];
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    s16 unk08;
    s16 unk0A;
    u8 unk0C[0x13];
    u8 unk1F;
};

struct Unk6BA50Tgt {
    u8 unk00[0xA];
    u8 unk0A;
    u8 unk0B[0x39];
    s32 unk44;
    u8 unk48[8];
    s32 unk50;
};

extern struct Unk6BA50Node *D_8009FA40;
extern u8 D_8009FA8C[];

extern s32 func_80046C84(s32 id, struct Unk6BA50Obj *obj);
extern struct Unk6BA50Tgt *func_8006AAEC(u8 id);
extern s32 func_8006ACB0(struct Unk6BA50Obj *obj, s32 idx);
extern void func_8006B23C(struct Unk6BA50Obj *obj, s32 idx, s32 arg2);

#define S16(off) (*(s16 *)((u32)ctl + (idx << 1) + (off)))

void func_8006BA50(struct Unk6BA50Obj *obj, s32 idx)
{
    struct Unk6BA50Ctl *ctl;
    struct Unk6BA50Node *n;
    struct Unk6BA50Tgt *tgt;
    struct Unk6BA50Entry *e;
    s32 val;
    u8 flag;
    s32 ty;
    register s32 t __asm__("$2");
    register s32 t2 __asm__("$3");
    register s32 c __asm__("$2");
    u8 ok;

    ok = 1;
    if ((u32)idx < 3) {
        ctl = obj->ctl;
        if (idx == 2) {
            n = ctl->unk98;
            if (n != 0) {
                if (ctl->unk9C != n->unk08) {
                    ctl->unk98 = 0;
                } else {
                    struct Unk6BA50Node *m = D_8009FA40;
                    u32 a = n->unk14;
                    u32 b = n->unk16;
                    for (; m != 0; m = m->next) {
                        if (m->unk08 != 0 && m->unk14 == a && m->unk16 == b) {
                            if (m->unk16 != 7) {
                                if (m->unk16 == 0xF) {
                                    m->unkC4 = 0;
                                } else {
                                    m->unk1C = -1;
                                }
                            } else {
                                m->unk1C = -1;
                            }
                        }
                    }
                    ctl->unk98 = 0;
                    return;
                }
            }
        }
        if (ctl->unk78[idx] > 0) {
            return;
        }
        val = S16(0x8A);
        flag = val > 0;
        if (!flag) {
            val = ctl->unk84[idx];
            if (val <= 0) {
                func_80046C84(0xD, obj);
                return;
            }
        }
        if (val == 5) {
            func_80046C84(0xE, obj);
        }
        tgt = func_8006AAEC(ctl->unk90[idx]);
        e = (struct Unk6BA50Entry *)((u8 *)tgt + (flag ? 0x24 : 0x4));
        ty = e->type;
        if (ty >= 10 && (ty < 13 || ty == 14)) {
            func_8006B23C(obj, idx, e->unk1F);
        } else {
            ok = func_8006ACB0(obj, idx);
        }
        if (ok) {
            c = ctl->unkC1;
            __asm__ volatile("" : "+r"(c));
            if (D_8009FA8C[c] == 0) {
                val = val - 1;
            } else {
                val = val ^ 1;
            }
            if (flag) {
                S16(0x8A) = val;
            } else {
                ctl->unk84[idx] = val;
            }
            if (--ctl->unk93[idx] == 0) {
                ctl->unk93[idx] = e->unk06;
                ctl->unk78[idx] += e->unk08;
            } else {
                ctl->unk78[idx] += e->unk0A;
            }
            if (flag && val <= 0) {
                S16(0x8A) = 0;
                ctl->unk93[idx] = tgt->unk0A;
                t = tgt->unk50;
                t2 = tgt->unk44;
                tgt->unk44 = t;
                tgt->unk50 = t2;
            }
        }
    }
}
