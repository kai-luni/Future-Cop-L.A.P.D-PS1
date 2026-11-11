#include "game/types.h"

struct G59ABC {
    u8 pad00[0x5C];
    s32 unk5C;
};

struct C59ABC {
    u8 pad00[0x24];
    u32 flags;
};

struct O59ABC {
    u8 pad00[0xB4];
    struct C59ABC *unkB4;
};

extern struct G59ABC *D_8009FA94;

extern s32 func_8002AD58(s32 message, struct O59ABC *object, s32 data);
extern s32 func_800584BC(s32 message, struct C59ABC *child, s32 data, s32 arg3, s32 arg4);
extern void func_8005920C(struct O59ABC *object, s32 a, s32 b);
extern void func_800597C0(struct O59ABC *object);

s32 func_80059ABC(s32 message, struct O59ABC *object, s32 data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB8;
    case 2:
        func_8005920C(object, data, arg3);
    case 6:
    case 9:
zero:
        return 0;
    case 10:
        return (s32)object->unkB4 != data;
    case 3:
        if (object->unkB4->flags & 0x20) {
            goto zero;
        }
        func_800597C0(object);
        return 5;
    case 11:
    case 14:
        return func_800584BC(message, object->unkB4, data, arg3, 1);
    case 4:
        if (D_8009FA94->unk5C == 1) {
            goto zero;
        }
    default:
        return func_8002AD58(message, object, data);
    }
}
