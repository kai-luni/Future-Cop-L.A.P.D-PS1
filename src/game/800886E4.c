#include "game/types.h"

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern s32 func_8008E1BC(s32 code, void *arg1, void *arg2);

s32 func_800886E4(void)
{
    u8 buffer1[0x20];
    u8 buffer2[0x20];

    func_8001E4C0(buffer1, 0, 0x20);
    func_8001E4C0(buffer2, 0, 0x20);

    if (func_8008E1BC(1, buffer1, buffer2) != 0) {
        switch (buffer2[0]) {
        case 0:
        case 2:
            return 0;
        default:
            return 1;
        }
    }

    func_8008E1BC(0xA, 0, 0);
    return 1;
}
