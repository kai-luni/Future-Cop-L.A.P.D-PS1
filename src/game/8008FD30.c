#include "game/types.h"

extern volatile u8 *D_8009CB6C;
extern volatile u8 *D_8009CB78;
extern volatile u32 *D_8009CBA0;
extern volatile u32 *D_8009CB7C;
extern volatile u32 *D_8009CBA4;
extern volatile u32 *D_8009CBA8;
extern volatile u32 *D_8009CBAC;
extern volatile u32 *D_8009CBB0;

int func_8008FD30(u32 destination, u32 size)
{
    volatile u8 *p;
    volatile u32 status;

    *D_8009CB6C = 0;
    *D_8009CB78 = 0x80;
    *D_8009CBA0 = 0x21020843;
    *D_8009CB7C = 0x1325;
    *D_8009CBA4 |= 0x8000;
    *D_8009CBA8 = destination;
    *D_8009CBAC = size | 0x10000;
    p = D_8009CB6C;
    while ((*p & 0x40) == 0) {
        continue;
    }
    *D_8009CBB0 = 0x11400100;
    status = *D_8009CBB0;
    return 0;
}
