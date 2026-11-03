#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec6B23C;

struct Unk6B23C_Item {
    u8 id;
    u8 unk01[0x53];
};

struct Unk6B23C_Sub {
    u8 type;
    u8 unk01[0x0B];
    s16 unk0C;
    s16 unk0E;
    u8 unk10[6];
    s16 unk16;
    u8 unk18[2];
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
};

struct Unk6B23C_S1 {
    u8 unk00[4];
    u8 unk04;
    u8 unk05[0x10];
    u8 unk15;
    u8 unk16[0x08];
    u8 unk1E;
    u8 unk1F[0x15];
    s16 unk34;
    s16 unk36;
    s16 unk38;
    u8 unk3A;
    u8 unk3B;
    u8 unk3C;
    u8 unk3D;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    u8 unk46[2];
    s16 unk48;
    u16 unk4A;
};

struct Unk6B23C_B {
    u8 unk00[0x6C];
    void *unk6C;
    u8 unk70[0x14];
    u16 unk84[3];
    s16 unk8A[3];
    u8 unk90[0x26C];
    u8 unk2FC;
};

struct Unk6B23C_C {
    u8 unk00[4];
    void (*unk04)(s32, void *, void *, s32);
    u8 unk08[0x1C];
    u32 unk24;
    u8 unk28[0x28];
    u8 unk50[0x0C];
    Vec6B23C vec;
    s32 unk68;
    u8 unk6C[2];
    u16 unk6E;
};

struct Unk6B23C {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16[0x8A];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    struct Unk6B23C_B *unkAC;
};

struct Unk6B23C_Local {
    s32 unk00;
    u8 unk04[0x0C];
    Vec6B23C vec;
    u8 unk1C[0x28];
    void *unk44;
    u8 unk48;
    u8 unk49;
    u8 unk4A[3];
};

struct Unk6B23C_Callback {
    s32 unk00;
    u16 unk04;
    u8 unk06;
    u8 unk07;
    struct Unk6B23C *unk08;
};

extern s32 D_8009F380;
extern s32 D_8009F384;
extern void *D_800A7040[][0x100];

extern struct Unk6B23C_Item *func_8006AAEC(u8 id);
extern void func_80046CF0(struct Unk6B23C *arg0, u8 arg1);
extern void func_80017668(void *instance, s32 part, s32 *out);
extern s32 func_80019668(void);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern s32 func_8001AB00(s32 *a, s32 *b);
extern s32 func_800684AC(s32 *a, s32 *b, s32 *c, s32 d, s32 e);
extern void func_800410BC(void *a, s16 b, s32 c, struct Unk6B23C *d);
extern void *func_80073500(Vec6B23C *pos, s32 a1, s32 a2, s32 a3);
extern void func_8007377C(void *a0, void *a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, s32 a12, s32 a13);
extern void func_8006A524(struct Unk6B23C *a0, void *a1, Vec6B23C *a2, void *a3, void *a4);
extern void func_8005BDE4(struct Unk6B23C *a0, struct Unk6B23C *a1, struct Unk6B23C_C *a2, void *a3, s32 a4, void *a5);
extern void func_8006F0E8(s32 a0, s32 a1, s32 a2, s32 a3, Vec6B23C *a4, Vec6B23C *a5);
extern s32 func_800402DC(void *owner, u8 column, s32 arg2, s32 row, s32 arg4);

#define MUL16(out, a, b) \
    __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

struct Frame6B23C {
    Vec6B23C v38; s32 p38;
    Vec6B23C v48; s32 p48;
    Vec6B23C v58; s32 p58;
    struct Unk6B23C_Local local;
    Vec6B23C vb8; s32 pb8;
    s32 sc1[2];
    s32 sc2[2];
    struct Unk6B23C_Callback cb; s32 pcb;
};

#define v38 F.v38
#define v48 F.v48
#define v58 F.v58
#define local F.local
#define vb8 F.vb8
#define sc1 F.sc1
#define sc2 F.sc2
#define cb F.cb

