// Categories: Graphics > Effects > Part Motion | Physics > Motion | Graphics > Renderer > Line Pass
#include "game/types.h"

extern int gWeaponSlotUpdate();

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

extern s32 gTracerRedCoreColour __asm__("D_8009F380");
extern s32 gTracerGreenCoreColour __asm__("D_8009F384");
extern void *gSpawnRecordMap[][0x100] __asm__("D_800A7040");

extern struct Unk6B23C_Item *gRegistryFind(u8 id) __asm__("func_8006AAEC");
extern void gRefreshPlayerWeaponSound(struct Unk6B23C *arg0, u8 arg1) __asm__("func_80046CF0");
extern void gTransformPartVertex(void *instance, s32 part, s32 *out) __asm__("func_80017668");
extern s32 func_80019668(void);
extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
extern s32 gVectorDot(s32 *a, s32 *b) __asm__("func_8001AB00");
extern s32 gTraceMapRay(s32 *a, s32 *b, s32 *c, s32 d, s32 e) __asm__("func_800684AC");
extern void gApplyRadialContactDamage(void *a, s16 b, s32 c, struct Unk6B23C *d) __asm__("func_800410BC");
extern void *gAllocPositionNode(Vec6B23C *pos, s32 a1, s32 a2, s32 a3) __asm__("func_80073500");
extern void gCreateRibbonBeam(void *a0, void *a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, s32 a12, s32 a13) __asm__("func_8007377C");
extern void gSpawnChainBeamArcs(struct Unk6B23C *a0, void *a1, Vec6B23C *a2, void *a3, void *a4) __asm__("func_8006A524");
extern void gAttachNearPlayerEffect(struct Unk6B23C *a0, struct Unk6B23C *a1, struct Unk6B23C_C *a2, void *a3, s32 a4, void *a5) __asm__("func_8005BDE4");
extern void gSpawnTracerBeam(s32 a0, s32 a1, s32 a2, s32 a3, Vec6B23C *a4, Vec6B23C *a5) __asm__("func_8006F0E8");
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");

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

/**
 * @brief Updates one animated part of a beam/effect object.
 *
 * Recomputes the part's target position (definition table plus optional random
 * spread around an anchor), resolves the swept move against obstacles, and then
 * runs the type-specific visual and callback for the part.
 *
 * @param arg0 Effect object; the work block sits at arg0->unkAC.
 * @param index Part slot; selects the definition id (unk90[index]) and the
 *        variant halfword (unk84/unk8A[index]).
 * @param arg2 Frame counter forwarded into the callback block (cb.unk06).
 * @return Nothing.
 *
 * @par Confirmed
 * b = self->unkAC, c = b->unk6C (a render/collision handle, may be 0) and
 * item = gRegistryFind(b->unk90[index]). Two record halves are used:
 * item + 0x24 when b->unk8A[index] > 0, else item + 0x04 (s3).
 * func_80046CF0(self, s3->unk1C) is called on every pass.
 * The variant selector s7 is (u16)b->unk8A[index] & 1 for the positive case,
 * otherwise b->unk84[index] & 1.
 * The instance transform is b + 0x178, or b + 0x178 + s7 * 0x4C;
 * func_80017668(instance, 0, &v58) yields the current part position.
 * For type 0x0B with s3->unk1B != 0, s1 = D_800A7040[1][s3->unk1B] is accepted
 * only when s1->unk04 == 0x58.
 * New position v38 comes from one GTE-transformed point of c (or from s1), or
 * from random angles: two func_80019668() values masked with 0xFFF are turned
 * into sine/cosine by func_8001846C, the radius is s1->unk34 << 4 or
 * c->unk68 >> 1 (s1 path), s3->unk0E or (s16)c->unk6E >> 1 (s3 path), and the
 * products use the local MUL16 16.16 macro. With c == 0 the offset is
 * v58 + (((self->unkA0/A4/A8 >> 4) * s3->unk16) >> 8).
 * func_8001AB00(&vb8, &v38) decides the sign: the offset is added to c->vec or
 * subtracted from it.
 * Swept test: s5 = func_800684AC(&v58, &v38, &v48, 0, 1); a non-zero low byte
 * means blocked, so v38 = v48 (contact point) and fp = 1.
 * Dispatch on s3->type: 0x0B builds two scene nodes with func_80073500 and
 * draws the segment with func_8007377C (fourteen arguments including colour and
 * width; without s1 the fixed set 0x20/1/0x7F/0x7F/0xFF/0x2AA/0x4000/-0x3000
 * is used) and then calls func_8006A524. 0x0A stores s7 in b->unk2FC. 0x0C
 * emits func_8006F0E8 with 0x60000 | random or 0x30000 + random. 0x0E delegates
 * to func_8005BDE4 and clears both fp and id.
 * Otherwise, and also for the 0x0B path when (c->unk24 & 0x200080) == 0x200000,
 * a block {unk00 = self->unk14, unk04 = s3->unk0C, unk06 = arg2, unk08 = self}
 * is passed to the indirect call c->unk04(0x0E, c, &cb, 0).
 * Finally func_800402DC(0, id, &local, 1, 0) queues up to two effects: character
 * 5 at v38 with unk00 = 0x800 when fp is set, and character id (s1->unk15 or
 * s3->unk1B) with unk49 = 0x60 and unk44 = instance.
 *
 * @par Likely
 * The per-part update of a tether/beam object (rope, cable or arc between two
 * points): the record halves are the two ends, the random term keeps the segment
 * loose, and the type byte selects beam, trail, sparkle or impact behaviour.
 *
 * @par Unknown
 * The semantics of the s3 type table (0x0A..0x0E) and of the 0x58 tag check on
 * s1; which indices the caller walks.
 *
 * @par Callers
 * gWeaponSlotUpdate.
 */
