// Categories: Gameplay > AI > Hover Enemy Behaviour | Physics > Motion > Ground Follow | Graphics > GTE > Matrix Build | Game Engine > Entity Lifecycle
#include "game/types.h"

typedef long long s64;

typedef struct V3 { s32 x; s32 y; s32 z; } V3;
typedef union Prod { s64 v; u32 w[2]; } Prod;

struct Sub { u8 p00[0x10]; s32 f10; u8 p14[0x20]; s32 f34; };
struct Res { u8 p00[8]; s32 f08; u8 p0c[0x44]; V3 pos; };
struct Hit { u8 p00[0x14]; u16 f14; u8 p16[0x96]; struct Hit *fAC; };
struct Q { u8 p00[0x3E]; u16 f3E; u8 p40[8]; s32 f48; };

struct Obj {
    u8 p00[0x16]; u8 type; u8 p17[2]; u8 f19; u8 p1a[2];
    s16 f1C; u16 f1E; u8 f20; u8 p21[3];
    u32 f24; struct Sub *f28; V3 f2C; V3 f38; u8 p44[0xC];
    V3 pos;
    u8 p5C[0xC]; s32 f68; s16 f6C; u8 p6E[2]; s16 f70; u8 p72[6];
    V3 f78; u8 p84[8]; s32 f8C; u8 p90[8]; s32 f98; u8 p9C[4];
    u32 fA0; u8 pA4[0x24];
    struct Res *fC8; s32 fCC; s32 fD0; u8 pD4[4]; s32 fD8;
    V3 fDC; V3 fE8; s32 fF4; s16 fF8; u8 fFA; u8 pFB;
    u16 fFC; u16 fFE; u16 f100; u16 f102; u8 f104; u8 f105;
    s16 f106; s16 f108; s16 f10A; s16 f10C; s16 f10E;
    s16 f110; s16 f112; s16 f114; s16 f116; s32 f118; s32 f11C;
    u8 p120[0xC]; s16 f12C; u8 p12E[6]; struct Q *f134;
};

extern s32 *gGameSession __asm__("D_8009FA94");
extern struct Hit *gPlayerEntities __asm__("D_8009FA58");
extern s32 gFrameTime __asm__("D_8009FAA4");

