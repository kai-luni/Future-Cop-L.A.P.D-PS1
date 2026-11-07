extern unsigned int D_8009DD64[];
extern unsigned int *D_8009DD60;

unsigned int func_80091E8C(int arg0, unsigned int arg1)
{
    register int index __asm__("$6") = arg0;
    register unsigned int value __asm__("$4");
    register unsigned int *base __asm__("$3");
    unsigned int old;
    register unsigned int result __asm__("$2");
    __asm__ volatile("" : "=r"(index) : "0"(index));
    value = arg1;
    base = D_8009DD64;
    old = base[index];
    result = old;
    if (value != old) {
        if (value) {
            register unsigned int tmp __asm__("$4");
            register int shift __asm__("$3");
            unsigned int bits;
            base[index] = value;
            tmp = *D_8009DD60 & 0xFFFFFF;
            shift = index + 16;
            __asm__ volatile("" : "+r"(tmp), "+r"(shift));
            bits = (1 << shift) | 0x800000;
            __asm__ volatile("" : "+r"(bits));
            *D_8009DD60 = tmp | bits;
            __asm__ volatile("" : : : "memory");
        } else {
            register unsigned int *ptr __asm__("$5") = D_8009DD60;
            register int shift __asm__("$4");
            register unsigned int mask __asm__("$2") = 0xFF0000;
            unsigned int tmp;
            __asm__ volatile("" : "+r"(value), "+r"(mask));
            __asm__ volatile("" : "+r"(ptr));
            mask |= 0xFFFF;
            base[index] = 0;
            tmp = (*ptr & mask) | 0x800000;
            shift = index + 16;
            __asm__ volatile("" : "+r"(ptr), "+r"(shift), "+r"(tmp));
            *ptr = tmp & ~(1 << shift);
        }
        result = old;
        __asm__ volatile("" : "+r"(result));
    }
    return result;
}
