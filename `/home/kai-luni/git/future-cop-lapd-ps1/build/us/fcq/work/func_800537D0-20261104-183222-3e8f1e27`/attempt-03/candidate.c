#include "game/types.h"

struct Unk537D0 {
    u8 pad0[0xA0];
    s32 fieldA0;
    u8 pad1[0x4];
    s32 fieldA8;
};

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void func_80053730(struct Unk537D0 *a0, s32 *a3);

void func_800537D0(struct Unk537D0 *a0, s32 a1, s32 a2, s32 *a3, s32 a4)
{
    register s32 t2 __asm__("$t2");
    register s32 t3 __asm__("$t3");
    register s32 t4 __asm__("$t4");
    register s32 res __asm__("$t5");
    s32 tmp;

    if (a1 == 0) {
        a3[0] = 0;
        a3[1] = 0;
    } else {
        t2 = a0->fieldA0;
        t3 = a0->fieldA8;
        t4 = a1;
        if (a2 != 0) {
            tmp = t2;
            t2 = t3;
            t3 = -tmp;
        } else if (a4 != 0) {
            t4 = a1 = a1 * 181 / 256;
            if (a4 == 1) {
                if (a1 < 1) {
                    tmp = t2;
                    t2 -= t3;
                    t3 += tmp;
                } else {
                    tmp = t2;
                    t2 += t3;
                    t3 -= tmp;
                }
            } else {
                if (a1 < 1) {
                    tmp = t2;
                    t2 += t3;
                    t3 -= tmp;
                } else {
                    tmp = t2;
                    t2 -= t3;
                    t3 += tmp;
                }
            }
        }
        FMUL(res, t4, t2);
        a3[0] = res;
        FMUL(res, a1, t3);
        a3[1] = res;
    }
    func_80053730(a0, a3);
}
