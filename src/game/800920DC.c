#include "game/types.h"

struct S {
    u16 a;
    u16 b;
    u16 c;
    u16 d;
};

extern void func_80092F38(void *rect, void *data);
extern u16 func_8009226C(s32 x, s32 y);

u16 func_800920DC(s32 x, s32 a, s32 b)
{
    struct S s;

    s.a = a;
    s.b = b;
    s.c = 0x10;
    s.d = 1;
    func_80092F38(&s, (void *)x);
    return func_8009226C(a, b);
}
