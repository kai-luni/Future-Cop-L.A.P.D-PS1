#include "game/types.h"
extern u8 *D_8009F51C;
extern u8 *D_8009F520;

int func_80089BBC(u16 *input, u16 *output)
{
    register u16 *src __asm__("$9") = input;
    register unsigned int count __asm__("$8") = 0;
    register u8 *table1 __asm__("$12") = D_8009F51C;
    register u8 *table2 __asm__("$11") = D_8009F520;
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
