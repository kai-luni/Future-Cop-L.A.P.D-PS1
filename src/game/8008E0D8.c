extern int D_8009C8B0;

int func_8008E0D8(int value)
{
    register int *base __asm__("$1");
    int previous = D_8009C8B0;
    __asm__ volatile("" : "+r"(previous) : : "$1", "memory");
    base = (int *)0x800A0000;
    __asm__("" : "+r"(base));
    base[-0xDD4] = value;
    return previous;
}