extern void gAdvanceAnimTrack(void *, void *, s32, s32) __asm__("func_80044B34");
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
extern s32 func_80019668(void);
extern struct Res *gFindTargetEntity(void *, s32, s32, s32, s32, s32, s32) __asm__("func_8004E80C");
extern void gPickHoverWanderTarget(struct Obj *) __asm__("func_800320E4");
extern s32 gSampleHeightGridBilinear(V3 *) __asm__("func_80069A38");
extern s32 gGetGroundHeight(V3 *, s32, s32, s32) __asm__("func_80066F78");
extern void gFlagQuartileChange(struct Obj *, s32) __asm__("func_8003BA50");
extern void gSpawnEntitySlotTypes(s32, struct Obj *, s32, s32, s32) __asm__("func_8003B898");
extern struct Hit *gFindUnitById(s32) __asm__("func_8004D664");
extern void gInterpretEntityScript(s32, s32, s32, struct Obj *) __asm__("func_80069D68");
extern void gVectorAddScaled(V3 *, V3 *, s32, V3 *) __asm__("func_8001ACA4");
extern void gNormalizeVector(V3 *) __asm__("func_8001A180");
extern s32 func_8001A750(V3 *, V3 *);
extern void func_8001A644(V3 *);
extern void gVectorScaleInPlace(s32, V3 *) __asm__("func_8001AB78");
extern void gEaseVectorQuarter(V3 *, V3 *) __asm__("func_8001A6F8");
extern void gBuildBasisZFromPair(V3 *, V3 *, V3 *) __asm__("func_80019310");
extern void gGteLoadMatrix(V3 *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(s32, void *, s32) __asm__("func_80010234");
#define FMUL(d, x, y) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(x), "r"(y) : "$8", "$9")

extern void gStepAttackScheduler(u32 *, s32, struct Obj *, s32, s32) __asm__("func_80060238");

#define AB8(v) ((((v) >> 8) >> 31) + ((v) >> 8) ^ (((v) >> 8) >> 31))
/**
 * @brief Message-6 tick of a hovering enemy: re-acquires a target, flies after it
 *        inside a ground-referenced altitude band, and runs the approach, dive,
 *        descent and death states.
 *
 * @param message Message id. Both call sites pass 6 and the body never reads it.
 * @param o Object block; the fields used reach from +0x16 (type) to +0x134.
 * @param dt Frame delta: drains the timers, advances the animation and scales
 *        every velocity term.
 * @return Always 0.
 *
 * @par Confirmed prologue
 * A non-zero animation counter at model+0x10 runs func_80044B34(o, o+0x120, dt, 0)
 * and func_800175F8(address of the model pointer, +0x12C) - the address of the
 * pointer is passed, so the model can replace itself. For every type except 0x25
 * the retarget countdown at +0x102 reloads to (random 9 bits) + 0x384 and calls
 * func_8004E80C(o, +0x105, +0x106, +0x104, +0x108 << 4, 0xffff0000, 0), caching
 * the block id (+0x08) at +0xCC; a later mismatch drops the pointer after
 * func_800320E4 re-rolls the wander point around +0xD0/+0xD8. States below 2
 * alternate between the timers +0xFC and +0xFE with a random quarter of jitter.
 *
 * @par Confirmed states (+0xFA)
 * 0 and 1 - approach the tracked object's position when the +0xC8 block exists,
 * otherwise the wander point at +0xDC; a drift beyond +0x10E/+0x10C (absolute,
 * world units divided by 256) away from home re-rolls that point and enters 1.
 * 3 - dive. func_80066F78(&pos, 0, 0, 0) probes the terrain: once the probe surface
 * plus half the body height (+0x68) reaches the body, the impact stamps animation
 * frame +0x1E into +0x1C, ORs 0x35 into +0x24, arms a countdown from word 0x13 of
 * D_8009FA94, updates map occupancy with func_8003B898, bumps the counter at +0x3E
 * of the +0x134 record (stamping it with D_8009FAA4), compares the struck entity's
 * class word (+0x14) against the player block D_8009FA58 and increments a counter
 * through its +0xAC pointer, runs the script at +0x98 via func_80069D68, and enters
 * state 5. While airborne it bounces: the velocity at +0xE8 is pulled back toward
 * the wander point by gVectorAddScaled with a negative step and jittered by
 * (random 4 bits - 3) << 9, then +0xDC and +0x5C are chased with lerps of dt << 8.
 * 4 - descent: y falls by (dt * +0xF4) >> 9 until the body is within (+0x114 << 4)
 * of the ground height from func_80069A38 plus (+0x6C << 4), then back to 0.
 * 5 - dead; only +0x1C is cleared, and only for types other than 0x25.
 *
 * @par Confirmed steering
 * The direction is target minus position, but when the planar distance from
 * func_8001A750 drops below (+0x10A << 4), or while the turn-away cooldown at
 * +0x116 runs (re-armed with (random 7 bits) + 0x96), it is negated - the enemy
 * backs off. y is zeroed, func_8001A644 normalises and func_8001AB78 applies the
 * step (dt * +0xF8) >> 3. Altitude is separate: below the upper edge (+0x114 << 4)
 * the climb is +0x1800, between the two lower edges (+0x110, +0x112) it is scaled
 * by +0x11C/+0x118, above the band the descent is -0x1800, and magnitudes under
 * 0x201 are dropped. func_8001A6F8 blends that into +0xE8 (three quarters old, one
 * quarter new), the vertical part is damped by a quarter, and the flight direction
 * at +0x78 accumulates it, is damped by an eighth, normalised and clamped to
 * +-0x8000 on y.
 *
 * @par Confirmed tail
 * Position += direction * speed through the fixed-point product macro, clamped to
 * the ground; the horizontal speed was scaled beforehand by how far the up row at
 * +0x38 departs from vertical (full speed at y = 0x10000, one sixteenth at 0), so
 * banking slows the enemy down. Type 0x25 or state 0 also steps the script at
 * +0xA0 with func_80060238. The three-row basis at +0x2C is rebuilt by
 * func_80019310 and loaded by func_80010750, and the current model frame (+0x70,
 * stride 0x10, table at model+0x34) is transformed into +0x5C by func_80010234.
 *
 * @par Likely
 * A flying attacker: it loiters around a home point, wanders when it loses its
 * prey, dives (state 3) and scores a kill on impact. The recurring "except type
 * 0x25" guards suggest 0x25 is the player-controlled counterpart of this class.
 *
 * @par Unknown
 * The meaning of +0x104..+0x108 (a search class handed to func_8004E80C) and of the
 * +0x134 record (+0x3E counter, +0x48 stamp) cannot be proven here, nor can the
 * first argument of the func_80019310 call, which is assigned only on the dive path.
 *
 * @par Callers
 * func_80032F3C (the message-6 case of the type's message table) and
 * func_8003532C.
 *
 * @par Environment path
 * Shared flyer motion tick, called from the message handlers func_80032F3C
 * and func_8003532C: it advances the animation (func_80044B34,
 * func_800175F8), periodically re-acquires a target with func_8004E80C and
 * toggles between the pursue (0) and wander (1) states on random timers. In
 * states 0/1 it steers toward the target or its home point (func_8001A750,
 * func_8001A644, func_8001AB78, func_8001A6F8), holds an altitude band above
 * the heightfield sample func_80069A38, integrates the position and ticks its
 * attack descriptor with func_80060238; state 4 climbs back into the band.
 * State 3 is the crash fall: it tumbles with random jitter until the ground
 * height from func_80066F78 is reached, and for type 0x25 then registers map
 * occupancy (func_8003B898), stamps the record at +0x134 with the frame time
 * D_8009FAA4, credits a kill to player slot 0 of D_8009FA58 when it was the
 * killer (func_8004D664) and runs the death script with func_80069D68 before
 * entering the dead state 5. Every live path ends by building the basis with
 * func_80019310 and transforming the frame vertex (func_80010750,
 * func_80010234); no primitives are emitted.
 */
s32 gUpdateHoverEnemy(s32 unused, struct Obj *o, s32 dt) __asm__("func_800324E0");

s32 gUpdateHoverEnemy(s32 unused, struct Obj *o, s32 dt)
{
    V3 a;
    V3 b;
    s32 ay;
    s32 cmp;
    V3 *q;

    if (o->f28->f10 != 0) {
        gAdvanceAnimTrack(o, (u8 *)o + 0x120, dt, 0);
        gSetModelFrame((u8 *)o + 0x28, o->f12C);
    }

    if (o->type != 0x25) {
        o->f102 -= dt;
        if ((s16)o->f102 < 0) {
            o->f102 = (func_80019668() & 0x1FF) + 0x384;
            o->fC8 = gFindTargetEntity(o, o->f105, o->f106, o->f104,
                                   o->f108 << 4, 0xFFFF0000, 0);
            if (o->fC8 == 0)
                goto L5E4;
            o->fCC = o->fC8->f08;
        }
        if (o->fC8 != 0 && o->fCC != o->fC8->f08) {
            gPickHoverWanderTarget(o);
            o->fC8 = 0;
        }
L5E4:
        if (o->fFA < 2) {
            o->f100 -= dt;
            if ((s16)o->f100 < 0) {
                if (o->fFA == 0) {
                    gPickHoverWanderTarget(o);
                    o->fFA = 1;
                    o->f100 = o->fFE +
                        (u32)((s16)o->fFE * (func_80019668() & 0xFF)) / 1024;
                } else {
                    o->fFA = 0;
                    o->f100 = o->fFC +
                        (u32)((s16)o->fFC * (func_80019668() & 0xFF)) / 1024;
                }
            }
        }
    }

    switch (o->fFA) {
    case 4: {
        s32 gr;
        o->pos.y += (dt * o->fF4) >> 9;
        gr = gSampleHeightGridBilinear(&o->pos) + (o->f6C << 4);
        if (o->pos.y - gr > (o->f114 << 4))
            o->fFA = 0;
        a.z = 0;
        a.x = 0;
        ay = 0x10000;
        goto L_ED0;
    }
    case 0: {
        s32 cA;
        s32 dA;
        if (o->type != 0x25) {
            cA = o->pos.x - o->fD0;
            cA = AB8(cA);
            dA = o->pos.z - o->fD8;
            if (o->f10E < cA || o->f10C < AB8(dA)) {
                gPickHoverWanderTarget(o);
                b = o->fDC;
                if (o->type == 0x25)
                    goto L_AD4;
                o->fFA = 1;
                o->f100 = (func_80019668() & 0x7FF) + 0x4B0;
                goto L_AD4;
            }
        }
        if (o->fC8 != 0) {
            b = o->fC8->pos;
            goto L_AD4;
        }
        b = o->fDC;
        if (o->type == 0x25)
            goto L_AD4;
        o->fFA = 1;
        o->f100 = 0x4B0 - ((0x4B0 - (s16)o->fFE) >> 31 & (0x4B0 - (u16)o->fFE));
        goto L_AD4;
    }
    case 1:
        b = o->fDC;
        goto L_AD4;
    case 3: {
        V3 *pp;
        V3 *f78p;
        s32 t3;
        s32 c3;
        pp = &o->pos;
        t3 = gGetGroundHeight(pp, 0, 0, 0);
        if (!(t3 + (o->f68 >> 1) < o->pos.y)) {
            if (o->type == 0x25) {
                s32 k;
                k = gGameSession[0x13];
                o->f1C = o->f1E;
                o->f24 |= 0x35;
                o->f100 = (10 - k) * 150 + 600;
                if (o->f20 != 0) {
                    if (!(o->f24 & 0x40000))
                        gFlagQuartileChange(o, 0);
                }
                o->f1C = -1;
                gSpawnEntitySlotTypes(o->f20, o, 0, (o->f19 >> 3) & 0x1E, 2);
                if (o->f134->f48 == 0)
                    o->f134->f48 = gFrameTime;
                o->f134->f3E++;
                if (o->f8C != 0) {
                    struct Hit *h;
                    h = gFindUnitById(o->f8C & 0x3FFFFFFF);
                    if (h != 0 && gPlayerEntities != 0 &&
                        (h->f14 & 0x3FFF) == gPlayerEntities->f14)
                        gPlayerEntities->fAC->f14++;
                }
                if (o->f98 != 0)
                    gInterpretEntityScript(o->f98, 0, -1, o);
            }
            o->fFA = 5;
            return 0;
        }
        gVectorAddScaled(&o->fE8, &o->fDC, -0x400, &o->fE8);
        t3 = func_80019668();
        c3 = dt << 8;
        o->fE8.y -= ((t3 & 0xF) - 3) << 9;
        gVectorAddScaled(&o->fDC, &o->fE8, c3, &o->fDC);
        gVectorAddScaled(pp, &o->fDC, c3, pp);
        a = o->f38;
        a.y = a.y + 0x2000;
        f78p = &o->f78;
        q = &o->f78;
        o->f78 = o->fDC;
        gNormalizeVector(q);
        goto L_ED8;
    }
    case 5:
        if (o->type != 0x25)
            o->f1C = -1;
        return 0;
    default:
        break;
    }

L_AD4: {
    s32 ang;
    s32 dL;
    s32 gL;
    s32 cq;
    Prod pr;
    a.x = b.x - o->pos.x;
    a.y = b.y - o->pos.y;
    a.z = b.z - o->pos.z;
    ang = func_8001A750(&b, &o->pos);
    if (o->type != 0x25) {
        if (o->f116 > 0)
            o->f116 -= dt;
    }
    if ((ang >> 4) < o->f10A || o->f116 > 0) {
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        if (o->type != 0x25 && o->f116 <= 0)
            o->f116 = (func_80019668() & 0x7F) + 0x96;
    }
    a.y = 0;
    func_8001A644(&a);

    dL = (dt * o->fF8) >> 3;
    gL = gSampleHeightGridBilinear(&o->pos) + (o->f6C << 4);
    gVectorScaleInPlace(dL, &a);
    cmp = gL + (o->f114 << 4);
    if (o->pos.y < cmp) {
        if (gL + (o->f110 << 4) >= o->pos.y) {
            dL = 0x1800;
        } else {
            pr.v = (s64)(cmp - o->pos.y) * o->f11C;
            dL = ((s32)((pr.w[1] << 16) | (pr.w[0] >> 16)) * 0x1800) >> 16;
        }
    } else {
        if (o->pos.y >= gL + (o->f112 << 4)) {
            dL = -0x1800;
        } else {
            pr.v = (s64)(cmp - o->pos.y) * o->f118;
            dL = ((s32)((pr.w[1] << 16) | (pr.w[0] >> 16)) * 0x1800) >> 16;
        }
    }
    a.y = (((dL >> 31) + dL) ^ (dL >> 31)) < 0x201 ? 0 : dL;
    gEaseVectorQuarter(&a, &o->fE8);
    o->fE8.y = o->fE8.y - o->fE8.y / 4;
    o->f78.x += a.x;
    o->f78.y += a.y;
    o->f78.z += a.z;
    o->f78.y = o->f78.y - o->f78.y / 8;
    func_8001A644(&o->f78);
    dL = o->f78.y;
    if (dL < -0x8000)
        o->f78.y = -0x8000;
    else if (dL > 0x8000)
        o->f78.y = 0x8000;
    gNormalizeVector(&o->f78);

    dL = (dt * o->fF4) >> 8;
    cmp = (0x10000 - (((o->f38.y >> 31) + o->f38.y) ^ (o->f38.y >> 31))) * 2;
    dL = (dL * (0x200 - ((cmp - cmp / 16) >> 8))) >> 9;

    FMUL(cq, dL, o->f78.x);
    o->pos.x += cq;
    FMUL(cq, dL, o->f78.y);
    o->pos.y += cq;
    FMUL(cq, dL, o->f78.z);
    o->pos.z += cq;
    if (o->pos.y < gL)
        o->pos.y = gL;

    if (o->type == 0x25 || o->fFA == 0) {
        if (o->fA0 != 0)
            gStepAttackScheduler(&o->fA0, dt, o, 0, 1);
    }
    a = o->fE8;
    ay = a.y + 0x2000;
}

L_ED0:
    a.y = ay;
L_ED8:
    gBuildBasisZFromPair(q, &a, &o->f2C);
    gGteLoadMatrix(&o->f2C);
    gGteTransformVerticesWide((o->f70 << 4) + 8 + o->f28->f34, (u8 *)o + 0x5C, 1);
    return 0;
}
