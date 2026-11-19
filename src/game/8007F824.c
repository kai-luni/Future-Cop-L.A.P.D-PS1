// Categories: Utilities > Data Validation
#include "game/types.h"

extern void gComputeDualChecksum(u8 *data, u32 len, u32 *sum, u32 *other) __asm__("func_8007F688");
extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");

/**
 *  @brief Validates two computed values for a data buffer.
 *
 *  Computes a pair of values over the supplied bytes and compares both with
 *  the expected values, optionally clearing the buffer on a mismatch.
 *  @param data Buffer to validate and optionally clear.
 *  @param len Number of bytes in the buffer.
 *  @param expectedSum Expected first computed value.
 *  @param expectedOther Expected second computed value.
 *  @param clear Nonzero to zero the buffer if validation fails.
 *  @return One if both values match, otherwise zero.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  A mismatch returns zero after an optional clear; a match returns one.
 *  @par Calls
 *  func_8007F688 computes the values; func_8001E4C0 clears the buffer.
 *  @par Side effects
 *  May overwrite len bytes of data with zero.
 *  @par Callers
 *  Confirmed: func_8007F750.
 */
s32 gVerifyDualChecksum(u8 *data, u32 len, u32 expectedSum, u32 expectedOther, s32 clear) __asm__("func_8007F824");

s32 gVerifyDualChecksum(u8 *data, u32 len, u32 expectedSum, u32 expectedOther, s32 clear)
{
    u32 sum;
    u32 other;

    gComputeDualChecksum(data, len, &sum, &other);
    if (expectedSum != sum || expectedOther != other) {
        if (clear != 0) {
            gFillMemory(data, 0, len);
        }
        return 0;
    }

    return 1;
}
