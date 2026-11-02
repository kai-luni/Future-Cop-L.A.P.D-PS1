#include "game/types.h"

struct Unk3BF50Object {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0xA9];
    struct Unk3BF50Object *unkC0;
    u8 unkC4[4];
    struct Unk3BF50Object *unkC8;
};

s32 func_8003BF50(struct Unk3BF50Object *object)
{
    struct Unk3BF50Object *other;
    s32 result;

    if (object != 0) {
        switch (object->kind) {
        case 1:
        case 0x12:
            return 1;
        case 7:
            other = object->unkC0;
            result = 0;
            goto check;
        case 2:
        case 0xF:
            other = object->unkC8;
            result = 0;
        check:
            if (other != 0) {
                result = other->kind == 0x12;
            }
            return result;
        default:
            return 0;
        }
    }
    return 0;
}
