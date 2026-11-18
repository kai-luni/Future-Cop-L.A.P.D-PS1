#include "game/types.h"
extern u8 D_8009FA8C[];
struct S { u8 p[0xC1]; s8 f; };
extern void f(s32, s32, s32, s32);
void test(struct S *d) {
    f(-1, D_8009FA8C[d->f], 0xC, 0xE0);
}
