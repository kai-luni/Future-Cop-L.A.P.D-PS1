// Categories: Utilities > Mathematics | Platform > GTE
#include "game/types.h"
void gGteLeadingZeros(u32 value) __asm__("func_80095C14");


/**
 * @brief Leading-zero count via the GTE (log2 helper).
 *
 * @param value Input word; the C parameter is unused by name because the value
 *              must arrive in $a0 for the inline assembly to see it.
 * @return In $v0: the number of leading bits that equal the sign of the input -
 *         the GTE "Leading Zero Count" result. Callers use it to build a shift
 *         amount (func_8001B4A8: shift = 0x20 - result).
 *
 * @par Confirmed mechanics
 * mtc2 $4,$30 (GTE data register 30 = LZCS), two nops to let the pipeline
 * settle, mfc2 $2,$31 (register 31 = LZCR). The function is declared void, but
 * the retail caller reads $v0 - the return value is part of the ABI here.
 *
 * @par Callers
 * func_8001B4A8 only, for the ordering-table depth bucketing.
 *
 * @par Environment path
 * Returns the GTE leading-zero/one count of its argument (mtc2 to LZCS, mfc2
 * from LZCR). gUpdateChaseCamera (func_800569EC) turns 1 << (31 - result)
 * into the power-of-two mask of the camera shake jitter, and
 * gNormalizeVector, gInitReciprocalTable, func_8001A644, func_8001B4A8 and
 * func_8004F330 use it for range normalisation. It is a GTE primitive on the
 * camera motion path.
 */
void gGteLeadingZeros(u32 value) {
    (void)value;
    __asm__ volatile(
        "mtc2 $4,$30\n\t"
        "nop\n\t"
        "nop\n\t"
        "mfc2 $2,$31"
    );
}
