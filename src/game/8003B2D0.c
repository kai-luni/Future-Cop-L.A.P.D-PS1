#include "game/types.h"

struct Unk3B2D0 {
    u8 pad00[0x28];
    u8 unk28[0x68];
    u32 flags90;
    u8 pad94[0xC];
    u8 unkA0;
    u8 padA1[7];
    u8 unkA8[0x20];
    s32 unkC8;
};

extern void func_80013AA0(s32 low, s32 high);
extern s32 func_800175A0(void *a, s32 b, s32 c, void *d);
extern void func_800181D4(s32 block);
extern s32 func_8003AA94(s32 message, struct Unk3B2D0 *object);
extern void func_8003B144(s32 message, struct Unk3B2D0 *object);

s32 func_8003B2D0(s32 message, struct Unk3B2D0 *object)
{
    register u32 m __asm__("$17");

    switch (message) {
    case 1:
        return 0xCC;
    case 2:
        return func_8003AA94(message, object);
    case 6:
        func_8003B144(message, object);
        return 0;
    case 3:
        m = 0xFFFF0000;
        if ((object->flags90 & m) != 0) {
            func_80013AA0(((u8 *)object)[0x92], ((u8 *)object)[0x93]);
        }
        func_800175A0(object->unk28, object->unkA0, object->unkC8, object->unkA8);
        if ((object->flags90 & m) != 0) {
            func_80013AA0(0, 0);
            return 0;
        }
        return 0;
    case 5:
        if (object->unkC8 != 0) {
            func_800181D4(object->unkC8);
        }
        return 0;
    case 16:
        return 1;
    default:
        return 0;
    }
}
