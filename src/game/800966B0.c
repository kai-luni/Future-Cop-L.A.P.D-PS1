// Categories: Utilities > Integer Arithmetic
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef union { u64 whole; struct { u32 lo, hi; } words; } Pair;
extern u64 gDivModUnsigned64(u32, u32, u32, u32, int) __asm__("func_80096748");

u64 gDivSigned64(u32 a, u32 b, u32 c, u32 d) __asm__("func_800966B0");

/**
 *  @brief Applies signed operand and result handling around a 64-bit helper.
 *
 *  Treats each pair of words as a signed 64-bit operand, converts negative
 *  operands to magnitudes, and restores the result sign when they differ.
 *  @param a Low word of the first operand.
 *  @param b High word of the first operand.
 *  @param c Low word of the second operand.
 *  @param d High word of the second operand.
 *  @return The helper result, negated when the operand signs differ.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Two sign branches normalize operands; a final branch conditionally negates the result.
 *  @par Calls
 *  func_80096748 processes the normalized words with a final argument of zero.
 *  @par Side effects
 *  No direct state changes; effects of the helper are not shown here.
 *  @par Callers
 *  Confirmed: func_800867B0, func_80086F58.
 */
u64 gDivSigned64(u32 a, u32 b, u32 c, u32 d)
{
    register int sign __asm__("$18") = 0;
    register u32 xlo __asm__("$8");
    register u32 xhi __asm__("$9");
    register u32 ylo __asm__("$10");
    register u32 yhi __asm__("$11");
    register u32 tmp __asm__("$3");
    register u32 carry __asm__("$2");
    register u32 rlo __asm__("$16");
    register u32 rhi __asm__("$17");
    register Pair result __asm__("$2");
    if ((s32)b < 0) {
        sign = -1;
        xlo = -a;
        tmp = -b;
        __asm__ volatile("" : "+r"(xlo), "+r"(tmp));
        carry = xlo != 0;
        __asm__ volatile("" : "+r"(carry));
        xhi = tmp - carry;
        __asm__ volatile("" : "+r"(xlo), "+r"(xhi));
        a = xlo;
        b = xhi;
    }
    if ((s32)d < 0) {
        sign = ~sign;
        ylo = -c;
        tmp = -d;
        __asm__ volatile("" : "+r"(ylo), "+r"(tmp));
        carry = ylo != 0;
        __asm__ volatile("" : "+r"(carry));
        yhi = tmp - carry;
        __asm__ volatile("" : "+r"(ylo), "+r"(yhi));
        c = ylo;
        d = yhi;
    }
    result.whole = gDivModUnsigned64(a, b, c, d, 0);
    __asm__ volatile("" : "+r"(result));
    rlo = -result.words.lo;
    if (sign) {
        tmp = -result.words.hi;
        __asm__ volatile("" : "+r"(rlo), "+r"(tmp));
        carry = rlo != 0;
        __asm__ volatile("" : "+r"(carry));
        rhi = tmp - carry;
        __asm__ volatile("" : "+r"(rlo), "+r"(rhi));
        result.words.lo = rlo;
        result.words.hi = rhi;
    }
    __asm__ volatile("" : "+r"(result));
    return result.whole;
}