void gBeamPartUpdate(struct Unk6B23C *arg0, s32 index, s32 arg2) __asm__("func_8006B23C");

void gBeamPartUpdate(struct Unk6B23C *arg0, s32 index, s32 arg2)
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
    item = gRegistryFind(b->unk90[index]);
    flag = (((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] > 0);
    if (flag)
        s3 = (struct Unk6B23C_Sub *)((u8 *)item + 0x24);
    else
        s3 = (struct Unk6B23C_Sub *)((u8 *)item + 0x04);
    gRefreshPlayerWeaponSound(self, s3->unk1C);
    if (((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] > 0)
        s7 = (u16)((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk8A[0] & 1;
    else
        s7 = ((struct Unk6B23C_B *)((u8 *)b + (index << 1)))->unk84[0] & 1;

    s1 = 0;
    if (s3->type == 0x0B) {
        if (s3->unk1B != 0) {
            s1 = (struct Unk6B23C_S1 *)gSpawnRecordMap[1][s3->unk1B];
            if (s1 != 0 && s1->unk04 != 0x58)
                s1 = 0;
        }
        instance = (void *)((u8 *)b + 0x178);
        gTransformPartVertex(instance, 0, (s32 *)&v58);
    } else {
        instance = (void *)((u8 *)b + 0x178 + s7 * 0x4C);
        gTransformPartVertex(instance, 0, (s32 *)&v58);
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
                gTransformPartVertex((void *)((u8 *)c + 0x28), (s8)ang, (s32 *)&v38);
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
        gSinCos(angle, &sc1[0], &sc1[1]);
        angle = func_80019668() & 0xFFF;
        gSinCos(angle, &sc2[0], &sc2[1]);

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

            if (gVectorDot((s32 *)&vb8, (s32 *)&v38) < 0) {
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
    s5 = gTraceMapRay((s32 *)&v58, (s32 *)&v38, (s32 *)&v48, 0, 1);
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
                        gApplyRadialContactDamage((void *)((u8 *)c + 0x50), s3->unk0C, a2 << 4, self);
                        if ((c->unk24 & 0x200080) == 0x200000)
                            goto do_cb;
                        goto skip_callback;
                    } else {
                        gApplyRadialContactDamage((void *)&v38, s3->unk0C, a2 << 4, self);
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
            node1 = gAllocPositionNode(&v58, 0, s1->unk48, 0);
            node2 = gAllocPositionNode(&v38, 0, (s1->unk4A & 8) ? 0 : s1->unk48, 0);
            gCreateRibbonBeam(node1, node2, s1->unk36, s1->unk38,
                          s1->unk3B, s1->unk3C, s1->unk3D, s1->unk3E,
                          s1->unk40, s1->unk42, s1->unk44, 0, 0, 1);
        } else {
            node1 = gAllocPositionNode(&v58, 0, 0x18, 0);
            node2 = gAllocPositionNode(&v38, 0, 0, 0);
            gCreateRibbonBeam(node1, node2, 0x20, 1,
                          0x7F, 0x7F, 0xFF, 0x2AA, 0x4000, -0x3000, 0, 0, 0, 1);
        }
        if (flag) {
            gSpawnChainBeamArcs(self, (s5 & 0xFF) ? 0 : c, &v38,
                          (void *)((u8 *)s3 + 0x0C), s1);
        }
    } else if (s3->type == 0x0A) {
        self->unkAC->unk2FC = s7;
    } else if (s3->type == 0x0C) {
        if (flag) {
            gSpawnTracerBeam(0x300, 0x30, 0x60000 | (func_80019668() & 0x1F000),
                          gTracerRedCoreColour, &v58, &v38);
        } else {
            gSpawnTracerBeam(0x200, 0x30, 0x30000 + (func_80019668() & 0x1F000),
                          gTracerGreenCoreColour, &v58, &v38);
        }
    } else if (s3->type == 0x0E) {
        gAttachNearPlayerEffect(self, self, c, (void *)((u8 *)b + 0x178), 0,
                      (void *)((u8 *)s3 + 0x0C));
        fp = 0;
        id = 0;
    }

    if (fp) {
        local.vec = v38;
        local.unk49 = 5;
        local.unk00 = 0x800;
        if (s3->unk1A != 0) {
            gSpawnTypeAtOwner(0, s3->unk1A, (s32)&local, 1, 0);
        }
    }

    if (id != 0) {
        local.unk48 = 0;
        local.unk49 = 0x60;
        local.unk44 = instance;
        gSpawnTypeAtOwner(0, id, (s32)&local, 1, 0);
    }
}
