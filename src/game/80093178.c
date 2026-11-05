#include "game/types.h"

typedef struct {
    u8 unk00[0x2C];
    void (*unk2C)(s32, s32);
} Unk8009DDE8;

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(u8 *, s32, s32);
extern Unk8009DDE8 *D_8009DDE8;
extern u8 D_80012AD0[];
extern u32 D_8009DEB0[];
extern u32 D_8009DE9C[];

void func_80093178(u32 *arg0, s32 arg1)
{
    register u32 a2 __asm__("$6");
    register u32 a0 __asm__("$4");
    register u32 v1 __asm__("$3");
    register u32 a1 __asm__("$5");
    register u32 *v0 __asm__("$2");

    if (D_8009DDF2 >= 2) {
        D_8009DDEC(D_80012AD0, (s32)arg0, arg1);
    }

    D_8009DDE8->unk2C((s32)arg0, arg1);

    a2 = 0xFFFFFF;
    v0 = arg0;
    __asm__ volatile("" : "+r"(v0));
    a1 = (u32)D_8009DEB0;
    __asm__ volatile("" : "+r"(a1));
    v1 = (u32)D_8009DE9C;
    v1 = (v1 & a2);
    a0 = 0x4000000;
    v1 = v1 | a0;
    *(u32 *)a1 = v1;
    a1 = a1 & a2;
    *v0 = a1;
}
