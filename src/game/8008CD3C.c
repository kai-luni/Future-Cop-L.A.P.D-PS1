// Categories: Utilities > String Formatting
#include "game/types.h"

typedef struct Spec {
    unsigned minus : 1;
    unsigned plus : 1;
    unsigned alt : 1;
    unsigned zero : 1;
    unsigned hasprec : 1;
    unsigned h : 1;
    unsigned l : 1;
    unsigned L : 1;
    signed char sign;
    s32 width;
    s32 prec;
} Spec;

extern Spec gDefaultFormatSpec __asm__("D_8009C7E0");
extern u8 gHexUpperTable[] __asm__("D_8001223C");
extern u8 gHexLowerTable[] __asm__("D_80012250");
extern s32 func_8008CCC0(u8 *s);
extern u8 *func_8008D5C0(u8 *s, s32 c, s32 n);
extern u8 *gCopyMemOverlap(u8 *dest, u8 *src, s32 count) __asm__("func_8008D5D0");

#define VA(T) ((T)*ap[0]++)
s32 gFormatStringBody(u8 *buf, signed char *fmt) __asm__("func_8008CD3C");


/**
 *  @brief Formats arguments into a null-terminated output buffer.
 *
 *  Parses printf-style flags, width, precision, modifiers, and supported conversions.
 *  Arguments are read from the calling convention's argument area after fmt.
 *  @param buf Destination buffer, which must have sufficient space.
 *  @param fmt Null-terminated format string.
 *  @return Number of output bytes written, excluding the terminator.
 *  @par Globals
 *  D_8009C7E0 supplies the initial conversion specification; D_80012250
 *  supplies lowercase hexadecimal digits, and the uppercase table is addressed at D_8001223C.
 *  @par Control flow
 *  Scans literals and conversions, builds numeric digits backward, and applies padding.
 *  Supports integer, pointer, character, string, count, and percent conversions;
 *  an unsupported conversion ends formatting, and %n writes the current count.
 *  @par Calls
 *  func_8008CCC0 measures strings; func_8008D5C0 searches bounded strings.
 *  func_8008D5D0 copies each formatted field into the output.
 *  @par Side effects
 *  Writes buf and any count destination supplied for %n; performs no buffer bounds check.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Implements the printf-style formatting body entered from func_8008CD30,
 * reading variadic arguments past fmt and writing the NUL-terminated result
 * into buf. Initializes each conversion Spec from D_8009C7E0, formats
 * d/i/u/o/p/X/x/c/s/n/%% conversions using D_80012250 and 0x8001223C hex
 * tables, and calls func_8008CCC0 (strlen), func_8008D5C0 (memchr), and
 * func_8008D5D0 to measure and copy fields.
 */
