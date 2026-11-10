#include "game/types.h"

typedef s32 (*PrintFn)();
extern PrintFn D_8009DDEC;

extern char D_800128C4;
extern char D_800128DC;
extern char D_800128EC;
extern char D_80012904;
extern char D_80012910;
extern char D_8001289C;

struct S {
    s16 a, b, c, d, e, f, g, h, i, j;
    u16 k;
    u8 l, m;
};

#define FN() __asm__ volatile("lui %0, %%hi(D_8009DDEC)\n\tlw %0, %%lo(D_8009DDEC)(%0)" : "=r"(f))
#define HI(name, var) __asm__ volatile("lui %0, %%hi(" #name ")" : "=r"(var))

void func_80092764(struct S *p)
{
    register s32 k __asm__("$3");
    s32 a, b, c, d;
    register PrintFn f __asm__("$2");
    register char *a0 __asm__("$4");

    a = p->a; b = p->b; c = p->c; d = p->d;
    FN();
    HI(D_800128C4, a0);
    f(a0 + 0x28C4, a, b, c, d);
    a = p->e; b = p->f;
    FN();
    HI(D_800128DC, a0);
    f(a0 + 0x28DC, a, b);
    a = p->g; b = p->h; c = p->i; d = p->j;
    FN();
    HI(D_800128EC, a0);
    f(a0 + 0x28EC, a, b, c, d);
    a = p->l;
    FN();
    HI(D_80012904, a0);
    f(a0 + 0x2904, a);
    a = p->m;
    FN();
    HI(D_80012910, a0);
    f(a0 + 0x2910, a);
    __asm__ volatile("lui %0, %%hi(D_8001289C)\n\taddiu %0, %0, %%lo(D_8001289C)\n\tlhu %1, 0x14(%3)\n\tlui %2, %%hi(D_8009DDEC)\n\tlw %2, %%lo(D_8009DDEC)(%2)" : "=&r"(a0), "=&r"(k), "=&r"(f) : "r"(p));
    f(a0, (k >> 7) & 3, (k >> 5) & 3, (k << 6) & 0x7C0,
               ((k << 4) & 0x100) + ((k >> 2) & 0x200));
}
