#include "game/types.h"

struct Entity {
    u8 unk00[0x0C];
    u8 unk0C;
    u8 unk0D[0x94];
    u8 unkA1;
};

extern s32 func_8002AD58(s32 message, struct Entity *self, void *data);
extern void func_800423F0(struct Entity *self);
extern s32 func_80042470(s32 message, struct Entity *self);
extern s32 func_80042504(s32 message, struct Entity *self, void *data);
extern void func_800425BC(struct Entity *self);

s32 func_800426A8(s32 message, struct Entity *self, void *data)
{
    switch (message) {
    case 1:
        return 0xD0;
    case 2:
        return func_80042504(message, self, data);
    case 6:
        if (self->unkA1 != 0) {
            func_800423F0(self);
            self->unkA1 = 0;
        }
        if (self->unk0C != 0) {
            func_800425BC(self);
        }
        return 0;
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
        return 0;
    case 5:
        return func_80042470(message, self);
    case 12:
    default:
        return func_8002AD58(message, self, data);
    }
}
