#include "game/types.h"

extern u8 D_8009F26C;
extern s32 D_8009F280;
extern s32 D_8009F648;
extern s32 D_8009F64C;

extern u8 func_8002443C[];

extern s32 func_80022484(s32 arg0, ...);
extern u32 func_800226C8(void);
extern void func_800226D8(void);
extern s32 func_80022710(void);
extern void func_800227BC(s32 arg0, s32 arg1, s32 arg2);
extern void func_800228F4(void);
extern void func_80024630(s32 arg0);
extern s32 func_800246A8(s32 arg0);
extern void func_8002A31C(s32 arg0, s32 arg1);
extern s32 func_800451E0(void);
extern s32 func_80045288(void);

s32 func_800244A4(void)
{
    s32 result;

    result = 0;
    if (D_8009F26C != 0) {
        if ((func_800226C8() & 0xFF) != 0) {
            func_800226D8();
            if (D_8009F280 != 0) {
                func_8002A31C(D_8009F280, result);
                D_8009F280 = 0;
            }
        }
        D_8009F648 = 1;
        D_8009F64C = 0x1C2;
        D_8009F280 = 0;
        if (func_80022484(1, 0x37B4, 2, 0x2FC0, 3, 3, 10, 2, 6,
                          (s32)(func_8002443C + 0x34), 0) < 0) {
            func_80024630(0);
        } else {
            result = func_800246A8(4);
            if (result != 0) {
                if (func_80022710() == 0) {
                    func_80024630(0);
                    return 0;
                }
                if (func_800451E0() != 0) {
                    func_800227BC(0, func_80045288(), 0);
                    func_800227BC(1, func_80045288(), 0x7F);
                } else {
                    func_800227BC(0, func_80045288(), 0x40);
                    func_800227BC(1, func_80045288(), 0x40);
                }
                func_800228F4();
            }
        }
    }
    return result;
}
