#include "game/types.h"

struct Context {
    u8 pad[8];
    void (*send)(void *, void *, int, int);
    u8 pad2[12];
    void *arg;
};
extern u8 D_8009DDF2[];
extern char D_80012B14[];
extern void (*D_8009DDEC)(char *, ...);
extern struct Context *D_8009DDE8;
extern void func_80093CE8(u8 *, u8 *);
extern void func_8008CD10(void *, void *, int);

void func_800933B0(u32 arg0, u8 *arg1)
{
    register u8 *base __asm__("$19") = D_8009DDF2;
    register u8 *saved __asm__("$17");
    u32 cond;
    register u32 mask __asm__("$4");
    register struct Context *ctx __asm__("$3");
    register u8 *p __asm__("$16");
    __asm__ volatile ("" : "=r"(base) : "0"(base));
    cond = base[0];
    __asm__ volatile ("" : "=r"(cond) : "0"(cond));
    saved = arg1;
    if (cond >= 2)
        D_8009DDEC(D_80012B14, arg0, saved);
    p = saved + 0x1c;
    func_80093CE8(p, saved);
    mask = 0xffffff;
    ctx = D_8009DDE8;
    *(u32 *)p = (*(u32 *)p & 0xff000000) | (arg0 & mask);
    ctx->send(ctx->arg, p, 0x40, 0);
    func_8008CD10(base + 0xe, saved, 0x5c);
}
