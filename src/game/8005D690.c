#include "game/types.h"
typedef struct Obj Obj;
typedef struct Entry Entry;
struct Obj { u8 pad0[0x16]; u8 kind; u8 pad17; u8 flags; u8 pad19[0x93]; void *data; };
struct Entry { Obj *obj; Obj *owner; u8 pad08[4]; s32 time; s32 value; u8 pad14[0x38]; s16 low; s16 high; s16 other; u16 flags; s32 index; };
typedef struct { s16 other, low, high; } Params;
typedef struct { u8 pad[0xA]; s16 value, other, low, high; } Extra;
extern Entry *D_800A22F0[4];
extern s32 D_8009FAA4;
extern Entry *func_8005D3FC(Obj *, s32);
s32 func_8005D690(Obj *arg0, Params *arg1, Extra *arg2)
{
    register Obj *obj __asm__("$17") = arg0;
    register Params *params __asm__("$18") = arg1;
    register Extra *extra __asm__("$19") = arg2;
    s32 result = 0;
    Entry *e;
    s32 i;
    register Obj *o __asm__("$4");
    register u8 *base __asm__("$3");
    register s32 idx __asm__("$2");
    __asm__("" : "+r"(result));
    e = 0;
    for (i = 0; i < 4; i++) {
        if (D_800A22F0[i] && D_800A22F0[i]->owner == obj) {
            e = D_800A22F0[i];
            break;
        }
    }
    if (!e) {
        e = func_8005D3FC(obj, 0);
        if (!e) return 0;
        e->low = params->low;
        e->high = params->high;
        if (e->high < e->low) e->high = (e->low * 8) / 5;
        e->other = params->other;
    }
    if (obj->kind == 1 && extra) {
        o = obj;
        if (D_8009FAA4 > e->time) {
            if (e->flags & 1) e->flags &= ~1;
            else e->flags |= 1;
        }
        __asm__("" : "+r"(o));
        e->time = D_8009FAA4 + 60;
        if (e->flags & 1) {
            e->value = extra->value;
            __asm__("" : : : "memory");
            idx = e->index;
            __asm__("" : "+r"(idx));
            base = (u8 *)o->data + idx * 2;
            __asm__("" : "+r"(base));
            if (*(s16 *)(base + 0x8A) > 0) {
                e->flags |= 4;
                e->obj->flags &= ~4;
            } else {
                e->flags &= ~4;
                e->obj->flags |= 4;
            }
            e->low = extra->low;
            e->high = extra->high;
            if (e->high < e->low) e->high = (e->low * 8) / 5;
            e->other = extra->other;
            result = 1;
        } else result = 0;
    } else {
        e->time = D_8009FAA4 + 120;
        e->flags |= 1;
    }
    return result;
}
