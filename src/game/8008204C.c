#include "game/types.h"

extern u8 *D_8009F92C;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;
extern s32 D_8009F8B8;
extern u8 D_8009F8C0[];

extern s32 func_80020B64(void);
extern void func_80091270(s32 arg0);
extern s32 func_800207B4(s32 arg0);
extern s32 func_80078740(void);

void func_8008204C(void)
{
    u32 i;

    if (func_80020B64() != 0) {
        i = 0;
        do {
            func_80091270(0);
            if (func_800207B4(0) != -1) {
                i = 5;
                break;
            }
            if (func_80020B64() == 0) {
                D_8009F92C[0x922] = 3;
                D_8009F92C[0x201] = 3;
                D_8009F92C[0x202] = 1;
                D_8009F92C[0x207] = 1;
                D_8009F8BD = 0;
                D_8009F8BE = 0;
                break;
            }
            i++;
        } while (i < 5);
        if (i == 5) {
            D_8009F92C[0x201] = 4;
        }
    } else {
        D_8009F92C[0x922] = 3;
        D_8009F92C[0x201] = 3;
        D_8009F92C[0x202] = 1;
        D_8009F92C[0x207] = 1;
        D_8009F8BD = 0;
        D_8009F8BE = 0;
    }
}
