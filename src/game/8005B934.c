// Categories: Gameplay > Combat | Physics > Collision | Graphics > Effects
#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Tag { u8 pad[4]; u8 type; };
struct Moving { u8 pad[0x78]; struct Vec velocity; s32 speed; };
struct Aim { u8 pad[0x5c]; struct Vec pos; };
struct Target { u8 pad0[4]; struct Aim *aim; struct Moving *moving; u8 padc[0xc]; s32 health; u8 pad1c[0xc]; s16 strength; u8 flags; };
struct Transform { u8 pad[4]; struct Vec axis0; struct Vec axis1; struct Vec axis2; };
struct Mid { u8 pad0[0x34]; struct Transform *transform; u8 pad38[0x10]; struct Target **target; };
struct Obj { u8 pad0[0xf]; u8 flagsF, flags10; u8 pad11[0xb]; struct Mid *mid; struct Tag *tag; s32 (*handler)(s32, struct Obj *, s32, s32); u8 pad28[0x66]; u16 flags8E; u8 pad90[0xa]; s16 x, y, z; };
extern void gNormalizeVector(struct Vec *) __asm__("func_8001A180");
extern s32 gVectorDot(struct Vec *, struct Vec *) __asm__("func_8001AB00");
extern void gTraceWeaponSphereRays(struct Obj *) __asm__("func_8005B6A8");
extern u32 gSqrt12(s32) __asm__("func_80095900");
extern u16 D_8009F7F2, D_8009F7F4;
extern u16 gMessageReturnData __asm__("D_8009F7F0");
extern s16 data_return[3];
void *gHandleTargetDamageEffect(u32 msg, struct Obj *obj, s32 arg2, s32 arg3) __asm__("func_8005B934");


/**
 * @brief Four-message callback for kind-0x57 beam/tentacle effect nodes handling radius strength, hit ticks, target marking, and aim offsets.
 *
 * Confirmed: registered as D_8005B934 (func_8005B934) at node +0x24 by func_8005BDE4. Requires tag->type == 0x57 for msgs 1 and 3: msg 3 clears flags10 bit 2, updates flags8E ((flags8E & ~8) | 1), and stores func_80095900((x*x>>8)+(y*y>>8)+(z*z>>8))>>2 into (*mid->target)->strength; msg 1 decrements target->health by arg2 (retiring via obj->handler(8) on < 0), sets target->flags |= 2 and obj->flagsF |= 1 if moving == 0, else runs func_8005B6A8; msg 2 sets target->flags |= 1; msg 4 (arg3 == 1) computes projected offsets into D_8009F7F0/F2/F4 (data_return) via func_8001A180/func_8001AB00 and returns data_return. Category follows tag 0x57, func_8005B6A8 area hit, and D_800A22A0 effect pool.
 * Likely: beam/tether effect controller called by pool/segment managers. Unknown: exact indirect call sites of msgs 1..4.
 *
 * @param msg Selector 1 (tick/damage), 2 (mark flag 1), 3 (recompute strength), or 4 (compute transform offset).
 * @param obj Kind-0x57 owner (+0x0F/+0x10/+0x8E flags, +0x1C mid, +0x20 tag, +0x24 handler, +0x9A..+0x9E vector).
 * @param arg2 Damage step (msg 1) or base Vec3 pointer (msg 4).
 * @param arg3 Must equal 1 for msg 4 to compute and return data_return.
 * @return Pointer to D_8009F7F0 (data_return) on msg 4 with arg3 == 1, otherwise 0.
 * @par Globals
 * D_8009F7F0/D_8009F7F2/D_8009F7F4 (data_return) - three s16 offsets written and returned by msg 4.
 * @par Control flow
 * Switch on msg 1..4; msg 1/3 guard on tag->type == 0x57, msg 4 guards on arg3 == 1 and mid->transform/moving/aim pointers.
 * @par Calls
 * func_80095900 (isqrt in msg 3), obj->handler (indirect msg 8 in msg 1), func_8005B6A8 (sphere damage/raycast in msg 1), func_8001A180 and func_8001AB00 (msg 4).
 * @par Side effects
 * Mutates obj flags, (*obj->mid->target) health/strength/flags, and D_8009F7F0..D_8009F7F4.
 * @par Callers
 * Confirmed: none direct; address taken as D_8005B934 in func_8005BDE4 (0x8005BF98, stored in D_800A22A0 node +0x24) / Likely: indirect callers via node +0x24 / Unknown: specific indirect call sites.
 */
