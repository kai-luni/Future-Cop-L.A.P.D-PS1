#include "game/types.h"

struct Entity {
    u8 pad[0xAD];
    u8 unkAD;
    s16 unkAE;
};

extern s32 func_80030AD4(void *, void *, s32);
extern u8 func_8002F8B8(u8, s16, void *, u8);
extern s32 func_80030C60(void *, void *, s32);
extern s32 func_8002AD58(s32, struct Entity *, void *, s32);

s32 func_80030D3C(s32 message, struct Entity *self, void *data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB0;
    case 2:
        return func_80030AD4(self, data, arg3);
    case 10:
        return func_8002F8B8(self->unkAD, self->unkAE, data, 0);
    case 11:
        return func_80030C60(self, data, arg3);
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 12:
    case 13:
        return 0;
    default:
        return func_8002AD58(message, self, data, arg3);
    }
}
