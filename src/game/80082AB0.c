#include "game/types.h"

extern void *func_80082B0C(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void *func_80082AB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s8 arg7, s8 arg8)
{
    return func_80082B0C(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg6, arg6, arg7, arg8);
}
