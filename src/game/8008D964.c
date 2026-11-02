#include "game/types.h"

extern s32 D_800A3E00;
extern s32 D_800A3E04[];
extern s32 D_800A3E0C;
extern u8 D_8008DA14[];
extern u8 D_8008DA7C[];

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008CA00(s32, void *);
extern void func_8008CA10(s32, void *);

s32 func_8008D964(void)
{
    register s32 *p __asm__("$3");
    register s32 one __asm__("$4");
    s32 *q;

    func_8008C9A0();

    one = 1;
    p = D_800A3E04;
    __asm__ volatile("" : "=r"(p) : "0"(p), "r"(one));
    q = p - 1;
    p[0] = (s32) D_8008DA14;
    p[1] = (s32) D_8008DA7C;
    D_800A3E00 = 0;
    D_800A3E0C = 0;

    func_8008CA10(one, q);
    func_8008CA00(1, q);
    func_8008C9B0();

    return 1;
}
