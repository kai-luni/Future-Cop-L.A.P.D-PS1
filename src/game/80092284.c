extern void (*D_8009DDEC)(char *, int, int, int, int);

void func_80092284(unsigned short value)
{
    unsigned int arg1 = value >> 7;
    unsigned int arg2 = value >> 5;
    unsigned int arg3 = value << 6;
    D_8009DDEC(({ char *address;
                   __asm__("lui %0,%%hi(D_8001289C)\n\taddiu %0,%0,%%lo(D_8001289C)" : "=r" (address) : "r" (arg1), "r" (arg2), "r" (arg3));
                   address; }), arg1 & 3, arg2 & 3,
               arg3 & 0x7C0, ((value << 4) & 0x100) | ((value >> 2) & 0x200));
}
