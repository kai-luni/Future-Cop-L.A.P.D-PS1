#include "game/types.h"

extern volatile s32 D_8009FA60;
extern u8 D_8009F8D8[];

extern void func_80082EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b);
extern void func_8007C0F8(s32 arg0);
extern s32 func_8001EF34(s32 port);
extern u8 func_8001EAEC(s32 arg0);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

void func_8007C6F0(s32 slot)
{
    register s32 target __asm__("$2");
    s32 a;
    s32 b;
    s32 t;

    if (slot != target) {
        return;
    }

    func_80082EF0(0x20, 0x30, 0x100, 0x90, 0xBB8, 0, 0, 0);
    func_8007C0F8(slot);

    a = 0x6AE;
    b = 0x1324;
    t = (slot == 1);
    if (((D_8009FA60 >> 9) & 1) == 0) {
        b = 0x132E;
        a = 0x10CC;
        if (((D_8009FA60 >> 10) & 1) != 0) {
            if (func_8001EF34(t) && func_8001EAEC(t)) {
                __asm__("");
                if (D_8009F8D8[slot] != 0) {
                    a = 0x1338;
                } else {
                    a = 0x133D;
                }
            }
        }
    }

    func_80078B34(b, 0, 0);
    func_80078B34(a, 1, 0);
}
