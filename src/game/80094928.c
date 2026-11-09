extern unsigned int *D_8009DEC8;
extern unsigned char D_800A62C8[];

void func_80094928(unsigned int value)
{
    register unsigned char *base asm("$1");
    unsigned int index;
    *D_8009DEC8 = value;
    index = value >> 24;
    asm volatile ("lui %0, %%hi(D_800A62C8)\n\taddu %0, %0, %1" : "=r" (base) : "r" (index));
    base[0x62c8] = value;
}
