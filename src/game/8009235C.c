void func_8009235C(unsigned int *a0, unsigned int *a1)
{
    register unsigned int low asm("$6") = 0x00FFFFFFU;
    register unsigned int high asm("$7") = 0xFF000000U;
    asm("" : "+r" (low), "+r" (high));
    *a1 = (*a1 & high) | (*a0 & low);
    *a0 = (*a0 & high) | ((unsigned int)a1 & low);
}
