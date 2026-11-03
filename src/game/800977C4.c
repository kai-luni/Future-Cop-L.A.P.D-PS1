#include "game/types.h"

extern u16 *D_8009EEA4;
extern u32 D_8009EECC;

void func_800977C4(s32 index, u32 value, s32 flag)
{
    if (flag == 0) {
        D_8009EEA4[index] = value;
    } else {
        D_8009EEA4[index] = value >> D_8009EECC;
    }
}
