#include "game/types.h"

typedef struct {
    u16 id;
    u16 x;
    u8 y;
    u8 align;
} Entry;

extern Entry D_8009C0E4[];
extern Entry D_8009C18C[];
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);

void func_8007E67C(s32 arg0)
{
    Entry *p;
    u32 i;

    if (arg0 == 0) {
        p = D_8009C0E4;
        i = 0;
        while (i < 28) {
            func_800788F0(0, p->x, p->y, 16, p->id, p->align, 8, 2);
            i++;
            p++;
        }
        p = D_8009C18C;
        i = 0;
        while (i < 5) {
            func_80082AB0(p->id, p->x, p->y, -1, -1, 20, 128, 1, 8);
            i++;
            p++;
        }
    }
    func_80078B34(0x6AE, 0, 0);
    func_80078B34(0x10CC, 1, 0);
}
