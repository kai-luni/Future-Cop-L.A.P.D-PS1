#include "game/types.h"

struct PadActuator {
    u8 port;
    u8 unk01;
    u8 motor[2];
    u8 smallIndex;
    u8 largeIndex;
    u8 unk06[2];
    s32 smallTimer;
    s32 largeTimer;
    u8 unk10[6];
    u8 unk16;
    u8 enabled;
};

extern struct PadActuator D_800A6FF8[];
extern s32 D_8009FA2C;

void func_8001EA5C(s32 port, s32 level)
{
    struct PadActuator *actuator;
    s32 clamped;
    s32 index;

    actuator = &D_800A6FF8[port];
    if (!actuator->enabled || (index = actuator->largeIndex) == 0xFF) {
        return;
    }

    clamped = 0xFF;
    if (level >= 0) {
        if (level < 0x100) {
            clamped = level;
        }
    } else {
        clamped = 0;
    }
    level = clamped;

    if (level) {
        actuator->motor[index] = level;
        actuator->largeTimer = D_8009FA2C + 300;
    } else {
        actuator->motor[index] = 0;
        actuator->largeTimer = 0;
    }
}
