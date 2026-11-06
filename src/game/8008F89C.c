#include "game/types.h"

extern s32 D_8009C8AC;
extern s32 D_8009C8A8;
extern s32 D_8009C8B8;
extern u8 D_8008FE28[];

extern void func_80091480(void);
extern void func_800914B0(s32 arg0, u8 *arg1);

void func_8008F89C(void)
{
    register u32 base __asm__("$1");
    register u8 *callback __asm__("$5");
    D_8009C8AC = 0;
    D_8009C8A8 = 0;
    D_8009C8B8 = 0;
    __asm__("" ::: "memory");
    base = 0x800A0000;
    __asm__("" : "+r"(base));
    *(s32 *)(base - 0x374C) = 0;
    func_80091480();
    callback = D_8008FE28;
    __asm__("" : "+r"(callback));
    func_800914B0(2, callback);
}
