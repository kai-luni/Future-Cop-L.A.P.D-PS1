// Categories: Graphics > Video Decode | Resources > Decompression
#include "game/types.h"
extern u8 *gMovieVlcTableA __asm__("D_8009F51C");
extern u8 *gMovieVlcTableB __asm__("D_8009F520");

/**
 * @brief Decodes one compressed bit stream into code words using the tables
 *        built by func_80089A88().
 *
 * @param input  Compressed stream: two 16-bit words are copied verbatim, then a
 *               10-bit prefix and the bit stream follow.
 * @param output Destination for the decoded 16-bit words.
 *
 * @return 0 on success.
 *
 * @par Confirmed flow
 * 1. The first two words are copied to the output, the third becomes the
 *    10-bit prefix (shifted left by 10) and words 4/5 are combined into the
 *    bit reservoir (bits = (raw << 16) | code).
 * 2. The outer loop reads a 9-bit code; 0x1FF terminates the stream.
 * 3. Each code is refined in the inner loop through the length table: codes
 *    below 32 consume 8 more bits, longer codes take their length directly; the
 *    symbol is looked up in the value table (length table base + 0x2000 or the
 *    second table at +0x200).
 * 4. Two escapes are handled: 0x7C1F means "next 16 bits are a literal word"
 *    and 0xFE00 ends the stream.
 * 5. The output is padded with 0xFE00 up to a total of 64 words.
 *
 * @par Globals
 * gMovieVlcTableA/gMovieVlcTableB (decode tables, read-only here).
 *
 * @par Callers
 * func_800861E4, after func_80089A88 built the tables.
*
 * @par Matching note
 * Every working value is pinned to a retail register ($2-$10, $24, $25); the
 * shift/refill sequence is what reproduces the retail instruction order.
 */
int func_80089BBC(u16 *input, u16 *output)
{
    register u16 *src __asm__("$9") = input;
    register unsigned int count __asm__("$8") = 0;
    register u8 *table1 __asm__("$12") = gMovieVlcTableA;
    register u8 *table2 __asm__("$11") = gMovieVlcTableB;
    register u16 *values1 __asm__("$14") = (u16 *)(table1 + 0x2000);
    register u16 *values2 __asm__("$13") = (u16 *)(table2 + 0x200);
    register unsigned int bits __asm__("$6");
    register unsigned int prefix __asm__("$3");
    register unsigned int code __asm__("$4");
    register unsigned int value __asm__("$7");
    register u8 *length __asm__("$10");
    register u16 *valptr __asm__("$7");
    register unsigned int tmp __asm__("$4");
    register unsigned int raw __asm__("$2");
    register unsigned int tmp7 __asm__("$7");
    *output++ = src[0];
    *output++ = src[1];
    prefix = src[2];
    __asm__("" : "+r" (prefix));
    raw = src[4];
    code = src[5];
    src += 6;
    prefix <<= 10;
    __asm__("" : "+r" (raw), "+r" (code), "+r" (prefix));
    bits = (raw << 16) | code;
    code = bits >> 22;
outer:
    if (code == 0x1ff) goto finish;
    {
        count += 10;
        bits <<= 10;
        if (count & 0x10) {
            count &= 15;
            tmp7 = *src++;
            tmp7 <<= count;
            __asm__("" : "+r" (tmp7));
            bits |= tmp7;
        } else count &= 15;
        *output++ = code | prefix;
inner:
        {
            code = bits >> 19;
            __asm__("" : "+r" (code));
            length = table1 + code;
            if (code < 32) {
                count += 8;
                bits <<= 8;
                if (count & 16) {
                    count &= 15;
                    tmp = *src++;
                    __asm__("" : "+r" (tmp));
                    bits |= tmp << count;
                } else count &= 15;
                code = bits >> 23;
                __asm__("" : "+r" (code));
                length = table2 + code;
                raw = code << 1;
                __asm__("" : "+r" (raw));
                valptr = (u16 *)((u8 *)values2 + raw);
            } else {
                raw = code << 1;
                __asm__("" : "+r" (raw));
                valptr = (u16 *)((u8 *)values1 + raw);
            }
            value = *valptr;
            __asm__("" : "+r" (value));
            bits <<= *length;
            count += *length;
            if (count & 16) {
                count &= 15;
                tmp = *src++;
                    __asm__("" : "+r" (tmp));
                    bits |= tmp << count;
            } else count &= 15;
            if (value != 0x7c1f) {
                *output++ = value;
                if (value == 0xfe00) { code = bits >> 22; goto outer; }
                goto inner;
            } else {
                tmp = bits >> 16;
                __asm__("" : "+r" (tmp));
                *output++ = tmp;
                tmp = *src++;
                __asm__("" : "+r" (tmp));
                bits <<= 16;
                    __asm__("" : "+r" (tmp));
                    bits |= tmp << count;
            }
        }
        goto inner;
    }
finish:
    bits = 64;
    raw = 0xfe00;
    __asm__("" : "+r" (raw), "+r" (bits));
    do { *output++ = raw; bits--; } while (bits != 0);
    __asm__("" : "+r" (bits));
    return 0;
}
