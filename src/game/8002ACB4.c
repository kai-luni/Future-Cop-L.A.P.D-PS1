#include "game/types.h"

struct Unk17508 {
    void *model;
};

union Unk90 {
    u32 word;
    struct {
        u8 pad[2];
        u8 unk92;
        u8 unk93;
    } bytes;
};

struct Unk2ACB4 {
    u8 unk00[0x24];
    u32 flags;
    struct Unk17508 inst;
    u8 unk2C[0x90 - 0x2C];
    union Unk90 u90;
};

extern void func_80013AA0(s32 low, s32 high);
extern s32 func_80017508(struct Unk17508 *instance, u32 *flags, s32 arg2);

s32 func_8002ACB4(struct Unk2ACB4 *object, s32 value)
{
    s32 result;

    if (object->inst.model != 0) {
        register u32 m __asm__("$18");
        m = 0xFFFF0000;

        if ((object->u90.word & m) != 0) {
            func_80013AA0(object->u90.bytes.unk92, object->u90.bytes.unk93);
        }
        result = func_80017508(&object->inst, &object->flags, value & 0xFF);
        if ((object->u90.word & m) != 0) {
            func_80013AA0(0, 0);
        }
        return result & 0xFF;
    }
    return 0;
}
