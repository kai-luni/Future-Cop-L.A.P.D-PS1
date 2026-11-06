#include "game/types.h"

typedef struct {
    u8 pad[0x10];
    void (*fn)(s32);
} Context;

extern u8 D_8009DDF2[];
extern u8 D_80012A30[];
extern void (*D_8009DDEC)(u8 *, s32);
extern Context *D_8009DDE8;
extern void func_80095830(u8 *, int, unsigned int);

void func_80092BF4(s32 arg0)
{
    register u8 *p __asm__("$17") = D_8009DDF2;
    s32 mode;
    Context *ctx;
    __asm__("" : "=r"(p) : "0"(p));
    if (p[0] >= 2) {
        D_8009DDEC(D_80012A30, arg0);
    }
    if (arg0 == 0) {
        func_80095830(p + 0x6A, -1, 0x14);
    }
    mode = 0x3000001;
    ctx = D_8009DDE8;
    if (arg0 != 0) {
        mode = 0x3000000;
    }
    ctx->fn(mode);
}
