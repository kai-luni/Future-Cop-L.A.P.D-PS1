#include "game/types.h"

extern u8 *D_8009CB6C;
extern u8 *D_8009CB70;
extern u8 *D_8009CB74;
extern u8 *D_8009CB78;

s32 func_8008F650(u8 *arg0)
{
    *D_8009CB6C = 2;
    *D_8009CB74 = arg0[0];
    *D_8009CB78 = arg0[1];

    *D_8009CB6C = 3;
    *D_8009CB70 = arg0[2];
    *D_8009CB74 = arg0[3];
    *D_8009CB78 = 0x20;

    return 0;
}
