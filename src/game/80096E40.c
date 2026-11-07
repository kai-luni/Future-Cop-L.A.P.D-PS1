#include "game/types.h"

typedef void (*Callback)(void);
extern s32 D_8009EE94;
extern void *D_8009EA2C;
extern void func_800972F8(void);
extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_800979F8(Callback callback);
extern void *func_8008C960(u32, s32, s32, s32);
extern void func_8008C980(void *);

void func_80096E40(void)
{
    if (D_8009EE94 == 0) {
        __asm__ volatile(".set\tnoreorder\n\t"
                         "addiu\t$v0,$zero,1\n\t"
                         "lui\t$at,%%hi(D_8009EE94)\n\t"
                         "jal\tfunc_8008C9A0\n\t"
                         "sw\t$v0,%%lo(D_8009EE94)($at)\n\t"
                         ".set\treorder" ::: "memory");
        func_800979F8(func_800972F8);
        func_8008C960(0xF0000009, 0x20, 0x2000, 0);
        __asm__ volatile(".set\tnoreorder\n\t"
                         "addu\t$a0,$v0,$zero\n\t"
                         "lui\t$at,%%hi(D_8009EA2C)\n\t"
                         "jal\tfunc_8008C980\n\t"
                         "sw\t$a0,%%lo(D_8009EA2C)($at)\n\t"
                         ".set\treorder" ::: "memory");
        func_8008C9B0();
    }
}
