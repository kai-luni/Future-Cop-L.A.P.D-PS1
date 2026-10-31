#include "game/types.h"

extern u16 D_8009EEBC;
extern s32 D_8009EECC;

extern void func_8009745C(s32 arg0, ...);

s32 func_80097760(s32 arg0, s32 arg1)
{
    func_8009745C(2, D_8009EEBC << D_8009EECC);
    func_8009745C(0);
    func_8009745C(3, arg0, arg1);
    return arg1;
}
