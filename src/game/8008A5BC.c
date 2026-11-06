#include "game/types.h"

typedef void (*Handler)(s32 port);

extern s32 D_8009C760;
extern s32 *D_8009C788;
extern s32 D_8009C75C;
extern Handler D_8009C72C;
extern u8 D_800A3B60[];
extern s32 D_800A3B70[];

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008CA00(s32 arg0, u8 *arg1);
extern void func_8008CA10(s32 arg0, u8 *arg1);
extern void func_8008CA20(s32 arg0, s32 arg1);

void func_8008A5BC(void)
{
    u8 *buf;
    register s32 *state __asm__("$3");
    register s32 *end __asm__("$2");
    register s32 arg __asm__("$4");
    register s32 *base __asm__("$1");

    __asm__ volatile("lui %0, %%hi(D_8009C760)" : "=r"(base));
    base[-0x38A0 / 4] = 0;
    func_8008C9A0();
    __asm__ volatile("" ::: "$16");
    buf = D_800A3B60;
    func_8008CA10(2, buf);
    func_8008CA00(2, buf);

    arg = 3;
    __asm__ volatile("" : "+r"(arg) :: "memory");
    state = D_8009C788;
    state[0] = -2;
    __asm__ volatile("" ::: "memory");
    state[1] |= 1;
    func_8008CA20(arg, 0);

    func_8008C9B0();

    D_8009C72C(D_8009C75C);
    D_8009C72C(D_8009C75C + 0xF0);

    end = D_800A3B70;
    __asm__ volatile("" : "+r"(end));
    end[1] = 0;
    end[0] = 0;
    D_8009C760 = 1;
}
