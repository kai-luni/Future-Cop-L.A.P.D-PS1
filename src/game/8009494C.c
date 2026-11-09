int func_8009494C(int index)
{
    register unsigned char *ptr asm("$2") = (unsigned char *)0x800A0000U;
    asm("" : "+r"(ptr));
    ptr += index;
    return ptr[0x62C8];
}
