#include "game/types.h"

extern u8 D_8009FA98;
extern u8 D_8009FA99;
extern u8 D_8009F8D8;
extern u8 D_8009F8D9;
extern u8 D_8009F900;
extern u8 D_8009F901;
extern u8 D_8009F902;
extern u8 D_8009F903;
extern s8 D_8009F904;
extern u8 D_800A374D[];
extern u8 func_8001EB2C(s32 arg0);

void func_8007C088(void)
{
    D_8009F900 = D_8009FA98;
    D_8009F901 = D_8009FA99;
    D_8009F8D8 = func_8001EB2C(0);
    D_8009F8D9 = func_8001EB2C(1);
    D_8009F902 = 0;
    D_8009F903 = 0;
    if (D_8009F904 == 1) {
        D_800A374D[0] = 3;
    } else {
        D_800A374D[0] = 1;
    }
}
