// Categories: Gameplay > Entity Update > Orientation | Graphics > GTE > Matrix Build | AI > Steering
#include "game/types.h"

struct Info {
    u8 pad00[8]; s32 count; u8 pad0c[0x1C];
    u8 instance[4]; s32 m[9]; u8 pad50[0x34];
    s32 limit; u8 pad88[0x84]; u16 h10C; u8 pad10E[10];
    u32 flags; u8 pad11C[0xE]; s16 yawBias;
};
struct Target { u8 pad00[8]; s32 id; u8 pad0c[0x50]; s32 pos[3]; };
struct Model { u8 pad00[0x10]; s32 anim; u8 pad14[0x20]; u8 *frames; };
struct Obj {
    u8 pad00[0x28]; struct Model *model; s32 m[9]; s32 pos[3]; s32 vtx[3];
    u8 pad68[8]; s16 frame; u8 pad72[6]; s32 old[3]; u8 pad84[0x1C];
    u8 script[6]; u8 padA6a; u8 padA6b[0x11]; struct Target *target; s32 targetId;
    u8 padC0[8]; u8 anim[0xC]; s16 animFrame; u8 padD6[6];
    s16 yaw; s16 pitch; s32 speed; s16 heading; u8 part; u8 mode; u16 flags;
    struct Info *info;
};

extern void gTransformPartVertex(void *, s32, void *) __asm__("func_80017668");
extern s32 gAtan2(s32, s32) __asm__("func_80095C30");
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gGteTransformWords(void *, void *, s32) __asm__("func_8001006C");
extern void gSinCos(s32, s32 *, s32 *) __asm__("func_8001846C");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern s32 gAdvanceAnimTrack(void *, void *, s32, s32) __asm__("func_80044B34");
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
extern s32 gStepAttackScheduler(void *, s32, void *, void *, s32) __asm__("func_80060238");
extern void gSelectAnimationClip(void *, s32, s32, s32) __asm__("func_800448D8");
extern void gFireEntityWeapon(void *, void *, void *, s32) __asm__("func_8005F6E8");

union Prod { long long val; struct { u32 lo, hi; } w; };
#define FMUL(d,a,b) do { u32 raw; register u32 hi __asm__("$9"); register u32 lo __asm__("$8"); __asm__(".set reorder"); raw = (a) * (b); __asm__("" : "=l"(raw), "=h"(hi) : "0"(raw)); lo = raw; __asm__("" : : "r"(lo)); hi <<= 16; lo >>= 16; (d) = hi | lo; __asm__(".set noreorder"); } while (0)
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

#define STEP(field, tgt) \
    if (obj->field != (tgt)) { \
        s32 df = ((tgt) - obj->field) & 0xFFF; \
        s32 lim; \
        s32 ad; \
        if (df > 0x800) df -= 0x1000; \
        lim = dt * obj->speed; \
        ad = ABS(df); \
        if (ad < lim) lim = ad; \
        if (df < 0) lim = -lim; \
        obj->field = (obj->field + lim) & 0xFFF; \
    }
void gUpdateMountedTurret(struct Obj *obj, s32 dt) __asm__("func_80038A00");


