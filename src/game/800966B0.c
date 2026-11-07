typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef union { u64 whole; struct { u32 lo, hi; } words; } Pair;
extern u64 func_80096748(u32, u32, u32, u32, int);

u64 func_800966B0(u32 a, u32 b, u32 c, u32 d)
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
    result.whole = func_80096748(a, b, c, d, 0);
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
