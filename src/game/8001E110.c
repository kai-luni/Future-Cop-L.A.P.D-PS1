#include "game/types.h"

typedef struct {
    volatile s32 field0;
    u8 unk04[4];
    s32 field8;
    u8 unk0C[0xC];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];
extern s32 D_8009F258[];
extern s32 (*D_8009F230)(s32, s32);

extern s32 func_8001D424(s32 arg0, s32 arg1, void *arg2);
extern s32 func_8001D5E4(s32 arg0, s32 arg1);
extern s32 func_8001E004(s32 arg0, s32 arg1, s32 *arg2);

s32 func_8001E110(s32 arg0, s32 arg1, s32 *arg2, s32 *arg3, s32 arg4)
{
    s32 index;
    s32 value;
    s32 ret;

    if (arg1 < 0) {
        arg1 = -arg1;
        index = func_8001D424(arg0, arg1, D_8009F258);
    } else {
        index = func_8001D5E4(arg0, arg1);
    }

    if ((u32)index >= 4) {
        return -5;
    }
    if (D_800A6D78[index].field0 == 0) {
        return -5;
    }
    if (D_800A6D78[index].field0 != 0 && D_800A6D78[index].field8 == 0) {
        *arg3 = 0;
        *arg2 = 0;
        return 0;
    }

    value = -9;
    if ((u32)index < 4) {
        if (D_800A6D78[index].field0 != 0) {
            value = D_800A6D78[index].field8;
        }
    }

    ret = D_8009F230(value, arg4);
    *arg2 = ret;
    if (ret == 0) {
        return -0x16;
    }
    return func_8001E004(index, ret, arg3);
}
