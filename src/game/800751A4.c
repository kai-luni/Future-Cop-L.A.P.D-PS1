typedef unsigned char u8;
typedef int s32;

u8 func_800751A4(u8 *base, s32 d1, s32 d2, s32 d3, s32 a5, u8 a6, u8 a7)
{
    register s32 ret __asm__("$8") = 3;
    register s32 old __asm__("$9");
    register s32 target __asm__("$10") = a5;
    register s32 lim __asm__("$2");
    register s32 masked __asm__("$3");
    __asm__ volatile("" : "=r"(target) : "0"(target));
    lim = target;

    if (d1 > 0) {
        old = base[0x15];
        masked = (u8)lim;
        __asm__ volatile("" : "=r"(masked) : "0"(masked));
        if (old + d1 >= masked) goto clamp1;
        base[0x15] = old + d1;
        goto next1;
    }
    if (d1 >= 0) goto zero1;
    old = base[0x15];
    if (old + d1 > (u8)lim) goto store1;
clamp1:
    ret = 2;
    base[0x15] = target;
    goto next1;
store1:
    base[0x15] = old + d1;
    goto next1;
zero1:
    ret = 2;
next1:
    __asm__ volatile("" : : "r"(old));

    if (d2 > 0) {
        u8 old2 = base[0x16];
        if (old2 + d2 >= a6) {
            ret--;
            base[0x16] = a6;
        } else {
            base[0x16] = old2 + d2;
        }
    } else if (d2 < 0) {
        u8 old2 = base[0x16];
        if (a6 >= old2 + d2) {
            ret--;
            base[0x16] = a6;
        } else {
            base[0x16] = old2 + d2;
        }
    } else {
        ret--;
    }

    if (d3 > 0) {
        u8 old3 = base[0x17];
        if (old3 + d3 >= a7) {
            ret--;
            base[0x17] = a7;
        } else {
            base[0x17] = old3 + d3;
        }
    } else if (d3 < 0) {
        u8 old3 = base[0x17];
        if (a7 >= old3 + d3) {
            ret--;
            base[0x17] = a7;
        } else {
            base[0x17] = old3 + d3;
        }
    } else {
        ret--;
    }

    return ret;
}
