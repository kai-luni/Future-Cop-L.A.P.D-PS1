// Categories: Utilities > Data Conversion
typedef unsigned char u8;
typedef unsigned int u32;

/**
 *  @brief Expands groups of 15 source bytes into 16-byte destination groups.
 *
 *  Each group copies its first byte, inserts a zero, then copies 14 bytes.
 *  A nonzero count not divisible by 15 still processes a complete final group.
 *  @param dst Destination buffer for the expanded groups.
 *  @param src Source buffer containing complete 15-byte groups.
 *  @param count Source-byte count used as the loop bound.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Iterates in steps of 15 until the loop index reaches or exceeds count.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes 16 bytes per iteration to dst.
 *  @par Callers
 *  Confirmed: gMovieAudioChunkPushSplit.
 */
void func_8008731C(u8 *dst, u8 *src, u32 count)
{
    u32 i;
    for (i = 0; i < count; i += 15) {
        *dst++ = *src++;
        *dst++ = 0;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}
