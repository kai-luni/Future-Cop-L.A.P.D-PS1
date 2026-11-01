#include "game/types.h"

typedef struct {
    u8 pad_00[0x46];
    u8 device;
    u8 pad_47[0x9F];
    u16 ready;
} Unk8008C858;

s32 func_8008C858(Unk8008C858 *state)
{
    register u32 tmp __asm__("$2");
    register u32 device __asm__("$3");

    tmp = state->ready;
    if (tmp == 0) {
        return 1;
    }
    tmp = 0xFF;
    device = state->device;
    if (device == tmp) {
        return 0;
    }
    return 1;
}