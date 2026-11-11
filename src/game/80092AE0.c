#include "game/types.h"

typedef struct {
    u8 pad[0x34];
    void (*fn34)(s32);
} Unk8009DDE8;

extern u8 D_8009DDF1[];
extern char D_80012A00[];
extern void (*D_8009DDEC)(char *, s32);
extern Unk8009DDE8 *D_8009DDE8;
extern void func_800914E0(s32, s32);

s32 func_80092AE0(s32 arg0)
{
    register u8 *p __asm__("$17") = D_8009DDF1;
    s32 old;
    u8 mode = p[1];

    old = p[0];
    if (mode >= 2) {
        D_8009DDEC(D_80012A00, arg0);
    }
    if (arg0 != p[0]) {
        D_8009DDE8->fn34(1);
        p[0] = arg0;
        func_800914E0(2, 0);
    }
    return old;
}
