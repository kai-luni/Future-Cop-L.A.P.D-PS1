#include "game/types.h"

struct Object;
struct Child {
    u8 pad00[8];
    void *unk08;
    u8 pad0C[0xA];
    u8 type;
    u8 pad17[5];
    s16 unk1C;
    u8 pad1E[0x55];
    s8 count;
    u8 pad74[0xE4];
    struct Object *objects[1];
};
struct Object {
    u8 pad00[0xE8];
    u16 flags;
    u8 padEA[2];
    struct Child *child;
};
extern void func_80038754(struct Object *, void *, s32);
extern s32 func_8002AD58(s32, struct Object *, void *, s32);
extern void func_800395A8(s32, struct Child *, void *, s32);
extern void func_800398EC(s32, struct Child *, void *, s32);

s32 func_80039298(s32 message, struct Object *object, void *data, s32 arg3)
{
    register struct Object *obj __asm__("$5") = object;
    register struct Child *child __asm__("$9");
    register s32 i __asm__("$8");
    switch (message) {
    case 1:
        return 0xF0;
    case 2:
        func_80038754(obj, data, arg3);
    case 3:
    case 6:
        return 0;
    case 10:
        return obj->child != data;
    case 9:
        if (obj->flags & 1)
            break;
        {
            register s32 result __asm__("$2") = 0;
            __asm__("" : "+r"(result));
            return result;
        }
    case 5:
        child = obj->child;
        if (child->type == 0x1C) {
            if (child->unk08 != 0) {
                register s32 n __asm__("$2") = child->count;
                __asm__("" : "+r"(n));
                i = 0;
                if (n > 0) {
                    register s32 count __asm__("$10") = n;
                    do {
                        register struct Child *cur __asm__("$3") = child;
                        __asm__("" : "+r"(cur));
                        if (cur->objects[0] == obj)
                            goto clear;
                        i++;
                        child = (struct Child *)((u8 *)cur + 4);
                    } while (i < count);
                }
            }
        } else {
        clear:
            child->objects[0] = 0;
        }
        if (!(obj->flags & 1))
            obj->child->unk1C = -1;
        break;
    case 11:
    case 14:
        if (obj->flags & 1)
            break;
        {
            register struct Child *handler __asm__("$5") = obj->child;
            if (handler->type == 0x14)
                goto type20;
            if (handler->type == 0x1C)
                goto type28;
            {
                register s32 result __asm__("$2") = 0;
                __asm__("" : "+r"(result));
                return result;
            }
        type20:
            func_800395A8(message, handler, data, arg3);
            return 0;
        type28:
            func_800398EC(message, handler, data, arg3);
            return 0;
        }
    }
    return func_8002AD58(message, obj, data, arg3);
}
