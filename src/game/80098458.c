#include "game/types.h"
extern s32 D_8009EA3C, D_8009EA40, D_8009EECC, D_8009EED4;
extern s32 D_8009EF04, D_8009EF08;
extern s32 *D_8009EF0C;
extern void func_80097DF8(void);
extern s32 func_800980F8(s32, s32, s32);

s32 func_80098458(s32 arg, s32 size)
{
    s32 reserve, mask, i;
    register s32 address __asm__("$6");
    register s32 remaining __asm__("$6");
    register s32 sum __asm__("$7");
    register s32 count __asm__("$2");
    register s32 gap __asm__("$5");
    s32 x;
    u32 start;
    s32 *p;
    if (D_8009EA3C == 0) reserve = 0;
    else reserve = (0x10000 - D_8009EA40) << D_8009EECC;
    address = arg;
    mask = D_8009EED4;
    if (size & ~mask) size += mask;
    size >>= D_8009EECC;
    size <<= D_8009EECC;
    x = address;
    if (x & ~mask) x += mask;
    x >>= D_8009EECC;
    x <<= D_8009EECC;
    if (x < 0x1010) return -1;
    sum = x + size;
    if (sum > (0x10000 << D_8009EECC) - reserve) return -1;
    p = D_8009EF0C;
    if (p[0] & 0x40000000) {
        register s32 *hi __asm__("$1");
        gap = x - 0x1010;
        { register s32 temp __asm__("$2");
          temp = p[1] + 0x1010;
          temp -= x;
          remaining = temp - size; }
        if (gap > 0) {
            if (D_8009EF04 - 2 < D_8009EF08) return -1;
            p[0] = 0x80001010;
            p[4] = sum | 0x40000000;
            __asm__ volatile ("" : : "r" (sum) : "memory");
            count = 2;
            __asm__ volatile ("" : "=r" (count) : "0" (count));
            p[1] = gap;
            p[2] = x;
            p[3] = size;
            p[5] = remaining;
        } else {
            if (D_8009EF04 - 1 < D_8009EF08) return -1;
            p[2] = sum | 0x40000000;
            __asm__ volatile ("" : : "r" (sum) : "memory");
            count = 1;
            __asm__ volatile ("" : "=r" (count) : "0" (count));
            p[0] = x;
            p[1] = size;
            p[3] = remaining;
        }
        __asm__ volatile ("lui %0,%%hi(D_8009EF08)" : "=r" (hi) : : "memory");
        *(s32 *)((char *)hi - 4344) = count;
    } else {
        func_80097DF8();
        i = 0;
        for (;;) {
            register s32 cond __asm__("$2");
            __asm__ volatile ("" : "=r" (i) : "0" (i) : "memory");
            start = D_8009EF0C[i * 2];
            if ((s32)start < 0) {
                start &= 0x0fffffff;
                if ((u32)x >= start) {
                    if (!((u32)x < start + D_8009EF0C[i * 2 + 1] ||
                        (D_8009EF0C[i * 2 + 2] & 0x40000000))) goto NEXT;
                    {
                        register s32 *base __asm__("$2");
                        register s32 *q __asm__("$3");
                        __asm__ volatile ("" : "=r" (i) : "0" (i));
                        base = *(s32 * volatile *)&D_8009EF0C;
                        q = (s32 *)((i * 8) + (s32)base);
                        __asm__ ("" : "=r" (q) : "0" (q));
                        {
                            register s32 value __asm__("$2");
                            register s32 length __asm__("$3");
                            value = q[0];
                            length = q[1];
                            __asm__("" : "=r" (value), "=r" (length) : "0" (value), "1" (length));
                            value &= 0x0fffffff;
                            value = x - value;
                            remaining = length - value;
                        }
                    }
                    __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                    if (cond) goto NEXT;
                    start = x;
                    break;
                } else {
                    remaining = D_8009EF0C[i * 2 + 1];
                    __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                    if (!cond) break;
                    i++;
                    continue;
                }
            } else {
                if (start & 0x40000000) goto FINAL;
            NEXT:
                __asm__ volatile ("" : : "r" (i));
                i++;
                continue;
            FINAL:
                {
                    register s32 value __asm__("$2");
                    value = D_8009EF0C[i * 2 + 1];
                    __asm__("" : "=r" (value) : "0" (value));
                    remaining = value - reserve;
                }
                __asm__("slt %0,%1,%2" : "=r" (cond) : "r" (remaining), "r" (size));
                if (cond) return -1;
                start &= 0x0fffffff;
                break;
            }
        }
        x = func_800980F8(i, start, size);
    }
    func_80097DF8();
    return x;
}
