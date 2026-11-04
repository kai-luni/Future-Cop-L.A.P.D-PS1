#include "game/types.h"

typedef void (*SpuDmaCallback)(void);

extern s32 D_8009EE94;
extern s32 D_8009EEDC;
extern SpuDmaCallback D_8009EEE0;
extern void *D_8009EA2C;

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_800979F8(SpuDmaCallback callback);
extern void func_8008C970(void *arg);
extern void func_8008C990(void *arg);

void func_80097A48(void)
{
    if (D_8009EE94 == 1) {
        __asm__ volatile(
            ".set\tnoreorder\n\t"
            "lui\t$at,%hi(D_8009EE94)\n\t"
            "jal\tfunc_8008C9A0\n\t"
            "sw\t$zero,%lo(D_8009EE94)($at)\n\t"
            "addu\t$a0,$zero,$zero\n\t"
            "lui\t$at,%hi(D_8009EEDC)\n\t"
            "sw\t$zero,%lo(D_8009EEDC)($at)\n\t"
            "lui\t$at,%hi(D_8009EEE0)\n\t"
            "jal\tfunc_800979F8\n\t"
            "sw\t$zero,%lo(D_8009EEE0)($at)\n\t"
            ".set\treorder\n\t"
        );
        func_8008C970(D_8009EA2C);
        func_8008C990(D_8009EA2C);
        func_8008C9B0();
    }
}
