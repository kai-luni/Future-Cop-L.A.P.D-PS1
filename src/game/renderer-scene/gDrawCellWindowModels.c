// Categories: Graphics > Renderer > Map Cell Objects | Graphics > Animation | Graphics > Model Renderer > Instance Submission
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

extern struct Slot *gCellBucketGrid __asm__("D_8009F938");
extern s32 gCellGridWidth __asm__("D_8009F93C");
extern s32 gFrameTime __asm__("D_8009FAA4");

extern s32 func_800196C4(void);
extern void gSetModelFrame(struct Obj *o, s32 frame) __asm__("func_800175F8");
extern void gSinCos(s32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
extern void gGteLoadMatrix(void *p) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
extern void gSubmitModelInstance(struct Obj *o, u32 *flags, s32 arg) __asm__("func_80017508");
extern void gAssignSceneSoundVoice(struct Obj *o) __asm__("func_8008363C");
void gDrawCellWindowModels(s32 x, s32 y, u32 *p) __asm__("func_80084398");


/**
 * @brief Walks the 5x5 cell window of one region block and animates plus
 *        submits every object whose band mask matches the cell mask.
 *
 * @param x Cell column of the window's origin, used as x + a + stride * (y + b).
 * @param y Cell row of the window's origin.
 * @param p Region block of 25 entries, 16 bytes each - mask word, object
 *        pointer and the four position halves - as filled by func_80064460.
 * @return Nothing.
 *
 * @par Confirmed
 * A null D_8009F938 ends the call. For every one of the 25 masks (the pointer
 * advances four words per cell) a zero mask is skipped; otherwise the slot
 * index is x + a + D_8009F93C * (y + b) and the object list at that slot is
 * walked. Objects with bit 0x10 in the +0x0F flag byte are ignored, the slot's
 * +0x04 word is stamped with the time base D_8009FAA4 for every visited object,
 * and the
 * animation only runs when the cell mask ANDs non-zero with the object's +0x4E
 * band word. The frame index is chosen by flags: with bit 0x20 the per-object
 * delay +0x4D is decremented by the low three bits of D_8009FAA4 and given a
 * fresh random length from func_800196C4 (0x7F or 0xF depending on the
 * direction byte +0x48) when it underflows; with bit 4 the index ping-pongs over
 * 2 * (+0x4D) * (frames - 1) and is reflected at the end; otherwise it wraps
 * over +0x4D * frames, where frames is the model's +0x0C word. A changed index
 * goes through func_800175F8. The yaw term is (+0x56 * (D_8009FAA4 & 0xFFF))
 * masked to 12 bits, negated through a complement when +0x56 is negative, and
 * with flag bit 0x80 and a non-zero amplitude +0x52 it is folded as a triangle
 * wave scaled by +0x52 over 4096 and offset by +0x50. func_8001846C then yields
 * the sine/cosine pair, stored as +0x1C = sin, +0x24 = +0x04 = cos and +0x0C =
 * -sin, with the neutral 0x10000 when the angle is zero. With +0x57 set,
 * func_80010750 loads that matrix into the GTE and func_80010234 transforms one
 * vertex record from the model block +0x34 at offset frame * 16 + 8 into
 * +0x34. func_80017508(o, &local, 1) finally gates and submits the instance,
 * and func_8008363C follows when +0x57 is set.
 *
 * @par Likely role
 * The third of the three drawers func_80066E98 runs over one screen region:
 * while func_8006531C and func_8004A924 paint the tile and cell geometry, this
 * pass advances the animation of the objects standing in the same 25 cells and
 * submits the visible ones, so the cell masks and the object band word +0x4E
 * are what selects which layers of a region an object belongs to.
 *
 * @par Unknown
 * What the band bits in +0x4E mean per layer, and why the stale-slot stamp
 * writes D_8009FAA4 before the mask test rather than after it.
 *
 * @par Callers
 * func_80066E98, after func_8006531C and func_8004A924 on the same region.
 *
 * @par Environment path
 * Layer 3 of the environment pass: animates and submits the models of the
 * same cell window, i.e. the props standing on the terrain.
 */
void gDrawCellWindowModels(s32 x, s32 y, u32 *p)
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

    if (gCellBucketGrid == 0) {
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
            idx = x + a + gCellGridWidth * (y + b);
            o = gCellBucketGrid[idx].head;
            if (o == 0) {
                continue;
            }
            do {
                if (!(o->flags & 0x10)) {
                    t = gFrameTime;
                    gCellBucketGrid[idx].unk04 = t;
                    if (mask & o->unk4E) {
                        if (!(o->flags & 8) && o->unk4D != 0) {
                            if (o->flags & 0x20) {
                                s = o->unk48;
                                w = o->unk4D - (*(u8 *)&gFrameTime & 7);
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
                                gSetModelFrame(o, s);
                            }
                        }
                        d = o->unk56;
                        if (d != 0) {
                            if (d >= 0) {
                                e = ((gFrameTime & 0xFFF) * d) & 0xFFF;
                            } else {
                                e = ~((gFrameTime & 0xFFF) * -d) & 0xFFF;
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
                                gSinCos(e, &sn, &cs);
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
                                gGteLoadMatrix(&o->unk04);
                                frame = o->unk48;
                                frame <<= 4;
                                __asm__("" : "+r"(frame));
                                frame += 8;
                                gGteTransformVerticesWide((u8 *)o->model->unk34 + frame,
                                              &o->unk34, 1);
                            }
                        }
                        gSubmitModelInstance(o, &local, 1);
                        if (o->unk57) {
                            gAssignSceneSoundVoice(o);
                        }
                    }
                }
                o = o->next;
            } while (o != 0);
        }
    }
}
