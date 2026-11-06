#include "game/types.h"

typedef struct {
    u8 pad00[8];
    void (*callback)(s32, s32, s32, s32);
    u8 pad0C[0xC];
    s32 unk18;
} Dispatch;

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(u8 *, s32);
extern Dispatch *D_8009DDE8;
extern u8 D_80012AE8[];

void func_80093280(s32 arg0)
{
    Dispatch *dispatch;
    register s32 a1 __asm__("$5");
    register s32 zero __asm__("$6");
    if (D_8009DDF2 >= 2) {
        D_8009DDEC(D_80012AE8, arg0);
    }
    a1 = arg0;
    __asm__ volatile("" : "+r"(a1));
    dispatch = D_8009DDE8;
    __asm__ volatile("" : "+r"(dispatch));
    zero = 0;
    __asm__ volatile("" : "+r"(zero));
    dispatch->callback(dispatch->unk18, a1, zero, 0);
}
