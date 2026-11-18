// Categories: Physics > Force Fields | Gameplay > Object Update > Tick Handler | Utilities > Mathematics > Fixed Point
#include "game/types.h"

typedef struct { s32 x, y, z; } Vec;
typedef struct Sub { u8 pad[0xbc]; s32 state; u8 padc0[0x20]; Vec velocity; } Sub;
typedef struct Obj {
    u8 pad0[8]; s32 sound; u8 padc; u8 pad0d[0x14]; u8 soundPlaying; s8 soundIndex; u8 pad23;
    u32 flags; void *model; u8 transform[0x24]; Vec position; Vec lastPosition;
    u8 pad68[8]; s16 part; u8 pad72[0x2e]; s32 motion[3]; Sub *sub;
    u8 padb0[0x6c]; Vec forces[2]; s16 range; s16 strength; u16 timeA, timeB; u8 curve, mode; s16 timer; u8 effectFlags, soundType; u8 pad142[2]; s32 multiplier;
} Obj;
typedef struct Model { u8 pad[0x34]; void *parts; } Model;
extern int gPlayerCount __asm__("D_8009FA0C");
extern Obj *D_8009FA58[];
extern void func_8003A09C(Obj *);
extern s32 func_8001A100(Vec *);
extern void func_8001A6F8(Vec *, Vec *);
extern void func_8001AB78(s32, Vec *);
extern void func_800457B8(s32, s32);
extern int func_80050D14(void *, s32, Obj *);
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, int) __asm__("func_80010234");
extern void func_80039E8C(int);
extern int func_8002AD58(int, void *, void *);
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
#define FMULP(d,a,b) do { register s32 mul __asm__("$2") = (b); FMUL(d,a,mul); } while (0)
#define FMULV(d,a,b) do { register s32 in __asm__("$2") = (a); register s32 out __asm__("$7"); __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(out) : "r"(in), "r"(b) : "$8", "$9"); (d) = out; } while (0)

/**
 * @brief Per-frame update of a force-field object: every tick it computes one
 *        acceleration vector per target list slot and adds it to the velocity
 *        of the actors in range.
 *
 * Confirmed role
 * --------------
 * func_8003A844 is the message table of this object class; message 6, the tick
 * message, is forwarded here and its (void) result discarded. The function
 * therefore owns the whole per-frame behaviour of the field: phase timer,
 * force computation, velocity injection and the closing animation step.
 *
 * @param obj Field object: script byte at +0x0C, sound id at +0x8, flags at
 *        +0x24, model at +0x28, matrix at +0x2C, position at +0x50, last
 *        position at +0x5C, part index at +0x70, motion triple at +0xA0, the
 *        linked body at +0xAC, two force slots at +0x11C, and the parameter
 *        block from +0x134 to +0x144.
 * @param dt  Frame delta, subtracted from the phase timer and used as the final
 *        scale of the force (product shifted right by 4).
 *
 * @return Nothing.
 *
 * @par Confirmed phase timer
 * When bit 0x40 of +0x140 is set the timer at +0x13E counts down; below zero
 * bit 0x80 is toggled, the timer reloads from +0x138 (on) or +0x13A (off) and,
 * when the new phase is "on", both force slots at +0x11C are cleared. The field
 * only acts while (flags & 0x81) == 0x81.
 *
 * @par Confirmed target loop
 * for (i = D_8009FA0C - 1; i >= 0; i--) over D_8009FA58. An entry is skipped
 * when it is null or when bit 4 of the flags is set and the body's state at
 * sub+0xBC is not 0x10000; a skipped slot writes a zero force and rejoins the
 * loop at the accumulation step. Otherwise delta = position(+0x50) - target
 * position(+0x50), with y only included when bit 1 of the flags is set.
 *
 * @par Confirmed three modes (+0x13D)
 * 1 - radial: distance = func_8001A100(delta). From 4 upwards the vector is
 * normalized: recip = 0x80000000 / (distance >> 1) and each component times
 * recip shifted right by 16 is the unit direction in Q16 (the three multiplications
 * are inlined as one asm block). Below 4 the result depends on the sign of
 * +0x136: with a positive strength the force is zero, otherwise it degenerates
 * to (0x10000, 0x10000, 0) - a pure x/y push.
 * 2 - along z: only when (|delta.x| >> 4) <= +0x134; force.z = +-0x10000 from
 * the sign of delta.z, x and y zero, distance = |delta.z|.
 * 3 - along x: mirrored, gated by (|delta.z| >> 4) <= +0x134.
 * In all three modes a distance whose (distance >> 4) exceeds +0x134 zeroes the
 * force again, so +0x134 is the radius in 16-unit steps.
 *
 * @par Confirmed fall-off curve (+0x13C) and scaling
 * 0 - factor 0x10000; 1 - 0x10000 - distance * multiplier(+0x144); 2 - the same
 * value squared; 3 - distance * multiplier; 4 - that product squared. The factor
 * is then multiplied by the strength at +0x136 and by dt and shifted right by 4
 * (an inline asm block), and func_8001AB78 scales the force vector with it.
 *
 * @par Likely
 * func_8001A6F8(force, slot) writes the new force into the object's own slot
 * and reads the old one, so it almost certainly smooths or slews the value
 * rather than overwriting it; the slot is then clamped so that no component
 * exceeds the corresponding delta component (inlined CLAMP macro). The
 * remaining parameters are an attractor/repulsor pair: modes 2 and 3 push along
 * one axis only, which fits a conveyor or a wind volume.
 *
 * @par Unknown
 * The unit of +0x134 (shifted by 4 before the comparison, so 16 world units per
 * step) and the exact meaning of the multiplier at +0x144 (it scales the raw
 * distance, not the normalized one) are not provable from this file.
 *
 * @par Globals
 * D_8009FA0C at 0x8009FA0C - number of actor slots to walk.
 * D_8009FA58 at 0x8009FA58 - actor array; the body pointer used here is the
 * field at obj+0xAC of each actor, whose velocity sits at +0xE0/+0xE4/+0xE8.
 *
 * @par Control flow
 * 1. Optional script step (obj+0x0C -> func_8003A09C).
 * 2. Phase timer.
 * 3. If the "on" phase is active: walk the actors, compute forces, accumulate.
 *    Otherwise: if a sound is pending, play it and clear the latch.
 * 4. Optional motion block (obj+0xA0) with func_80050D14, then either the model
 *    part transform (+0x70) into +0x5C or a plain copy of +0x50 into +0x5C.
 *
 * @par Calls
 * func_8003A09C (script step), func_8001A100 (length), func_8001A6F8 (slot
 * accumulate), func_8001AB78 (scale force), func_800457B8 (sound),
 * func_80050D14 (motion step), func_80010750 and func_80010234 (matrix and part
 * transform).
 *
 * @par Side effects
 * Adds to the velocity of every actor in the list - i.e. it changes state owned
 * by other objects - writes the two force slots, the timer, the sound latches at
 * +0x21 and bit 0x400000 of +0x24, and rewrites the matrix and +0x5C when a
 * model is attached. No global variable is written; the actor array is only
 * read.
 *
 * @par Callers
 * func_8003A844, message 6 (analysis/edges.json, kind "direct").
 *
 * @par Environment path
 * Per-frame tick of a force-field object, called from message 6 of its own
 * handler func_8003A844. It runs the phase timer, computes one force vector
 * per player slot from the distance to the actor (radial, along z or along x
 * depending on the mode byte), scales it by the fall-off curve, the strength
 * and the frame delta, and adds the result to the velocity of every actor in
 * D_8009FA58. When the object owns a motion block it also steps it with
 * func_80050D14 and rewrites its own model frame vertex through func_80010750
 * and func_80010234. It is a physics unit that changes state owned by other
 * objects; no view, region or camera state is touched.
 */
