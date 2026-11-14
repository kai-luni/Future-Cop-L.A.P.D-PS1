#include "game/types.h"

extern void func_8008DAD0(void);
extern void func_8008D7B0(s32 value);
extern void func_8008DB00(void);

s32 func_8008D8FC(void)
{
    func_8008DAD0();
    func_8008D7B0(0);
    func_8008DB00();
    return 1;
}
