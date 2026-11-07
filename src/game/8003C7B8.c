#include "game/types.h"

struct Parent { u8 pad[0x20]; u32 x, y; u8 pad28[2]; s16 z; };
struct Counter { u8 pad[0x3a]; u16 a; u8 pad3c[2]; u16 b; };
struct Obj { u8 pad[4]; s16 field04; u8 pad06[2]; s16 field08, field0a, field0c; u8 pad0e; u8 flags0f, flags10; u8 pad11[3]; s16 field14, field16; u8 pad18[4]; struct Counter *counter; struct Parent * volatile parent; };
extern s32 D_8009FA28, D_8009F9F0, D_8009F9C0, D_8009FA90;

s32 func_8003C7B8(s32 message, struct Obj *obj)
{
    s32 z;
    if (message == 1) goto case1;
    if (message == 0) goto case0;
    if (message == 10) goto case10;
    goto ret0;
case0:
        if ((u32)(((s32)((D_8009FA28 - D_8009F9F0) * 0xC4EC4EC5) >> 2)) / 52 - 50 < obj->field0a) return 1;
        if ((u32)(((s32)((D_8009F9C0 - D_8009FA90) * 0xB6DB6DB7) >> 2)) / 4 / 7 - 50 < obj->field08) return 1;
        obj->field16 = obj->parent->x * 75 >> 14;
        obj->field04 = obj->parent->y * 75 >> 14;
        __asm__ volatile ("" ::: "memory");
        z = obj->parent->z;
        obj->field0c = obj->field16;
        obj->flags0f = 0;
        obj->field14 = (u32)(z * 75) >> 10;
        goto ret0;
case1:
        obj->flags0f &= 11;
        goto ret0;
case10:
        if (obj->counter) {
            obj->counter->a--;
            if (!(obj->flags10 & 4)) obj->counter->b--;
        }
ret0:
    return 0;
}
