// Categories: Utilities > Checksums
#include "game/types.h"

/**
 *  @brief Computes two unsigned accumulators over a byte buffer.
 *
 *  Initializes both outputs to zero, then incorporates each byte and its index.
 *  The second accumulator uses XOR and a left shift before adding the index.
 *  @param data Buffer containing the bytes to process.
 *  @param len Number of bytes to process.
 *  @param sum Output accumulator for byte values plus their indices.
 *  @param other Output accumulator for the XOR-and-shift recurrence.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Loops once per byte, advancing the data pointer after each iteration.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes both output accumulators.
 *  @par Callers
 *  Confirmed: func_8007F6E0, func_8007F824.
 */
void func_8007F688(u8 *data, u32 len, u32 *sum, u32 *other)
{
    u32 i;

    *sum = 0;
    *other = 0;
    for (i = 0; i < len; i++) {
        *sum += *data + i;
        *other = ((*data ^ *other) << 1) + i;
        data++;
    }
}
