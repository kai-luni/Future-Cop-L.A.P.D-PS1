// Categories: Physics > Movement
#include "game/types.h"

struct Unk537D0 {
    u8 pad0[0xA0];
    s32 fieldA0;
    u8 pad1[0x4];
    s32 fieldA8;
};

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void gUpdateTwoPlayerSeparation(struct Unk537D0 *a0, s32 *out) __asm__("func_80053730");
void gBuildHoverMovementVector(struct Unk537D0 *a0, s32 a1, s32 a2, s32 *a3, s32 a4) __asm__("func_800537D0");


/**
 *  @brief Computes two fixed-point output components from object fields and inputs.
 *
 *  The calculation uses fields at offsets 0xA0 and 0xA8, optionally rotating
 *  or combining them before multiplying and shifting the products by 16 bits.
 *  @param a0 Object supplying the two source components.
 *  @param a1 Input magnitude used in the component calculations.
 *  @param a2 Nonzero selects a quarter-turn transformation.
 *  @param a3 Output array receiving components at indices 0 and 2.
 *  @param a4 Selects a diagonal transformation when a2 is zero and a4 is nonzero.
 *  @par Globals
 *  None directly referenced.
 *  @par Control flow
 *  A zero magnitude clears both components; otherwise branches select the transform.
 *  @par Calls
 *  func_80053730 processes the object and output array after component calculation.
 *  @par Side effects
 *  Writes a3[0] and a3[2]; the called function may make further changes.
 *  @par Callers
 *  Confirmed: func_80053A74.
 */
void gBuildHoverMovementVector(struct Unk537D0 *a0, s32 a1, s32 a2, s32 *a3, s32 a4)
{
    register s32 t2 __asm__("$10");
    register s32 t3 __asm__("$11");
    register s32 t4 __asm__("$12");
    register s32 tmp;
    register s32 old;
    register s32 neg;
    register s32 q __asm__("$3");

    if (a1 != 0) {
        t2 = a0->fieldA0;
        t3 = a0->fieldA8;
        t4 = a1;

        if (a2 == 0) {
            if (a4 != 0) {
                a1 *= 181;
                q = a1;
                if (a1 < 0) {
                    q = a1 + 255;
                }
                a1 = q >> 8;
                t4 = a1;
                neg = a1 < 1;
                a2 = neg ^ 1;
                if (a4 == 1) {
                    old = t2;
                    if (neg) {
                        t2 -= t3;
                        t3 += old;
                    } else {
                        t2 += t3;
                        t3 -= old;
                    }
                } else {
                    old = t2;
                    if (a2) {
                        t2 -= t3;
                        t3 += old;
                    } else {
                        t2 += t3;
                        t3 -= old;
                    }
                }
            }
        } else {
            old = t2;
            t2 = t3;
            t3 = -old;
        }
        FMUL(tmp, t4, t2);
        a3[0] = tmp;
        FMUL(tmp, a1, t3);
        a3[2] = tmp;
    } else {
        a3[0] = 0;
        a3[2] = 0;
    }
    gUpdateTwoPlayerSeparation(a0, a3);
}
