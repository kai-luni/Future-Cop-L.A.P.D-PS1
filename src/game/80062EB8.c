#include "game/types.h"

struct Object {
    u8 pad00[4];
    void (*callback)(s32, struct Object *, s32, s32);
    u8 pad08[0x1c];
    u32 flags;
    u8 data[0x4a];
    u8 flags72;
};
extern s32 D_8009F828[];
extern struct Object **D_8009F818[];
extern void func_800164F4(void *);

void func_80062EB8(s32 index)
{
    s32 count;
    struct Object **cursor;
    struct Object *object;
    register s32 offset __asm__("$2");
    register s32 arg __asm__("$4");

    offset = index * 4;
    __asm__("" : "+r" (offset));
    count = *(s32 *)((u8 *)D_8009F828 + offset);
    cursor = *(struct Object ***)((u8 *)D_8009F818 + offset);
    if (count > 0) {
      arg = 3;
      do {
        __asm__("" : "+r" (arg));
        object = *cursor++;
        count--;
        object->callback(arg, object, 0, 0);
        arg = 3;
      } while (count > 0);
    }
    offset = index * 4;
    __asm__("" : "+r" (offset));
    count = *(s32 *)((u8 *)D_8009F828 + offset);
    cursor = *(struct Object ***)((u8 *)D_8009F818 + offset);
    while (count > 0) {
        object = *cursor;
        if (!(object->flags & 0x20) && (object->flags72 & 0x10)) {
            func_800164F4(object->data);
        }
        count--;
        cursor++;
    }
}