/**
 * @brief Per-frame orientation update of a scripted entity: steers yaw and pitch
 *        toward a target (or aligns the entity to the ground normal), builds the
 *        world matrix, transforms the model vertex, advances the animation and
 *        runs the entity script.
 *
 * @param obj The entity. struct Obj: model +0x28, matrix m[9] +0x2C,
 *            position pos[3] +0x50, transformed vertex vtx[3] +0x5C,
 *            s16 frame +0x70, previous position old[3] +0x78, target-present
 *            byte +0xA6, script bytes +0xA0 (6 bytes), target pointer +0xB8,
 *            targetId +0xBC, anim track +0xC8, animFrame +0xD4, yaw +0xDC,
 *            pitch +0xDE, speed +0xE0, heading +0xE4, part +0xE6, mode +0xE7,
 *            flags +0xE8, info +0xEC. All offsets follow from the struct
 *            declaration at the top of the file.
 *            struct Info: count +0x08, instance +0x28, m[9] +0x2C, limit +0x84,
 *            h10C (u16) +0x10C, flags (u32) +0x118, yawBias (s16) +0x12A.
 *            struct Target: id +0x08, pos[3] +0x5C.
 *            struct Model: anim +0x10, frames +0x34.
 * @param dt   Time step: multiplies obj->speed to bound the per-frame turn rate.
 *
 * @return Nothing. Returns immediately when info->count <= 0.
 *
 * @par Control flow (confirmed)
 * 1. info = obj->info; zero or negative info->count returns at once - the entity
 *    has no live model, so nothing is updated.
 * 2. func_80017668(info->instance, obj->part, obj->pos) resolves the part
 *    position into obj->pos.
 * 3. Two mutually exclusive orientation paths, selected by bit 1 of obj->flags:
 *    a) Scripted steering (bit clear):
 *       - bit 5 set: yaw is forced to heading and the matrix is built directly.
 *       - else the target yaw s2 is (yaw + 0x800) & 0xFFF when the entity has no
 *         target byte or a negative heading, yaw itself when bit 0x8000 is set,
 *         the arctangent of the target delta when the target is valid
 *         (t->id == targetId), and heading otherwise. With bit 4 set the pitch
 *         s1 is additionally derived from the elevation of the same delta
 *         (atan2 on the vertical and the horizontal magnitude, built with the
 *         same max + (b >> 2) - (b >> 6) approximation) and clamped to
 *         -0x200..0x200.
 *       - The bias info->yawBias is subtracted from the target yaw unless bit
 *         0x1000 of info->flags is set.
 *       - STEP(pitch) and STEP(yaw) move the angle toward its target by at most
 *         dt * obj->speed per frame, wrapping in 0x1000 units. Bit 0x4000
 *         further limits the yaw step to +/-0x300 per frame.
 *       - apply: func_800188D0(yaw, pitch, 0, obj->m) builds the rotation; unless
 *         info->flags bit 0x1000 is set, info->m is loaded into the GTE and
 *         post-multiplied (func_80010820, func_8001006C with count 3).
 *    b) Ground alignment (bit set): the half-word info->h10C is the height above
 *       the surface and info->limit the threshold. s1 = 0 while the height is
 *       below the threshold; when limit < h / 4 it becomes 0x400, otherwise
 *       0x400 - ((limit - h/4) << 10) / (h >> 1), i.e. a pitch that follows the
 *       slope. STEP(pitch) is applied to that value, then obj->m is either
 *       copied verbatim from info->m (pitch == 0) or rebuilt from info->m with a
 *       pitch rotation about one axis (func_8001846C plus nine FMULs; the first
 *       row is copied unchanged, rows 2 and 3 are rotated).
 * 4. func_80010750(obj->m) loads the matrix into the GTE and func_80010234
 *    transforms the frame record at model->frames + (frame << 4) + 8 into
 *    obj->vtx with a single vertex.
 * 5. obj->old[0..2] = obj->m[6..8] - the previous translation, kept for the
 *    next frame.
 * 6. Animation: when model->anim is non-zero, func_80044B34(obj, obj->anim, dt,
 *    0) returns a command word which is consumed six bits at a time (>> 1 first,
 *    then a >>= 6 loop): 2 sets flag 0x8000, 1 moves mode 1 to mode 2 (and
 *    nothing otherwise), 3 clears flag 0x8000. func_800175F8(&obj->model,
 *    obj->animFrame) then installs the frame radius.
 * 7. Script: bit 2 of flags returns; bit 3 calls func_80060238(script, dt, info,
 *    0, 1) and returns; mode 0 runs func_80060238(script, dt, obj, info,
 *    ((flags >> 7) ^ 1) & 1) and, when it reports a hit while flag 0x80 is set,
 *    switches mode to 1 and calls func_800448D8(anim, 1, 1, 1); mode 2 resets
 *    mode to 0 and calls func_8005F6E8(script, obj, info, 1).
 *
 * @par Globals
 * None. The function touches only the entity, its Info, its Target and the GTE
 * registers loaded through func_80010750.
 *
 * @par Calls
 * func_80017668 (part position), func_80095C30 (12-bit arctangent),
 * func_800188D0 (rotation matrix from yaw/pitch), func_80010820 /
 * func_8001006C (load and concatenate), func_80010750 (load with translation),
 * func_80010234 (vertex transform), func_80044B34 / func_800175F8 (animation),
 * func_80060238 (entity script), func_800448D8 (animation switch),
 * func_8005F6E8 (script exit handler), func_8001846C (sine/cosine).
 *
 * @par Side effects
 * Writes obj->yaw, pitch, m[9], vtx[3], old[3], mode, flags (0x8000) and calls
 * into the script and animation subsystems, which may in turn change the mode.
 *
 * @par Callers
 * func_800395A8 and func_800398EC (direct), both from message 6 of their entity
 * dispatch tables - i.e. the per-frame tick - with the message argument as the
 * time step.
 *
 * @par Likely
 * The two paths are "vehicle/mission entity steers toward a target or heading"
 * and "walking/ground entity tilts with the terrain". The fact that the same
 * function updates the matrix, the model vertex and the script every frame
 * matches a scripted animated prop or a mission-critical actor.
 *
 * @par Unknown
 * - The unit of info->h10C and info->limit (they are compared as raw
 *   integers and fed through << 10 / >> 1).
 * - Which axis the ground-alignment rotation uses; the code only rotates rows 2
 *   and 3 of info->m, which is consistent with a pitch about the X axis but not
 *   proven by a constant.
 * - The semantics of the script return value tested with & 0xFF.
 *
 * @par Environment path
 * Per-tick update of a child entity mounted on a parent (the +0x158 child of
 * func_800395A8 and child[0] of func_800398EC, both from message 6): it
 * returns while the parent's count is not positive, places itself on the
 * parent's model part with func_80017668 and then either aims yaw and pitch
 * at its target (func_80095C30 arctangents, parent yaw bias, rate-limited
 * STEP, yaw clamped to +-0x300 around the heading when flag 0x4000 is set)
 * or, with flag 2, tilts only by a pitch derived from the parent's height and
 * limit words. The rotation from func_800188D0 is concatenated with the
 * parent matrix (func_80010820, func_8001006C) or built with func_8001846C,
 * loaded with func_80010750 and used to transform one frame vertex with
 * func_80010234. Finally it advances the animation (func_80044B34,
 * func_800175F8) and runs the attack timeline func_80060238 or fires directly
 * through the attack dispatcher func_8005F6E8; it touches no globals.
 */
