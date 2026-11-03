#include "game/types.h"

extern u8 *D_8009F468;
extern u8 D_8009F875;
extern u8 D_8009F876;
extern u8 D_8009F8C0;
extern u8 D_8009F8C1;
extern u8 D_8009F8CC;
extern void func_800785F8(void);
extern void func_800788B4(void);

s32 func_80078740(void)
{
    register s32 ret __asm__("$3");
    register u8 *p __asm__("$4");

    p = D_8009F468;
    ret = 0;
    if (*(u32 *)(p + 0x24) != 0) {
        if (*(void (**)(void))(p + 0x14) != 0) {
            (*(void (**)(void))(p + 0x14))();
        }
        D_8009F468 = *(u8 **)(D_8009F468 + 0x24);
        D_8009F8C0 = 0;
        D_8009F8C1 = 0;
        func_800785F8();
        D_8009F8CC = 0;
        func_800788B4();
        if ((s8)D_8009F468[3] != -1) {
            D_8009F875 = D_8009F468[3];
            D_8009F876 = D_8009F468[4];
        }
        ret = 1;
    }
    return ret;
}
