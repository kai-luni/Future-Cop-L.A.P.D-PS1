#include "game/types.h"

extern s32 D_8009C820[];
extern s32 D_8009C8A8;
extern u8 D_8009C8B4;
extern s32 func_8008F244(s32, void *, void *, s32);

s32 func_8008E1BC(s32 command, void *param, void *result)
{
    register s32 i __asm__("$16");
    register void *p __asm__("$17") = param;
    register void *r __asm__("$18") = result;
    register s32 index __asm__("$19");
    register s32 cmd __asm__("$20") = command;
    register s32 saved __asm__("$21");
    register s32 *entry __asm__("$22");
    register s32 ret __asm__("$23");
    s32 end;
    register s32 *base __asm__("$3");
    register s32 off __asm__("$2");
    register s32 one __asm__("$8");
    register s32 call_a0 __asm__("$4");
    register void *zero_a1 __asm__("$5");
    register void *zero_a2 __asm__("$6");
    register s32 zero_a3 __asm__("$7");

    __asm__("" : "+r"(cmd));
    i = 3;
    index = cmd & 0xff;
    __asm__("" : "+r"(index));
    base = D_8009C820;
    __asm__("" : "+r"(base));
    saved = D_8009C8A8;
    __asm__("" : "+r"(saved));
    off = index << 2;
    __asm__("" : "+r"(off));
    entry = (s32 *)((unsigned int)off + (unsigned int)base);
    ret = 0;
    end = -1;
    __asm__("" : "+r"(ret), "+r"(end));
    do {
        D_8009C8A8 = 0;
        one = 1;
        __asm__("" : "+r"(one));
        if (index != one && (D_8009C8B4 & 0x10)) {
            call_a0 = 1;
            __asm__("" : "+r"(call_a0));
            zero_a1 = 0;
            __asm__("" : "+r"(zero_a1));
            zero_a2 = 0;
            __asm__("" : "+r"(zero_a2));
            func_8008F244(call_a0, zero_a1, zero_a2, 0);
        }
        if (p == 0 || *entry == 0 ||
            func_8008F244(2, p, r, 0) == 0) {
            D_8009C8A8 = saved;
            if (func_8008F244(cmd & 0xff, p, r, 0) == 0) {
                return ret + 1;
            }
        }
        i--;
    } while (i != end);
    D_8009C8A8 = saved;
    ret = -1;
    __asm__("" : "+r"(ret));
    __asm__ volatile(".set reorder");
    return ret + 1;
}
