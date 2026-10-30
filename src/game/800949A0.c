#include "game/types.h"

extern u32 *D_8009DEC8;
extern u32 *D_8009DECC;
extern u32 *D_8009DED0;
extern u32 *D_8009DED4;

void func_800949A0(u32 value)
{
    *D_8009DEC8 = 0x04000002;
    *D_8009DECC = value;
    *D_8009DED0 = 0;
    *D_8009DED4 = 0x01000401;
}
