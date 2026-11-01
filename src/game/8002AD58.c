#include "game/types.h"

struct Unk2AD58 {
    u8 unk00[0x24];
    u32 flags;
    u8 unk28[0x50];
    s32 unk78;
    u8 unk7C[4];
    s32 unk80;
};

extern void func_8002A5F4(struct Unk2AD58 *object, void *data);
extern s32 func_8002ACB4(struct Unk2AD58 *object, s32 value);
extern s32 func_8002A810(struct Unk2AD58 *object, s32 arg1, s32 arg2);
extern s32 func_8002A940(struct Unk2AD58 *object, void *data);
extern s32 func_8002AA1C(s32 message, struct Unk2AD58 *object, void *hit);
extern s32 func_8002AB8C(struct Unk2AD58 *object, void *data);
extern s32 func_80095C30(s32 x, s32 y);

s32 func_8002AD58(s32 message, struct Unk2AD58 *object, void *data)
{
    switch (message) {
    case 1:
        return 0xA0;
    case 2:
        func_8002A5F4(object, data);
        return 0;
    case 3:
        if (object->flags & 0x20) {
            return 0;
        }
        return func_8002ACB4(object, 1);
    case 4:
        return func_8002A810(object, 1, 1);
    case 5:
    case 6:
    case 7:
    case 8:
    case 13:
        return 0;
    case 9:
        return func_8002A940(object, data);
    case 11:
    case 14:
        return func_8002AA1C(message, object, data);
    case 12:
        return func_8002AB8C(object, data);
    case 10:
    case 16:
        return 1;
    case 15:
        return func_80095C30(object->unk78, object->unk80) & 0xFFF;
    default:
        return -1;
    }
}
