// Categories: Graphics > Scene Graph > Attached Parts | Graphics > Geometry
#include "game/types.h"

extern void gGteLoadRotation(s32 *) __asm__("func_80010820");
extern void gGteTransformWords(s32 *, s32 *, s32) __asm__("func_8001006C");
extern s32 gVectorDot(s32 *, s32 *) __asm__("func_8001AB00");
s32 gTakeArmedLobePoint(u8 *a0, s32 *a1, u8 *a2) __asm__("func_8005CED4");


/**
 * @brief Finds the first active attachment record of an object and
 *        computes its world position, with an optional orientation test.
 *
 * @param a0 Object base; records start at a0 with stride 0x1C up to
 *           a0 + 0x150, the active bit is byte +0x71, the scale factor
 *           the s16 at +0x48, the origin the triple at +0x3C.
 * @param a1 Receives the resulting world position (three s32).
 * @param a2 Reference object whose position triple at +0x50 is used for
 *           the orientation test.
 * @return 1 when a record was processed, 0 when none is active or the
 *         orientation test rejects it.
 *
 * @par Confirmed
 * For the first record with byte +0x71 bit 0 set: a1 = ((record[0x58..]
 * >> 4) * scale) >> 8 per axis; func_80010820 loads the matrix at a0+0x18,
 * func_8001006C transforms a1 in place. When bit 2 of the word pointed to
 * by *(s32 *)a0 at +0x18 is clear, vec = a2 position - a0 origin and
 * func_8001AB00(a1, vec) < 0 aborts with 0. Otherwise a1 += origin, the
 * active bit is cleared and 1 is returned.
 *
 * @par Likely role
 * Emission-point lookup for attached effects: the dot-product test keeps
 * only points facing the reference object.
 *
 * @par Callers
 * func_8005D8F4.
 *
 * @par Environment path
 * Helper of the damage-field update func_8005D8F4 (its only caller): it scans
 * the twelve 0x1C-byte lobes from +0x58 for the first one whose armed bit
 * (+0x71 bit 0) is set, scales the lobe offset by the field radius at +0x48,
 * rotates it by the field matrix at +0x18 (func_80010820, func_8001006C) and
 * adds the field centre at +0x3C. Unless bit 2 of the byte at (record at
 * +0x00) + 0x18 is set, the lobe must also face the target (func_8001AB00 dot
 * product of the offset and the target-minus-centre vector must be
 * non-negative), otherwise it returns 0. On success it clears the lobe's
 * armed bit and returns 1; the caller then spawns a beam from that point to
 * the target. No globals are read.
 */
s32 gTakeArmedLobePoint(u8 *a0, s32 *a1, u8 *a2)
{
    u8 *p;
    s32 vec[3];

    p = a0;
    do {
        if (p[0x71] & 1) {
            a1[0] = ((*(s32 *)(p + 0x58) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            a1[1] = ((*(s32 *)(p + 0x5C) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            a1[2] = ((*(s32 *)(p + 0x60) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            gGteLoadRotation((s32 *)(a0 + 0x18));
            gGteTransformWords(a1, a1, 1);
            if (!(*(u8 *)(*(s32 *)a0 + 0x18) & 4)) {
                vec[0] = *(s32 *)(a2 + 0x50) - *(s32 *)(a0 + 0x3C);
                vec[1] = *(s32 *)(a2 + 0x54) - *(s32 *)(a0 + 0x40);
                vec[2] = *(s32 *)(a2 + 0x58) - *(s32 *)(a0 + 0x44);
                if (gVectorDot(a1, vec) < 0) {
                    return 0;
                }
            }
            a1[0] = *(s32 *)(a0 + 0x3C) + a1[0];
            a1[1] = *(s32 *)(a0 + 0x40) + a1[1];
            a1[2] = *(s32 *)(a0 + 0x44) + a1[2];
            p[0x71] &= 0xFE;
            return 1;
        }
        p += 0x1C;
    } while ((s32)p < (s32)(a0 + 0x150));
    return 0;
}