void *gHandleTargetDamageEffect(u32 msg, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Target *t;
    register struct Target *t1 __asm__("$5");
    struct Transform *tr;
    struct Mid *mid;
    struct Vec v;
    register s32 a __asm__("$4");
    register s32 b __asm__("$9");
    register s32 c __asm__("$5");
    register s32 flags __asm__("$3");
    register s32 zeroarg __asm__("$6");
    s32 (*handler)(s32, struct Obj *, s32, s32);
    s32 scale;
    register s32 product __asm__("$8");
    register s32 shifted __asm__("$2");
    register s32 shifted3 __asm__("$3");
    register s32 out __asm__("$2");
    u32 root;

    switch (msg) {
    case 3:
        if (obj->tag->type != 0x57) return 0;
        a = obj->x * obj->x;
        b = obj->y * obj->y;
        c = obj->z * obj->z;
        flags = *(volatile u8 *)&obj->flags10;
        flags &= ~4;
        *(volatile u8 *)&obj->flags10 = flags;
        *(volatile u16 *)&obj->flags8E = (*(volatile u16 *)&obj->flags8E & ~8) | 1;
        a >>= 8;
        shifted = b >> 8;
        a += shifted;
        shifted = c >> 8;
        root = gSqrt12(a + shifted);
        (*obj->mid->target)->strength = root >> 2;
        break;
    case 1:
        if (obj->tag->type != 0x57) return 0;
        t1 = *obj->mid->target;
        if (t1->health >= 0) {
            t1->health -= arg2;
            if (t1->health < 0) {
                t1->flags |= 2;
                if (obj != 0) {
                    handler = obj->handler;
                    __asm__("addu %0,$zero,$zero" : "=r"(zeroarg) : "r"(handler));
                    handler(8, obj, zeroarg, zeroarg);
                }
                return 0;
            }
        }
        if (t1->moving == 0) {
            t1->flags |= 2;
            obj->flagsF |= 1;
        } else {
            gTraceWeaponSphereRays(obj);
        }
        break;
    case 2:
        (*obj->mid->target)->flags |= 1;
        break;
    case 4:
        __asm__ volatile("" : : : "$2");
        if (arg3 != 1) return 0;
        mid = obj->mid;
        t = *mid->target;
        tr = 0;
        if (mid != 0) tr = mid->transform;
        if (t->moving != 0 && tr != 0) {
            scale = t->moving->speed << 4;
            if (t->aim != 0) {
                v.x = t->aim->pos.x - ((struct Vec *)arg2)->x;
                v.y = t->aim->pos.y - ((struct Vec *)arg2)->y;
                v.z = t->aim->pos.z - ((struct Vec *)arg2)->z;
                gNormalizeVector(&v);
                product = (gVectorDot(&v, &tr->axis2) >> 4) * t->strength;
                shifted = product >> 12;
                D_8009F7F4 = shifted;
                product = (gVectorDot(&v, &tr->axis1) >> 4) * t->strength;
                shifted = product >> 12;
                D_8009F7F2 = shifted;
                product = (gVectorDot(&v, &tr->axis0) >> 4) * t->strength;
                shifted = product >> 12;
                gMessageReturnData = shifted;
                __asm__ volatile("" : : "r"(shifted) : "$4", "$5", "memory");
                product = (gVectorDot(&t->moving->velocity, &tr->axis2) >> 4) * scale;
                shifted3 = product >> 12;
                out = (u16)D_8009F7F4 + shifted3;
            } else {
                product = (gVectorDot(&t->moving->velocity, &tr->axis2) >> 4) * scale;
                D_8009F7F2 = 0;
                gMessageReturnData = 0;
                shifted = product >> 12;
                D_8009F7F4 = shifted;
                __asm__ volatile("" : : : "memory");
                out = shifted + (u16)t->strength;
            }
        } else {
            gMessageReturnData = 0;
            D_8009F7F2 = 0;
            __asm__ volatile("" : : : "memory");
            out = (u16)t->strength;
        }
        D_8009F7F4 = out;
        return data_return;
    }
    return 0;
}
