#include "game/types.h"

struct Inner34 {
    u8 pad00[0x34];
    u8 *unk34;
};

struct Obj3015C {
    u8 pad00[8];
    s32 unk8;
    u8 pad0C[0x1C - 0x0C];
    s16 unk1C;
    u8 pad1E[0x24 - 0x1E];
    u32 unk24;
    struct Inner34 *unk28;
    u8 pad2C[0x70 - 0x2C];
    s16 unk70;
    u8 pad72[0xC0 - 0x72];
    u8 unkC0;
    u8 padC1[0xC4 - 0xC1];
    s16 unkC4;
    u16 unkC6;
    u32 unkC8;
    u8 padCC[0xD4 - 0xCC];
    s32 unkD4;
};

struct Arg3015C {
    u8 pad00[0x16];
    u8 unk16;
};

extern s32 D_8009FAA4;

extern void func_8002FA20(struct Obj3015C *obj, struct Arg3015C *arg, s32 arg3);
extern void func_80030060(struct Obj3015C *obj, struct Arg3015C *arg);
extern s32 func_8002F8B8(u8 arg0, s16 arg1, struct Arg3015C *arg2, u8 arg3);
extern s32 func_8002FF6C(struct Obj3015C *obj, struct Arg3015C *arg);
extern s32 func_800188D0(s32 a, s32 b, s32 c, void *d);
extern void func_80010750(void *a);
extern void func_80010234(void *a, void *b, s32 c);
extern s32 func_8002AD58(s32 message, struct Obj3015C *object, struct Arg3015C *data);

s32 func_8003015C(s32 cmd, struct Obj3015C *obj, struct Arg3015C *arg, s32 arg3)
{
    switch (cmd) {
    case 1:
        return 0xD8;
    case 2:
        func_8002FA20(obj, arg, arg3);
        return 0;
    case 10:
        if (arg->unk16 == 0x25 && obj->unk1C >= 0) {
            func_80030060(obj, arg);
            return 0;
        }
        return func_8002F8B8(obj->unkC0, obj->unkC4, arg, 0) & 0xFF;
    case 11:
        if (obj->unk8 == 0) {
            return 0;
        }
        return func_8002FF6C(obj, arg);
    case 6:
        if (obj->unkD4 != 0) {
            s32 val;
            val = (obj->unkC6 + (s32)arg * obj->unkD4) & 0xFFF;
            obj->unkC6 = val;
            func_800188D0(val, 0, 0, (u8 *)obj + 0x2C);
            func_80010750((u8 *)obj + 0x2C);
            func_80010234(obj->unk28->unk34 + ((obj->unk70 << 4) + 8),
                          (u8 *)obj + 0x5C, 1);
            if ((obj->unkC8 & 0xF0) && (D_8009FAA4 & 0x80)) {
                obj->unk24 |= 0x2000000;
            }
        }
        return 0;
    default:
        return func_8002AD58(cmd, obj, arg);
    }
}
