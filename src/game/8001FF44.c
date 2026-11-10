#include "game/types.h"

struct Ctx {
    u8 pad0[0x8F4];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    u8 pad908[0x9A4 - 0x908];
    s32 unk9A4;
    s32 unk9A8;
    u8 pad9AC[0x1BB0 - 0x9AC];
    s32 unk1BB0;
};

extern volatile struct Ctx *D_8009AB08;
extern void func_8008D640(s32);

s32 func_8001FF44(void)
{
    static void *const table[] = { &&case0, &&fallback, &&fallback, &&fallback, &&fallback, &&fallback, &&fallback };
    register s32 count __asm__("$2");
    register volatile struct Ctx *ctx __asm__("$3");
    register volatile struct Ctx *result __asm__("$2");
    D_8009AB08->unk1BB0 = 0;
    D_8009AB08->unk9A4 = 1;
    if ((u32)D_8009AB08->unk8F4 < 7)
        goto *table[D_8009AB08->unk8F4];
    goto fallback;
case0:
    ctx = D_8009AB08;
    if (ctx->unk9A8 > 0) {
        count = ctx->unk9A8 - 1;
    } else {
        __asm__ volatile("" ::: "memory");
        ctx->unk900 = -3;
        __asm__ volatile("" ::: "$2", "memory");
        ctx->unk8F8 = 0;
        ctx->unk8F4 = 0;
        ctx->unk904 = 0;
        count = 1;
    }
    ctx->unk9A8 = count;
    __asm__ volatile("" ::: "memory");
    result = D_8009AB08;
    goto done;
fallback:
    result = D_8009AB08;
    result->unk900 = -3;
    result->unk8F8 = 0;
    result->unk8F4 = 0;
    result->unk904 = 0;
done:
    func_8008D640(result->unk8FC);
    return 0;
}
