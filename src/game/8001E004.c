#include "game/types.h"

typedef struct {
    volatile s32 field0;
    u8 unk04[4];
    s32 field8;
    u8 unk0C[0xC];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];
extern volatile u8 D_8009FA68;
extern s32 D_8009F620;

extern s32 func_8001D90C(s32 index, s32 arg1, s32 value, u32 (*callback)(u32));
extern s32 func_8001D3A8(u32 index);
extern u32 func_8001DFF0(u32 arg0);

s32 func_8001E004(s32 arg0, s32 arg1, s32 *arg2)
{
    s32 value;
    s32 arg;
    s32 result;
    s32 stored;

    if (arg1 == 0) {
        return -0x11;
    }

    value = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].field0 != 0) {
            value = D_800A6D78[arg0].field0;
        }
    }
    if (value < 0) {
        return value;
    }

    D_8009FA68 = 0;
    D_8009F620 = 0;

    arg = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].field0 != 0) {
            arg = D_800A6D78[arg0].field8;
        }
    }

    value = func_8001D90C(arg0, arg1, arg, func_8001DFF0);
    if (value < 0) {
        return value;
    }

    while (D_8009FA68 == 0) {
    }

    stored = D_8009F620;
    func_8001D3A8(arg0);
    if (arg2 != 0) {
        *arg2 = stored;
    }
    return stored;
}