void func_8006B23C(struct Unk6B23C *arg0, s32 index, s32 arg2)
{
    register struct Unk6B23C *self __asm__("$22") = arg0;
    struct Frame6B23C F;
    u8 id;
    u8 flag;
    void *instance;
    u32 angle;
    struct Unk6B23C_B *b;
    struct Unk6B23C_C *c;
    struct Unk6B23C_Item *item;
    struct Unk6B23C_Sub *s3;
    struct Unk6B23C_S1 *s1;
    s32 s0;
    s32 s5;
    s32 s7;
    s32 t;
    s32 p1, p2;
    s32 ma, mb;
    s32 x, y, z;
    s32 a0, a2, a3;

    u8 fp;
    u8 ang;
    void *node1;
    void *node2;

    b = self->unkAC;
    c = b->unk6C;
    item = func_8006AAEC(b->unk90[index]);
    flag = (((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] > 0);
    if (flag)
        s3 = (struct Unk6B23C_Sub *)((u8 *)item + 0x24);
    else
        s3 = (struct Unk6B23C_Sub *)((u8 *)item + 0x04);
    func_80046CF0(self, s3->unk1C);
    if (((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] > 0)
        s7 = (u16)((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] & 1;
    else
        s7 = ((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk84[0] & 1;

    s1 = 0;
    if (s3->type == 0x0B) {
        if (s3->unk1B != 0) {
            s1 = (struct Unk6B23C_S1 *)D_800A7040[1][s3->unk1B];
            if (s1 != 0 && s1->unk04 != 0x58)
                s1 = 0;
        }
        instance = (void *)((u8 *)b + 0x178);
        func_80017668(instance, 0, (s32 *)&v58);
    } else {
        instance = (void *)((u8 *)b + 0x178 + s7 * 0x4C);
        func_80017668(instance, 0, (s32 *)&v58);
    }

    if (s1 != 0)
        id = s1->unk15;
    else
        id = s3->unk1B;

    fp = 0;
    if (c != 0) {
        if (s1 != 0) {
            ang = s1->unk1E;
            if (ang < 4) {
                func_80017668((void *)((u8 *)c + 0x28), (s8)ang, (s32 *)&v38);
                goto after_movement;
            } else {
                t = s1->unk34;
                if (t < 0)
                    s0 = c->unk68 >> 1;
                else
                    s0 = t << 4;
            }
        } else {
            if (s3->type == 0x0C) {
                v38 = c->vec;
                goto after_movement;
            }
            t = s3->unk0E;
            if (t == 0)
                s0 = ((s16)c->unk6E) >> 1;
            else
                s0 = t;
        }

        fp = 1;
        angle = func_80019668() & 0xFFF;
        func_8001846C(angle, &sc1[0], &sc1[1]);
        angle = func_80019668() & 0xFFF;
        func_8001846C(angle, &sc2[0], &sc2[1]);

        ma = sc1[0] >> 2;
        mb = sc2[1] >> 2;
        sc1[1] >>= 2;

        p1 = (mb * ma) >> 12;
        p2 = (mb * sc1[1]) >> 12;
        MUL16(v38.x, p1, s0);
        __asm__ volatile("mult	%1,%2
	mflo	$8
	mfhi	$9
	sll	$9,$9,16
	srl	$8,$8,16
	or	%0,$9,$8"
                         : "=r"(v38.y) : "r"(p2), "r"(s0) : "$8", "$9");
        sc1[0] = ma;
        sc2[1] = mb;
        p1 = sc2[0];
        __asm__("" : "=r"(p1) : "0"(p1));
        {
            s32 d;
            __asm__("mult	%2,%3\n\tmflo	$8\n\tmfhi	$9\n\tsll	$9,$9,16\n\tsrl	$8,$8,16\n\tor	%0,$9,$8"
                    : "=r"(v38.z), "=r"(d) : "r"(p1), "1"(s0) : "$8", "$9");
            vb8.x = v58.x - c->vec.x;
            vb8.y = v58.y - c->vec.y;
            vb8.z = v58.z - c->vec.z;

            if (func_8001AB00((s32 *)&vb8, (s32 *)&v38) < 0) {
                __asm__("" : : "r"(d));
                v38.x = c->vec.x - v38.x;
                v38.y = c->vec.y - v38.y;
                v38.z = c->vec.z - v38.z;
            } else {
                v38.x += c->vec.x;
                v38.y += c->vec.y;
                v38.z += c->vec.z;
            }
        }
    } else {
        v38.x = v58.x + (((self->unkA0 >> 4) * s3->unk16) >> 8);
        v38.y = v58.y + (((self->unkA4 >> 4) * s3->unk16) >> 8);
        v38.z = v58.z + (((self->unkA8 >> 4) * s3->unk16) >> 8);
    }

after_movement:
    s5 = func_800684AC((s32 *)&v58, (s32 *)&v38, (s32 *)&v48, 0, 1);
    if (s5 & 0xFF) {
        v38 = v48;
        fp = 1;
    }

    if (s3->type != 0x0E) {
        if (flag) {
            a2 = s3->unk0E;
            if (a2 > 0) {
                if ((s5 & 0xFF) == 0) {
                    if (c != 0) {
                        func_800410BC((void *)((u8 *)c + 0x50), s3->unk0C, a2 << 4, self);
                        if ((c->unk24 & 0x200080) == 0x200000)
                            goto do_cb;
                        goto skip_callback;
                    } else {
                        func_800410BC((void *)&v38, s3->unk0C, a2 << 4, self);
                        goto skip_callback;
                    }
                } else {
                    goto skip_callback;
                }
            } else {
                goto chk_s5;
            }
        } else {
chk_s5:
            if ((s5 & 0xFF) != 0)
                goto skip_callback;
            if (c == 0)
                goto skip_callback;
        }

do_cb:
        cb.unk06 = arg2;
        cb.unk04 = s3->unk0C;
        cb.unk00 = self->unk14;
        cb.unk08 = self;
        c->unk04(0x0E, c, &cb, 0);
    }

skip_callback:
    if (s3->type == 0x0B) {
        __asm__("" : "=r"(s5) : "0"(s5));
        if (s1 != 0) {
            node1 = func_80073500(&v58, 0, s1->unk48, 0);
            node2 = func_80073500(&v38, 0, (s1->unk4A & 8) ? 0 : s1->unk48, 0);
            func_8007377C(node1, node2, s1->unk36, s1->unk38,
                          s1->unk3B, s1->unk3C, s1->unk3D, s1->unk3E,
                          s1->unk40, s1->unk42, s1->unk44, 0, 0, 1);
        } else {
            node1 = func_80073500(&v58, 0, 0x18, 0);
            node2 = func_80073500(&v38, 0, 0, 0);
            func_8007377C(node1, node2, 0x20, 1,
                          0x7F, 0x7F, 0xFF, 0x2AA, 0x4000, -0x3000, 0, 0, 0, 1);
        }
        if (flag) {
            func_8006A524(self, (s5 & 0xFF) ? 0 : c, &v38,
                          (void *)((u8 *)s3 + 0x0C), s1);
        }
    } else if (s3->type == 0x0A) {
        self->unkAC->unk2FC = s7;
    } else if (s3->type == 0x0C) {
        if (flag) {
            func_8006F0E8(0x300, 0x30, 0x60000 | (func_80019668() & 0x1F000),
                          D_8009F380, &v58, &v38);
        } else {
            func_8006F0E8(0x200, 0x30, 0x30000 + (func_80019668() & 0x1F000),
                          D_8009F384, &v58, &v38);
        }
    } else if (s3->type == 0x0E) {
        func_8005BDE4(self, self, c, (void *)((u8 *)b + 0x178), 0,
                      (void *)((u8 *)s3 + 0x0C));
        fp = 0;
        id = 0;
    }

    if (fp) {
        local.vec = v38;
        local.unk49 = 5;
        local.unk00 = 0x800;
        if (s3->unk1A != 0) {
            func_800402DC(0, s3->unk1A, (s32)&local, 1, 0);
        }
    }

    if (id != 0) {
        local.unk48 = 0;
        local.unk49 = 0x60;
        local.unk44 = instance;
        func_800402DC(0, id, (s32)&local, 1, 0);
    }
}
