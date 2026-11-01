#include "game/types.h"

extern u8 *D_800A0C70[];
extern u8 *func_80017EBC(s32 size, s32 type);
extern void func_800181D4(void *block);
extern void *memcpy(void *dst, const void *src, u32 n);

void func_8003BC7C(u8 *arg0)
{
    u8 *block;

    if (D_800A0C70[arg0[0x14]] != 0) {
        func_800181D4(D_800A0C70[arg0[0x14]]);
    }

    block = func_80017EBC(0x1C, 2);
    D_800A0C70[arg0[0x14]] = block;

    block[0] = arg0[0x18];
    block[1] = arg0[0x19];
    block[2] = arg0[0x1A];
    block[3] = arg0[0x1B];
    block[0x1A] = arg0[0x15];

    memcpy(block + 4, arg0 + 0x1C, 4);
    memcpy(block + 8, arg0 + 0x20, 4);
    memcpy(block + 0xC, arg0 + 0x24, 4);
    memcpy(block + 0x10, arg0 + 0x28, 4);
    memcpy(block + 0x14, arg0 + 0x2C, 4);

    *(u16 *)(block + 0x18) = *(u16 *)(arg0 + 0x16);
}
