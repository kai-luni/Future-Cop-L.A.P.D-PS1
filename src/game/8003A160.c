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
extern int D_8009FA0C;
extern Obj *D_8009FA58[];
extern void func_8003A09C(Obj *);
extern s32 func_8001A100(Vec *);
extern void func_8001A6F8(Vec *, Vec *);
extern void func_8001AB78(s32, Vec *);
extern void func_800457B8(s32, s32);
extern int func_80050D14(void *, s32, Obj *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, int);
extern void func_80039E8C(int);
extern int func_8002AD58(int, void *, void *);
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
#define FMULP(d,a,b) do { register s32 mul __asm__("$2") = (b); FMUL(d,a,mul); } while (0)
#define FMULV(d,a,b) do { register s32 in __asm__("$2") = (a); register s32 out __asm__("$7"); __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(out) : "r"(in), "r"(b) : "$8", "$9"); (d) = out; } while (0)

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
        for (i = D_8009FA0C - 1; i >= 0; i--) {
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
            func_80010750(obj->transform);
            {
                register s32 part __asm__("$4");
                register char *parts __asm__("$2");
                register Vec *dest __asm__("$5") = &obj->lastPosition;
                register s32 one __asm__("$6") = 1;
                __asm__ volatile("lh %0,%2\n\tlw %1,%3\n\tsll %0,%0,4\n\tlw %1,0x34(%1)\n\taddiu %0,%0,8" : "=&r"(part), "=&r"(parts) : "m"(obj->part), "m"(obj->model), "r"(dest), "r"(one));
                func_80010234(parts + part, dest, one);
            }
        } else obj->lastPosition = obj->position;
    }
}

