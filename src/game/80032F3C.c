#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk32F3C {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x61];
    struct Vec3 unk78;
    u8 unk84[0x58];
    struct Vec3 unkDC;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    u8 unkF8[2];
    u8 unkFA;
};

extern u32 func_80019668(void);
extern void func_8001AB78(s32 arg0, void *arg1);
extern s32 func_80032180(u32 arg0, void *arg1, s32 arg2, s32 arg3);
extern s32 func_800324E0(s32 arg0, void *arg1, s32 arg2);
extern s32 func_8002AD58(u32 message, void *object, s32 arg2, s32 arg3);

s32 func_80032F3C(u32 arg0, struct Unk32F3C *obj, s32 arg2, s32 arg3)
{
    register s32 c25 __asm__("$18");
    register s32 c3 __asm__("$19");
    register s32 c5 __asm__("$21");

    switch (arg0) {
    case 1:
        return 0x134;
    case 2:
        return func_80032180(2, obj, arg2, arg3);
    case 6:
        return func_800324E0(6, obj, arg2);
    case 4:
        c5 = 5;
        if (obj->unkFA == c5) {
            goto dflt;
        }
        c3 = 3;
        if (obj->unkFA == c3) {
            goto ret0;
        }
        c25 = 0x25;
        if (obj->unk16 == c25 || (func_80019668() & 0xF) < 0xA) {
            obj->unkFA = c3;
            obj->unkDC = obj->unk78;
            func_8001AB78(obj->unkF4, &obj->unkDC);
            obj->unkE8 = (func_80019668() & 0xFFF) - 0x800;
            obj->unkEC = (func_80019668() & 0xFFF) - 0x800;
            obj->unkF0 = (func_80019668() & 0xFFF) - 0x800;
            goto ret0;
        }
        if (obj->unk16 == c25) {
            obj->unkFA = c5;
        ret0:
            return 0;
        }
        break;
    }
dflt:
    return func_8002AD58(arg0, obj, arg2, arg3);
}
