#include "game/types.h"

typedef struct {
    u8 pad0[0xC];
    void *unkC;
    u8 pad10[0x26];
    volatile u8 unk36;
} Unk8008B810;

extern u32 D_8009C768;
extern u32 D_8009C778;
extern u32 D_8009C764;
extern u32 D_8009C758;
extern u32 D_8009C754;
extern u32 D_8009C7A8;
extern u32 D_8009C740;
extern s32 func_8008AB80(Unk8008B810 *, u32);

s32 func_8008B810(Unk8008B810 *arg0)
{
    register Unk8008B810 *a0 __asm__("$4");

    if (D_8009C768 == D_8009C778) {
        if (D_8009C764 != 0) {
            ((void (*)(void))D_8009C758)();
            ((void (*)(void))D_8009C754)();
        }
    }
    if (D_8009C7A8 != 0) {
        ((void (*)(void *))D_8009C740)(arg0->unkC);
        ((void (*)(void *))D_8009C740)((u8 *)arg0->unkC + 0xF0);
    }
    a0 = arg0;
    if (arg0->unk36 != 0) {
        return func_8008AB80(a0, arg0->unk36);
    }
    return func_8008AB80(a0, 0x42);
}
