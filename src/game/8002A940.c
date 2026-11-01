#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

struct Unk2A940Source {
    u8 unk00[8];
    u32 unk08;
};

struct Unk2A940Arg2 {
    u32 unk00;
    u8 unk04[4];
    struct Unk2A940Source *unk08;
    u8 unk0C[2];
    u16 unk0E;
};

struct Unk2A940 {
    u8 unk00[0x1C];
    s16 unk1C;
    u8 unk1E[2];
    u8 unk20;
    u8 unk21[3];
    u32 unk24;
    u8 unk28[0x64];
    u32 unk8C;
    u8 unk90[4];
    void *unk94;
};

extern void func_8003BA50(struct Unk3BObject *object, s32 value);
extern void func_80069D68(void *arg0, s32 arg1, s32 arg2, struct Unk2A940 *arg3);

s32 func_8002A940(struct Unk2A940 *object, struct Unk2A940Arg2 *arg)
{
    s32 delta;
    s32 previous;
    u16 value;

    delta = arg->unk0E;
    if (delta <= 0) {
        return delta;
    }

    previous = object->unk1C;
    object->unk1C = (u16)object->unk1C - delta;

    if (object->unk1C < 0) {
        if (arg->unk08 != 0) {
            object->unk8C = arg->unk08->unk08;
        }
    } else {
        if (object->unk20 != 0 && (object->unk24 & 0x40000) == 0) {
            func_8003BA50((struct Unk3BObject *)object, previous);
        }
    }

    object->unk24 |= 0x02000000;
    if (object->unk94 != 0) {
        func_80069D68(object->unk94,
                      (u32)object->unk1C >> 31,
                      arg->unk00 & 0x7FFF,
                      object);
    }

    return delta;
}
