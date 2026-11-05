#include "game/types.h"
typedef struct { u8 bits; u8 pad; u16 code; } Entry;
extern u8 *D_8009F51C, *D_8009F520;
extern u16 D_8009C560[];
extern Entry D_8009C1E0[], D_8009C360[];

s32 func_80089A88(u8 *arg0, u8 *arg1, u16 *arg2)
{
    register s32 i __asm__("$11");
    register s32 j __asm__("$7");
    register s32 count __asm__("$8");
    register s32 base __asm__("$9");
    register s32 bits __asm__("$10");
    register s32 value __asm__("$13");
    register u8 *ptr __asm__("$12");
    register u16 *codes __asm__("$14");
    register u16 *out __asm__("$15");
    register s32 maxbits __asm__("$25");
    register s32 one __asm__("$24");
    register u8 *addr __asm__("$2");
    register s32 code __asm__("$3");
    register s32 shift __asm__("$2");

    D_8009F51C = arg0;
    D_8009F520 = arg1;
    if (!arg0 || !arg1) return -1;
    out = (u16 *)(arg0 + 0x2000);
    if (!arg2) arg2 = D_8009C560;
    i = 0;
    maxbits = 13;
    one = 1;
    addr = (u8 *)D_8009C1E0;
    __asm__("" : "+r"(addr));
    codes = (u16 *)(addr + 2);
    ptr = addr;
    __asm__("" : "+r"(out), "+r"(maxbits), "+r"(one), "+r"(ptr), "+r"(codes));
    do {
        value = *arg2++;
        j = 0;
        bits = *ptr;
        code = *codes;
        shift = maxbits - bits;
        __asm__("" : "+r"(shift));
        count = one << shift;
        base = code >> 3;
        for (; j < count; j++) {
            arg0[base | j] = bits;
            out[base | j] = value;
        }
        codes += 2;
        i++;
        ptr += 4;
    } while (i < 0x60);
    {
        register u8 *dst __asm__("$4") = arg1;
        register s32 j2 __asm__("$5");
        register s32 count2 __asm__("$7");
        register s32 base2 __asm__("$8");
        register s32 bits2 __asm__("$9");
        register u8 *ptr2 __asm__("$10");
        register s32 value2 __asm__("$12");
        register u16 *codes2 __asm__("$13");
        register s32 one2 __asm__("$14");
        register s32 maxbits2 __asm__("$24");
        __asm__("" : "+r"(dst));
        out = (u16 *)(dst + 0x200);
        i = 0;
        maxbits2 = 9;
        one2 = 1;
        addr = (u8 *)D_8009C360;
        __asm__("" : "+r"(addr));
        codes2 = (u16 *)(addr + 2);
        ptr2 = addr;
        __asm__("" : "+r"(out), "+r"(maxbits2), "+r"(one2), "+r"(ptr2), "+r"(codes2));
        do {
            value2 = *arg2++;
            j2 = 0;
            bits2 = *ptr2;
            code = *codes2;
            shift = maxbits2 - bits2;
            __asm__("" : "+r"(shift));
            count2 = one2 << shift;
            base2 = code >> 7;
            for (; j2 < count2; j2++) {
                dst[base2 | j2] = bits2;
                out[base2 | j2] = value2;
            }
            codes2 += 2;
            i++;
            ptr2 += 4;
        } while (i < 0x80);
    }
    return 0;
}
