#include "game/types.h"

extern void func_8008C9A0(void);
extern void func_8008DC30(void);
extern void func_8008D7B0(s32 value);
extern void func_8008C9B0(void);

void func_8008D74C(void)
{
    func_8008C9A0();
    func_8008DC30();
    func_8008D7B0(0);
    func_8008C9B0();
}
