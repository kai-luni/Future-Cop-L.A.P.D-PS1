#include "game/types.h"

struct UnkDC {
    u8 pad00[0x24];
    u32 flags;
};

struct Unk5908C {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x10];
    u16 unk1C;
    u8 pad1E[0x8E];
    void *unkAC;
    u8 padB0[0x14];
    void *unkC4;
    u8 padC8[0x14];
    struct UnkDC *unkDC;
};

struct Unk5908CData {
    u8 pad00[8];
    struct Unk5908C *unk08;
};

extern s32 func_800579D4(struct Unk5908C *object, struct Unk5908CData *data, s32 arg3);
extern s32 func_800588D4(struct Unk5908C *object, struct Unk5908CData *data);
extern void func_80058D78(struct Unk5908C *object);
extern void func_800181D4(void *arg0);
extern s32 func_8002AD58(s32 message, struct Unk5908C *object, struct Unk5908CData *data);
extern void func_80058378(struct Unk5908C *object, s32 amount, s32 source);
extern s32 func_800584BC(s32 message, struct Unk5908C *object, struct Unk5908CData *hit,
                         s32 arg3, s32 arg4);

s32 func_8005908C(s32 message, struct Unk5908C *object, struct Unk5908CData *data, s32 arg4)
{
    struct Unk5908C *s0;
    struct Unk5908CData *s1;
    s32 result;
    s32 flag;

    s0 = object;
    s1 = data;

    switch (message) {
    case 1:
        return 0xF4;
    case 2:
        func_800579D4(s0, s1, arg4);
        return 0;
    case 3:
        return 0;
    case 10:
        flag = 0;
        if (s0->unkC4 != s1) {
            flag = (s0->unkDC != s1);
        }
        return flag;
    case 6:
        return func_800588D4(s0, s1);
    case 4:
        func_80058D78(s0);
        return 0;
    case 5:
        if (s0->unkAC != 0) {
            func_800181D4(s0->unkAC);
            s0->unkAC = 0;
        }
        return 0;
    case 9:
        if (s1->unk08 == s0) {
            return 0;
        }
        result = func_8002AD58(message, s0, s1);
        if (result <= 0) {
            return 0;
        }
        s0->unkDC->flags |= 0x02000000;
        s0->unk1C += result;
        func_80058378(s0, result, s1->unk08 != 0 ? s1->unk08->unk08 : 0);
        return 0;
    case 11:
    case 14:
        return func_800584BC(message, s0, s1, arg4, 0);
    case 7:
    case 8:
    case 12:
    case 13:
    default:
        return func_8002AD58(message, s0, s1);
    }
}
