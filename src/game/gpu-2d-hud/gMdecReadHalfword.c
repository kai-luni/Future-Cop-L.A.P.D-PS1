// Categories: Graphics > Video Decode | Platform > MDEC
typedef unsigned short u16;

/**
 * @brief Returns the u16 at the given address.
 *
 * @param value Pointer to a 16-bit word.
 * @return The word, zero-extended.
 *
 * @par Confirmed
 * Single u16 load. In the MDEC paths the callers use it to read the low
 * halfword of the decode control word (the companion of func_80095FF4,
 * which writes the control bits).
 *
 * @par Callers
 * gMovieSliceFeedDma, gMovieSliceFeedRows.
 */
u16 gMdecReadHalfword(u16 *value) __asm__("func_80095FE8");

u16 gMdecReadHalfword(u16 *value)
{
    return *value;
}
