#include "game/types.h"

typedef struct {
    u16 active;
    u8 pad02[0x30];
    u16 value;
    u32 data;
    u8 payload[1];
} Context;

extern Context D_8009CC88;
extern u16 *D_8009DD18;
extern u32 *D_8009DD1C;
extern void func_80091B80(void *);
extern void func_8008C9B0(void);

Context *func_80091AA4(void)
{
    Context *ctx;
    Context *result;
    register u32 value __asm__("$2");
    register u32 *dest __asm__("$3");
    __asm__("lui %0,%%hi(D_8009CC88)\n\taddiu %0,%0,%%lo(D_8009CC88)" : "=r"(ctx));
    if (ctx->active != 0)
        goto busy;
    func_80091B80(ctx->payload);
    ctx->active = 1;
    *D_8009DD18 = ctx->value;
    dest = D_8009DD1C;
    __asm__ volatile("" : "+r"(dest));
    value = ctx->data;
    __asm__ volatile("nop" : "+r"(value));
    *dest = value;
    func_8008C9B0();
    result = ctx;
    goto done;
busy:
    result = 0;
done:
    return result;
}
