#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Tag { u8 pad[4]; u8 type; };
struct Moving { u8 pad[0x78]; struct Vec velocity; s32 speed; };
struct Aim { u8 pad[0x5c]; struct Vec pos; };
struct Target { u8 pad0[4]; struct Aim *aim; struct Moving *moving; u8 padc[0xc]; s32 health; u8 pad1c[0xc]; s16 strength; u8 flags; };
struct Transform { u8 pad[4]; struct Vec axis0; struct Vec axis1; struct Vec axis2; };
struct Mid { u8 pad0[0x34]; struct Transform *transform; u8 pad38[0x10]; struct Target **target; };
struct Obj { u8 pad0[0xf]; u8 flagsF, flags10; u8 pad11[0xb]; struct Mid *mid; struct Tag *tag; s32 (*handler)(s32, struct Obj *, s32, s32); u8 pad28[0x66]; u16 flags8E; u8 pad90[0xa]; s16 x, y, z; };
extern void func_8001A180(struct Vec *);
extern s32 func_8001AB00(struct Vec *, struct Vec *);
extern void func_8005B6A8(struct Obj *);
extern u32 func_80095900(s32);
extern u16 D_8009F7F0, D_8009F7F2, D_8009F7F4;
extern s16 data_return[3];

void *func_8005B934(u32 msg, struct Obj *obj, s32 arg2, s32 arg3)
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
        root = func_80095900(a + shifted);
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
            func_8005B6A8(obj);
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
                func_8001A180(&v);
                product = (func_8001AB00(&v, &tr->axis2) >> 4) * t->strength;
                shifted = product >> 12;
                D_8009F7F4 = shifted;
                product = (func_8001AB00(&v, &tr->axis1) >> 4) * t->strength;
                shifted = product >> 12;
                D_8009F7F2 = shifted;
                product = (func_8001AB00(&v, &tr->axis0) >> 4) * t->strength;
                shifted = product >> 12;
                D_8009F7F0 = shifted;
                __asm__ volatile("" : : "r"(shifted) : "$4", "$5", "memory");
                product = (func_8001AB00(&t->moving->velocity, &tr->axis2) >> 4) * scale;
                shifted3 = product >> 12;
                out = (u16)D_8009F7F4 + shifted3;
            } else {
                product = (func_8001AB00(&t->moving->velocity, &tr->axis2) >> 4) * scale;
                D_8009F7F2 = 0;
                D_8009F7F0 = 0;
                shifted = product >> 12;
                D_8009F7F4 = shifted;
                __asm__ volatile("" : : : "memory");
                out = shifted + (u16)t->strength;
            }
        } else {
            D_8009F7F0 = 0;
            D_8009F7F2 = 0;
            __asm__ volatile("" : : : "memory");
            out = (u16)t->strength;
        }
        D_8009F7F4 = out;
        return data_return;
    }
    return 0;
}