void gUpdateMountedTurret(struct Obj *obj, s32 dt)
{
    struct Info *info;
    s32 d[3];
    s32 s1, s2;
    s32 f;
    s32 sc[2];

    info = obj->info;
    if (info->count <= 0) return;
    gTransformPartVertex(info->instance - 0, obj->part, obj->pos);
    f = obj->flags;
    if (!(f & 2)) {
        if (f & 0x20) {
            obj->yaw = obj->heading;
            goto apply;
        }
        s1 = 0;
        if (!obj->padA6a || obj->heading < 0) {
            s2 = (obj->yaw + 0x800) & 0xFFF;
        } else {
            struct Target *t = obj->target;
            if (f & 0x8000) {
                s2 = obj->yaw;
            } else if (t && t->id == obj->targetId) {
                d[0] = t->pos[0] - obj->vtx[0];
                d[1] = t->pos[1] - obj->vtx[1];
                d[2] = t->pos[2] - obj->vtx[2];
                s2 = gAtan2(d[0], d[2]) & 0xFFF;
                if (!(obj->info->flags & 0x1000))
                    s2 = (s2 - info->yawBias) & 0xFFF;
                if (obj->flags & 0x10) {
                    register s32 a3 __asm__("$7");
                    register s32 a2 __asm__("$6");
                    s32 a0;
                    a3 = d[0];
                    if (a3 < 0) a3 = -a3;
                    a2 = d[2];
                    if (a2 < 0) a2 = -a2;
                    if (a3 < a2) { a3 ^= a2; a2 ^= a3; a3 ^= a2; }
                    if ((a3 >> 1) < a2) {
                        a0 = a3 + a2;
                        a0 = (a0 << 6) + (a0 << 3) + (a0 << 1) + a3 + (a2 << 5);
                        a2 = a3 - a2;
                        a3 = a0 >> 7;
                    }
                    {
                        register s32 q __asm__("$5");
                        register s32 r __asm__("$2");
                        register s32 arg0 __asm__("$4") = d[1];
                        __asm__("" : : "r"(arg0));
                        q = a2 >> 2;
                        q = a3 + q;
                        r = a2 >> 6;
                        q -= r;
                        s1 = (-(gAtan2(arg0, q) << 12)) >> 12;
                    }
                    {
                        s32 cl;
                        if (s1 < -0x200) cl = -0x200;
                        else if (s1 < 0x201) cl = s1;
                        else cl = 0x200;
                        s1 = cl;
                    }
                }
            } else {
                s2 = obj->heading;
            }
        }
        STEP(pitch, s1);
        if (obj->yaw != s2) {
            if (obj->flags & 0x4000) {
                s32 h = obj->heading;
                register s32 tmp __asm__("$2") = s2 - h;
                register s32 df __asm__("$5") = tmp & 0xFFF;
                if (df > 0x800) df -= 0x1000;
                if (df < -0x300) s2 = (h - 0x300) & 0xFFF;
                else {
                    tmp = df < 0x301;
                    if (!tmp) s2 = (h + 0x300) & 0xFFF;
                }
            }
            STEP(yaw, s2);
        }
apply:
        gBuildRotationMatrix(obj->yaw, obj->pitch, 0, obj->m);
        if (!(obj->info->flags & 0x1000)) {
            gGteLoadRotation(info->m);
            gGteTransformWords(obj->m, obj->m, 3);
        }
    } else {
        s32 h = (s16)info->h10C;
        s32 a0 = h >> 2;
        s32 v = h - a0;
        s32 v3 = info->limit;
        s1 = 0;
        if (v >= v3) {
            if (v3 < a0)
                s1 = 0x400;
            else
                s1 = 0x400 - ((v3 - a0) << 10) / (h >> 1);
        }
        STEP(pitch, s1);
        if (obj->pitch == 0) {
            obj->m[0] = info->m[0]; obj->m[1] = info->m[1]; obj->m[2] = info->m[2];
            obj->m[3] = info->m[3]; obj->m[4] = info->m[4]; obj->m[5] = info->m[5];
            obj->m[6] = info->m[6]; obj->m[7] = info->m[7]; obj->m[8] = info->m[8];
        } else {
            register s32 c __asm__("$6");
            register s32 s __asm__("$5");
            register s32 i3 __asm__("$7");
            register s32 i6 __asm__("$4");
            register s32 p3 __asm__("$3");
            register s32 p2 __asm__("$2");
            gSinCos(obj->pitch, &sc[0], &sc[1]);
            {
                s32 t0 = info->m[0], t1 = info->m[1], t2 = info->m[2];
                obj->m[0] = t0; obj->m[1] = t1; obj->m[2] = t2;
            }
            i3 = info->m[3]; __asm__("" : : "r"(i3));
            c = sc[1]; __asm__("" : : "r"(c));
            FMUL(p3, c, i3);
            i6 = info->m[6]; __asm__("" : : "r"(i6));
            s = sc[0]; __asm__("" : : "r"(s));
            FMUL(p2, s, i6); p3 -= p2;
            FMUL(p2, c, i6); __asm__("" : : "r"(p2) : "memory"); obj->m[3] = p3;
            FMUL(p3, s, i3); p2 += p3; obj->m[6] = p2;
            i3 = info->m[4]; __asm__("" : : "r"(i3));
            FMUL(p2, c, i3);
            i6 = info->m[7]; __asm__("" : : "r"(i6));
            FMUL(p3, s, i6); p2 -= p3; obj->m[4] = p2;
            FMUL(p2, c, i6); FMUL(p3, s, i3); p2 += p3; obj->m[7] = p2;
            i3 = info->m[5]; __asm__("" : : "r"(i3));
            FMUL(p2, c, i3);
            i6 = info->m[8]; __asm__("" : : "r"(i6));
            FMUL(p3, s, i6); p2 -= p3; obj->m[5] = p2;
            FMUL(p2, c, i6); FMUL(p3, s, i3); p2 += p3; obj->m[8] = p2;
        }
    }
    gGteLoadMatrix(obj->m);
    {
        register s32 *out __asm__("$5");
        register s32 one __asm__("$6");
        register s32 fo __asm__("$4");
        register struct Model *model __asm__("$2");
        register u8 *frames __asm__("$2");
        out = obj->vtx; one = 1;
        __asm__("" : : "r"(out), "r"(one));
        fo = obj->frame;
        __asm__("" : : "r"(fo));
        model = obj->model;
        __asm__("" : : "r"(model));
        fo = fo << 4;
        __asm__("" : "=r"(fo) : "0"(fo));
        frames = model->frames;
        __asm__("" : : "r"(frames));
        fo += 8;
        __asm__("" : : "r"(fo));
        gGteTransformVerticesWide((u8 *)((s32)frames + fo), out, one);
    }
    obj->old[0] = obj->m[6]; obj->old[1] = obj->m[7]; obj->old[2] = obj->m[8];
    __asm__ volatile ("" : : : "memory");
    if (obj->model->anim) {
        s32 a = gAdvanceAnimTrack(obj, obj->anim, dt, 0) >> 1;
        while (a) {
            switch (a & 0x3F) {
            case 2: obj->flags |= 0x8000;
            case 1: if (obj->mode == 1) obj->mode = 2; break;
            case 3: obj->flags &= 0x7FFF; break;
            }
            a >>= 6;
        }
        gSetModelFrame(&obj->model, obj->animFrame);
    }
    f = obj->flags;
    if (f & 4) return;
    if (f & 8) {
        gStepAttackScheduler(obj->script, dt, info, 0, 1);
        return;
    }
    if (obj->mode == 0) {
        if (f & 0x8000) return;
        if (gStepAttackScheduler(obj->script, dt, obj, info, (((u32)f >> 7) ^ 1) & 1) & 0xFF) {
            if (obj->flags & 0x80) {
                obj->mode = 1;
                gSelectAnimationClip(obj->anim, 1, 1, 1);
            }
        }
    } else if (obj->mode == 2) {
        obj->mode = 0;
        gFireEntityWeapon(obj->script, obj, info, 1);
    }
}