void func_8003A160(Obj *obj, s32 dt)
{
    Vec force, delta;
    s32 i, scale;
    register s32 factor __asm__("$16");
    register s32 recip __asm__("$3");
    register s32 distance __asm__("$19");
    Obj *target;
    if (obj->padc) func_8003A09C(obj);
    if (obj->effectFlags & 0x40) {
        obj->timer -= dt;
        if (obj->timer < 0) {
            obj->effectFlags ^= 0x80;
            obj->timer = (obj->effectFlags & 0x80) ? obj->timeA : obj->timeB;
            if (obj->effectFlags & 0x80) {
                for (i = 0; i < 2; i++) {
                    obj->forces[i].x = 0;
                    obj->forces[i].y = 0;
                    obj->forces[i].z = 0;
                }
            }
        }
    }
    if ((obj->effectFlags & 0x81) == 0x81) {
        obj->soundPlaying = obj->soundType;
        for (i = gPlayerCount - 1; i >= 0; i--) {
            target = D_8009FA58[i];
            if (target != 0 && (!(obj->effectFlags & 4) || target->sub->state == 0x10000)) goto valid_target;
            force.z = 0;
            force.y = 0;
            __asm__ volatile("" ::: "memory");
            force.x = 0;
            goto smooth_force;
valid_target:
                delta.x = obj->position.x - target->position.x;
                delta.z = obj->position.z - target->position.z;
                delta.y = (obj->effectFlags & 2) ? obj->position.y - target->position.y : 0;
                switch (obj->mode) {
                case 1:
                    distance = func_8001A100(&delta);
                    if (distance >= 4) {
                        recip = 0x80000000U / (distance >> 1);
                        {
                            register s32 out __asm__("$7");
                            __asm__ volatile("lw $2,%3\n\tnop\n\tmult $2,%6\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor $7,$9,$8\n\tlw $2,%4\n\tsw $7,%0\n\tmult $2,%6\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor $7,$9,$8\n\tlw $2,%5\n\tsw $7,%1\n\tmult $2,%6\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %2,$9,$8" : "=m"(force.z), "=m"(force.x), "=r"(out) : "m"(delta.z), "m"(delta.x), "m"(delta.y), "r"(recip) : "$2", "$8", "$9");
                            force.y = out;
                        }
                    } else if (obj->strength > 0) {
                        force.x = 0;
                        goto force_yz_zero;
                    } else {
                        force.x = 0x10000;
                        force.y = 0x10000;
                        goto force_z_zero;
                    }
                    break;
                case 2:
                    distance = 0x40000000;
                    if ((ABS(delta.x) >> 4) <= obj->range) {
                        force.x = 0;
                        force.y = 0;
                        {
                            register s32 component __asm__("$4") = delta.z;
                            register s32 direction __asm__("$5") = -0x10000;
                            {
                                register s32 sign __asm__("$3") = component >> 31;
                                register s32 sum __asm__("$2") = sign + component;
                                distance = sum ^ sign;
                            }
                            if (component > 0) direction = 0x10000;
                            force.z = direction;
                        }
                    }
                    break;
                case 3:
                    distance = 0x40000000;
                    if ((ABS(delta.z) >> 4) <= obj->range) {
                        {
                            register s32 component __asm__("$4") = delta.x;
                            register s32 direction __asm__("$5") = -0x10000;
                            {
                                register s32 sign __asm__("$3") = component >> 31;
                                register s32 sum __asm__("$2") = sign + component;
                                distance = sum ^ sign;
                            }
                            if (component > 0) direction = 0x10000;
                            force.x = direction;
                        }
force_yz_zero:
                        force.y = 0;
force_z_zero:
                        force.z = 0;
                    }
                    break;
                }
                if ((distance >> 4) > obj->range) {
                    force.x = 0;
                    force.y = 0;
                    force.z = 0;
                } else {
                    switch (obj->curve) {
                    case 0: factor = 0x10000; break;
                    case 1: { register s32 product __asm__("$2"); FMULP(product, distance, obj->multiplier); factor = 0x10000 - product; break; }
                    case 2: { register s32 product __asm__("$2"); FMULP(product, distance, obj->multiplier); factor = 0x10000 - product; goto square_factor; }
                    case 3: FMULP(factor, distance, obj->multiplier); break;
                    case 4: FMULP(factor, distance, obj->multiplier);
square_factor:
                            FMUL(factor, factor, factor); break;
                    }
                    FMULP(factor, factor, obj->strength);
                    {
                        register Vec *fptr __asm__("$5");
                        __asm__ volatile("mult %0,%2\n\taddiu %1,$sp,0x10\n\tmflo %0\n\tsra %0,%0,4" : "+r"(factor), "=r"(fptr) : "r"(dt));
                        func_8001AB78(factor, fptr);
                    }
                }
smooth_force:
            func_8001A6F8(&force, &obj->forces[i]);
            if (obj->strength > 0) {
#define CLAMP(f,d) do { \
    register s32 value __asm__("$16") = (f); \
    register s32 cmp __asm__("$5") = (d); \
    register s32 absvalue __asm__("$4"); \
    __asm__ volatile("" : "+r"(value), "+r"(cmp)); \
    __asm__ volatile("sra $2,%1,31\n\taddu %0,$2,%1\n\txor %0,%0,$2" : "=r"(absvalue) : "r"(value) : "$2"); \
    if (absvalue > ABS(cmp)) (f) = cmp; \
} while (0)
                CLAMP(obj->forces[i].z, delta.z);
                CLAMP(obj->forces[i].x, delta.x);
                CLAMP(obj->forces[i].y, delta.y);
            }
            target->sub->velocity.x += obj->forces[i].x;
            target->sub->velocity.z += obj->forces[i].z;
            if (obj->effectFlags & 2) target->sub->velocity.y += obj->forces[i].y;
        }
    } else if (obj->soundType && (obj->flags & 0x400000)) {
        func_800457B8(obj->sound, obj->soundIndex);
        obj->soundPlaying = 0;
        obj->flags &= ~0x400000;
    }
    scale = 0;
    if (obj->motion[0]) scale = func_80050D14(obj->motion, dt, obj);
    if (scale) {
        if (obj->model) {
            gGteLoadMatrix(obj->transform);
            {
                register s32 part __asm__("$4");
                register char *parts __asm__("$2");
                register Vec *dest __asm__("$5") = &obj->lastPosition;
                register s32 one __asm__("$6") = 1;
                __asm__ volatile("lh %0,%2\n\tlw %1,%3\n\tsll %0,%0,4\n\tlw %1,0x34(%1)\n\taddiu %0,%0,8" : "=&r"(part), "=&r"(parts) : "m"(obj->part), "m"(obj->model), "r"(dest), "r"(one));
                gGteTransformVerticesWide(parts + part, dest, one);
            }
        } else obj->lastPosition = obj->position;
    }
}

