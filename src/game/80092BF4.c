// Categories: Graphics > GPU > Command Queue | Platform > GPU
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

/**
 * @brief Flushes or resets the GPU command queue, depending on the argument.
 *
 * @param arg0 0 = reset path, non-zero = flush path.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. Debug hook when the library level byte D_8009DDF2[0] >= 2.
 * 2. On arg0 == 0 the 0x14-byte block at D_8009DDF2 + 0x6A is filled with -1
 *    (func_80095830, i.e. "no command pending").
 * 3. The command word is 0x3000000 for the flush path and 0x3000001 for the reset
 *    path; it is handed to D_8009DDE8->fn.
 */
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
