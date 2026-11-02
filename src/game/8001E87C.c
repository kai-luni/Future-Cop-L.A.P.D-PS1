#include "game/types.h"

typedef struct {
    u8 a;
    u8 b;
    u8 c[2];
    s32 pad;
    s32 d[2];
    u8 pad2[6];
    u8 e;
    u8 f;
} Slot;

extern Slot D_800A6FF8[];
extern u8 D_800A78F0[];

extern void func_8008BDB0(u8 *start, u8 *end);
extern void func_80089E00(void);

void func_8001E87C(void)
{
    s32 i;
    s32 j;
    Slot *p;
    u8 *q;
    s32 *r;


    for (i = 0; i < 2; i++) {
        p = &D_800A6FF8[i];
        p->a = i << 4;
        p->b = 0;
        p->e = 0;
        p->f = 1;
        j = 1;
        q = (u8 *)p + 1;
        for (; j >= 0; j--) {
            q[2] = 0;
            q--;
        }
        j = 1;
        r = (s32 *)((u8 *)p + 4);
        for (; j >= 0; j--) {
            r[2] = 0;
            r--;
        }

    }
    func_8008BDB0(D_800A78F0, D_800A78F0 + 0x24);
    func_80089E00();
}
