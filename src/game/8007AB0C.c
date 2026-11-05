#include "game/types.h"

extern s8 D_8009F8C0[];
extern s32 D_8009F8B0[];
extern s32 D_8009F8F8[];
extern u8 D_8009F470[];
extern u8 D_800A3928[];
extern s16 D_8009F87C;
extern u8 *D_8009F878;

extern void func_8007A9D0(u8 *arg0, u8 *arg1, s32 arg2, u8 *arg3, s32 arg4);
extern s32 func_80046C84(s32 id, s32 arg1);

s32 func_8007AB0C(s32 arg0, s32 arg1)
{
    if (D_8009F8C0[arg1] != 0) {
        if ((arg0 & ~D_8009F8B0[arg1]) < 0) {
            D_8009F8F8[arg1] = 0;
            func_8007A9D0(D_8009F470, D_800A3928 + arg1 * 8, 8, D_8009F878, D_8009F87C);
            func_80046C84(0x48, 0);
        }
    }
    return arg0;
}
