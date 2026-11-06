#include "game/types.h"

extern s32 D_8009FA2C;
extern s32 D_8009F3D0;
extern s32 D_8009F870;
extern s8 D_8009F874;
extern u8 D_8009F4AC[];

extern void func_80047690(u8 enable);
extern void func_80089D44(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
extern s32 func_8008CD30(u8 *buffer, u8 *format, s32 value, s32 arg3);

void func_80082DDC(s32 arg0, s32 arg1)
{
    u8 buf[0x28];
    s32 start;

    start = D_8009FA2C;
    func_80047690(0);
    switch (arg1) {
    case 1:
        func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0);
        func_80089D44((s32)buf, 0x26, 0x20, 0x31, 0x100, 0x90);
        break;
    case 0:
    case 2:
        func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0);
        func_80089D44((s32)buf, 4, 0, 0x18, 0x140, 0xC0);
        break;
    }
    func_80047690(1);
    D_8009F874 = -1;
    D_8009F870 += D_8009FA2C - start;
}
