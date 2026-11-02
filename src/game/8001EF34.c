#include "game/types.h"

struct PadBuffer {
    u8 unk00[4];
    u8 rightX;
    u8 rightY;
    u8 leftX;
    u8 leftY;
    u8 unk08[0x1C];
};

extern struct PadBuffer D_800A78F0[];

s32 func_8001EF34(s32 port)
{
    struct PadBuffer *pad;
    s32 result;

    pad = &D_800A78F0[port];
    if (pad->unk00[0] != 0) {
        result = 0;
    } else {
        result = pad->unk00[1] >> 4;
        switch (result) {
        case 4:
        case 7:
            break;
        default:
            result = 0;
            break;
        }
    }
    return result;
}
