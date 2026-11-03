#include "game/types.h"

typedef struct {
    volatile s32 unk00;
    s32 unk04;
    volatile s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
} GlobalState;

extern GlobalState func_80010000;
extern s32 D_8009F618;
extern u8 D_8009F610[8];
extern u8 D_8009F611;
extern u8 D_8009F61C[];
extern u8 D_800A0798[];
extern void D_8001C808(s32 mode);

extern void func_800914B0(s32 arg0, s32 arg1);
extern void func_8008DEA0(void);
extern void func_8001C470(void *ptr);
extern s32 func_8008E1A8(s32 value);
extern s32 func_8008E1BC(s32 mode, void *src, void *dst);

s32 func_8001D130(s32 arg0)
{
    register u8 *s1 __asm__("$17");
    register void *a0 __asm__("$4");
    register s32 v0 __asm__("$2");
    register s32 v1 __asm__("$3");

    if (arg0 != 1 && arg0 != 0x11) {
        return -6;
    }

    if (func_80010000.unk00 != 0) {
        return -0x10;
    }

    func_80010000.unk00 = 0;
    func_80010000.unk10 = -1;
    func_80010000.unk04 = 0;
    func_80010000.unk08 = 0;

    s1 = D_800A0798;
    func_800914B0(2, 0);
    func_8008DEA0();

    a0 = D_8009F61C;
    __asm__("" : "+r"(a0));
    func_8001C470(a0);

    func_80010000.unk18 = func_8008E1A8((s32)D_8001C808);
    D_8009F618 = 0;

    func_8008E1BC(1, 0, s1);
    func_8008E1BC(1, 0, s1);

    v0 = D_800A0798[0] & 0x10;
    if (v0) {
        return -3;
    }

    {
        register void *a1 __asm__("$5");
        register void *a2 __asm__("$6");
        a0 = (void *)0xE;
        __asm__("" : "+r"(a0));
        v0 = (s32)&D_8009F610[1];
        __asm__("" : "+r"(v0));
        a1 = (u8 *)v0 - 1;
        __asm__("" : "+r"(a1));
        a2 = s1;
        __asm__("" : "+r"(a2));
        v1 = 0x82;
        D_8009F611 = v1;
        v1 = 0xA2;
        ((u8 *)v0)[-1] = v1;

        v0 = func_8008E1BC((s32)a0, a1, a2);
        if (v0 != 0) {
            return 0;
        }
        return -4;
    }
}
