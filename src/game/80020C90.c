#include "game/types.h"

struct CardManager {
    s32 unk00;
    s32 state[16];
    void *primary[16];
};

extern struct CardManager *D_8009AB08;

extern s32 func_80020A9C(s32 a0);
extern void func_800207CC(void);
extern s32 func_8008CCA0(void *arg0, void *arg1);

s32 func_80020C90(void *arg0)
{
    register s32 ret asm("s1") = 0;
    register u32 i asm("s0");
    register s32 one asm("s2");

    if (func_80020A9C(1) != 0) {
        func_800207CC();
        __asm__("" : "=r"(ret) : "0"(ret));
        i = ret;
        one = 1;
        do {
            if (D_8009AB08->state[i] == one) {
                if (func_8008CCA0(arg0, D_8009AB08->primary[i]) == 0) {
                    ret = i;
                    break;
                }
            }
            i++;
        } while (i < 16);
    }
    return ret;
}
