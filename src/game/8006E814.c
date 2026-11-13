#include "game/types.h"

struct Obj {
    u8 pad00[0x12];
    u16 field12;
    u8 pad14[6];
    u16 flags1a;
    u8 pad1c[0x56];
    u16 field72;
    u8 pad74[2];
    u16 field76, field78, field7a, field7c, field7e;
    u8 pad80[6];
    s16 field86;
    u8 pad88[0x10];
    u16 flags98, field9a;
    void *mesh;
};

extern s32 func_8003C7B8(s32, struct Obj *, s32, s32);
extern s32 func_8003C980(s32, struct Obj *, s32, s32);
extern void func_8006E720(struct Obj *);
extern void *func_8006C84C(s16, s32);
extern void func_8006CA68(void *);
extern void func_800181D4(void *);

s32 func_8006E814(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    s32 flags;
    if (func_8003C7B8(message, obj, arg2, arg3)) goto ret0;
    switch (message) {
    case 0:
        func_8006E720(obj);
        obj->field72 = 0;
        obj->field76 = 20;
        obj->field7c = obj->field78;
        obj->field7e = obj->field7a;
        obj->field9a = obj->field12;
        if (obj->flags98 & 0x40) {
            flags = (obj->flags98 & 0x100) ? 0x12 : 2;
            if (obj->flags98 & 0x80) flags |= 1;
            if (obj->flags1a & 0x104) flags |= 8;
            obj->mesh = func_8006C84C(0x100, (u8)flags);
            if (!obj->mesh) return 0;
        }
        return 1;
    case 10:
        if ((obj->flags98 & 0x40) && obj->mesh) func_8006CA68(obj->mesh);
        func_800181D4(obj);
        return 0;
    case 6:
        return obj->field86 == 0;
    case 1:
    case 9:
        goto ret0;
    default:
        return func_8003C980(message, obj, arg2, arg3);
    }
ret0:
    return 0;
}
