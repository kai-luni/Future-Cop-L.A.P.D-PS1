void func_80092398(unsigned int *a, unsigned int b, unsigned int *c)
{
    register unsigned int low __asm__("$7") = 0x00FFFFFFU;
    register unsigned int high __asm__("$8") = 0xFF000000U;
    __asm__("" : "+r"(low), "+r"(high));
    *c = (*c & high) | (*a & low);
    *a = (*a & high) | (b & low);
}
