#include "game/types.h"

extern void func_8002A5B4(u8 *arg, s32 index);

void func_8002F280(u8 *arg)
{
    s32 i;
    u8 cmd;
    u8 param;

    i = 0;
    while (i < 4) {
        cmd = arg[i * 2 + 0xC];
        if (cmd == 0) {
            break;
        }
        param = arg[i * 2 + 0xD];
        if (cmd == 0x50) {
            arg[0xA1] = param;
            func_8002A5B4(arg, i);
        } else {
            i++;
        }
    }
}
