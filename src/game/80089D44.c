#include "game/types.h"

extern u8 func_80087D3C(void);
extern u8 func_80087EA0(s32 arg0);
extern void func_80088120(s32 arg0, s32 arg1, s32 arg2);
extern void func_80087D48(void);
extern s32 func_8008A5A8(void);

void func_80089D44(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    u8 flag;

    flag = 0;
    if (!(func_80087D3C() & 0xFF)) {
        if (!(func_80087EA0(arg1) & 0xFF)) {
            return;
        }
        flag = 1;
    }
    func_80088120(arg0, arg2, arg3);
    if (flag & 0xFF) {
        func_80087D48();
    }
}