s32 gFormatStringBody(u8 *buf, signed char *fmt)
{
    u8 digits[0x200];
    Spec spec;
    s32 *ap[2];
    s32 i;
    s32 len;
    signed char *s;
    u32 n;
    s32 c;
    s32 t;
    signed char *fb;
    register s32 km __asm__("$23");
    register s32 kp __asm__("$22");
    register s32 ks __asm__("$21");
    register s32 kz __asm__("$19");

    ap[0] = (s32 *)(&fmt + 1);
    i = 0;
    c = *fmt;
    if (c != 0) {
        km = 45; kp = 43; ks = 32; kz = 48;
        __asm__ volatile("" : "+r"(km), "+r"(kp), "+r"(ks), "+r"(kz));
    }
    while (c != 0) {
        if (c != '%') {
            goto literal;
        }
        {
            register Spec *p __asm__("$5") = &gDefaultFormatSpec;
            __asm__ volatile(
                "lw\t$2,0(%3)\n\t"
                "lw\t$3,4(%3)\n\t"
                "lw\t$4,8(%3)\n\t"
                "sw\t$2,%0\n\t"
                "sw\t$3,%1\n\t"
                "sw\t$4,%2"
                : "=m"(*(u32 *)&spec), "=m"(spec.width), "=m"(spec.prec)
                : "r"(p)
                : "$2", "$3", "$4");
        }
        {
        register s32 kh __asm__("$3") = 35;
        __asm__ volatile("" : "+r"(kh));
        for (;;) {
            fb = fmt;
            c = *++fmt;
            __asm__ volatile("" : : : "$2");
            if (c == km) {
                spec.minus = 1;
            } else if (c == kp) {
                spec.plus = 1;
            } else if (c == ks) {
                spec.sign = c;
            } else if (c == kh) {
                spec.alt = 1;
            } else if (c == kz) {
                spec.zero = 1;
            } else {
                break;
            }
        }
        }
        if (c == '*') {
            spec.width = VA(s32);
            if (spec.width < 0) {
                spec.width = -spec.width;
                spec.minus = 1;
            }
            
            fmt = fb + 2;
            c = fb[2];
        } else {
            while ((u32)(c - '0') < 10) {
                t = spec.width * 10 - '0';
                    spec.width = t + c;
                c = *++fmt;
            }
        }
        if (c == '.') {
            signed char *db = fmt;
            c = *++fmt;
            __asm__ volatile("" : : : "$2");
            if (c == '*') {
                spec.prec = VA(s32);
                
                fmt = db + 2;
                c = db[2];
            } else {
                while ((u32)(c - '0') < 10) {
                    t = spec.prec * 10 - '0';
                    spec.prec = t + c;
                    c = *++fmt;
                }
            }
            if (spec.prec >= 0) {
                spec.hasprec = 1;
            }
        }
        s = (signed char *)digits + sizeof(digits);
        if (spec.minus) {
            spec.zero = 0;
        }
dispatch:
        {
            switch (c) {
            case 'h':
                spec.h = 1;
                goto modifier;
            case 'l':
                spec.l = 1;
                goto modifier;
            case 'L':
                spec.L = 1;
modifier:
                c = *++fmt;
                goto dispatch;
            case 'd':
            case 'i':
                n = VA(s32);
                __asm__ volatile("" : "+r"(n), "+m"(spec));
                if (spec.h) {
                    n = (s16)n;
                }
                if ((s32)n < 0) {
                    n = -n;
                    spec.sign = km;
                } else if (spec.plus) {
                    spec.sign = kp;
                }
                goto decimal;
            case 'u':
                n = VA(u32);
                __asm__ volatile("" : "+r"(n), "+m"(spec));
                if (spec.h) {
                    n = (u16)n;
                }
                spec.sign = 0;
decimal:
                if (!spec.hasprec) {
                    if (spec.zero) {
                        spec.prec = spec.width;
                        if (spec.sign) {
                            spec.prec = spec.width - 1;
                        }
                    }
                    if (spec.prec <= 0) {
                        spec.prec = 1;
                    }
                }
                len = 0;
                while (n != 0) {
                    *--s = n % 10 + '0';
                    n /= 10;
                    len++;
                }
                if (len < spec.prec) {
                    do {
                        *--s = kz;
                        len++;
                    } while (len < spec.prec);
                }
                if (spec.sign) {
                    *--s = (u8)spec.sign;
                    len++;
                }
                goto done;
            case 'o':
                n = VA(u32);
                __asm__ volatile("" : "+r"(n), "+m"(spec));
                if (spec.h) {
                    n = (u16)n;
                }
                if (!spec.hasprec) {
                    if (spec.zero) {
                        spec.prec = spec.width;
                    }
                    if (spec.prec <= 0) {
                        spec.prec = 1;
                    }
                }
                len = 0;
                while (n != 0) {
                    *--s = (n & 7) + '0';
                    n >>= 3;
                    len++;
                }
                if (spec.alt && len != 0 && *s != kz) {
                    *--s = kz;
                    len++;
                }
                if (len < spec.prec) {
                    do {
                        *--s = kz;
                        len++;
                    } while (len < spec.prec);
                }
                goto done;
            case 'p':
                spec.prec = 8;
                *(u32 *)&spec |= 0x50;
            case 'X':
                {
                    register u8 *hex __asm__("$7") = (u8 *)0x80010000;
                    __asm__("" : "+r"(hex));
                    hex += 0x223C;
                    goto hexcommon;
            case 'x':
                    hex = gHexLowerTable;
hexcommon:
                    n = VA(u32);
                    __asm__ volatile("" : "+r"(n), "+m"(spec));
                __asm__ volatile("" : "+r"(n), "+m"(spec));
                    if (spec.h) {
                        n = (u16)n;
                    }
                    if (!spec.hasprec) {
                        if (spec.zero) {
                            spec.prec = spec.width;
                            if (spec.alt) {
                                spec.prec = spec.width - 2;
                            }
                        }
                        if (spec.prec <= 0) {
                            spec.prec = 1;
                        }
                    }
                    len = 0;
                    while (n != 0) {
                        *--s = hex[n & 15];
                        n >>= 4;
                        len++;
                    }
                    if (len < spec.prec) {
                        do {
                            *--s = kz;
                            len++;
                        } while (len < spec.prec);
                    }
                    if (spec.alt) {
                        *--s = c;
                        *--s = kz;
                        len += 2;
                    }
                    goto done;
                }
            case 'c':
                *--s = *(u8 *)ap[0]++;
                len = 1;
                goto done;
            case 's':
                s = VA(signed char *);
                if (spec.alt) {
                    len = *(u8 *)s++;
                    if (spec.hasprec) {
                        if (spec.prec < len) {
                            len = spec.prec;
                        }
                    }
                } else if (!spec.hasprec) {
                    len = func_8008CCC0((u8 *)s);
                } else {
                    u8 *p = func_8008D5C0((u8 *)s, 0, spec.prec);
                    if (p != 0) {
                        len = (signed char *)p - s;
                    } else {
                        len = spec.prec;
                    }
                }
                goto done;
            case 'n':
                {
                    register u32 flags __asm__("$3");
                    register u32 h __asm__("$2");
                    s = VA(signed char *);
                    flags = *(u32 *)&spec;
                    __asm__ volatile("" : "+m"(ap[0]) : "r"(s), "r"(flags));
                    h = (flags >> 5) & 1;
                    __asm__("" : "+r"(h));
                    if (h) {
                        *(s16 *)s = i;
                    } else {
                        *(s32 *)s = i;
                    }
                }
                goto next;
            default:
                if (c != '%') {
                    goto end;
                }
                goto literal;
            }
        }
literal:
        buf[i++] = c;
        goto next;
done:
        if (len < spec.width) {
            if (!spec.minus) {
                do {
                    buf[i++] = ks;
                    spec.width--;
                } while (len < spec.width);
            }
        }
        gCopyMemOverlap(buf + i, (u8 *)s, len);
        i += len;
        while (len < spec.width) {
            buf[i++] = ks;
            len++;
        }
        goto next;
next:
        c = *++fmt;
    }
end:
    buf[i] = 0;
    return i;
}
