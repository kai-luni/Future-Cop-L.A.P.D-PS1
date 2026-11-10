#include "game/types.h"

struct Name7 {
    s8 c[7];
};

extern struct Name7 D_800A6FE0;
extern struct Name7 D_800A6FE9;
extern struct Name7 D_8009F474;
extern struct Name7 D_8009F47C;
extern u32 D_8009F8E8;
extern u8 D_8009F3E4;

extern u8 *func_80089914(s32 id);
extern s32 func_8008CCC0(u8 *s);
extern void func_8008CCB0(void *dst, u8 *src);

void func_8007799C(void)
{
    register u32 base __asm__("$2");
    register s32 flag __asm__("$3");
    u8 *p;
    u8 ok;

    D_8009F8E8 = base + 0x5DC;
    ok = 0;
    if (flag == 0) {
        p = func_80089914(0xC8);
        if (p != 0 && func_8008CCC0(p) < 9) {
            func_8008CCB0(&D_800A6FE0, p);
        } else {
            D_800A6FE0 = D_8009F474;
            ok = 1;
        }
        p = func_80089914(0xD2);
        if (p != 0 && func_8008CCC0(p) < 9) {
            func_8008CCB0(&D_800A6FE9, p);
        } else {
            D_800A6FE9 = D_8009F47C;
            ok = 1;
        }
        if (ok == 0) {
            D_8009F3E4 = 1;
        }
    }
}
