#include "game/types.h"

extern void func_8008D7B0(s32 arg0);
extern void func_8008C9A0(void);
extern s32 func_8008D7CC(void);
extern void func_8008DC20(s32 arg0);
extern void func_8008DDEC(void);
extern void func_8008DCE8(void);
extern void func_8008DD7C(void);
extern void func_8008C9B0(void);

void func_8008D6E0(s32 mode)
{
    s32 temp_s0;

    temp_s0 = mode;
    func_8008D7B0(0);
    func_8008C9A0();
    if (func_8008D7CC() == 0) {
        temp_s0 = 0;
    }
    func_8008DC20(temp_s0);
    func_8008DDEC();
    func_8008DCE8();
    func_8008DD7C();
    func_8008C9B0();
}
