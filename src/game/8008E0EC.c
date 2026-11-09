extern void *D_8009C8CC[];

void *func_8008E0EC(unsigned char index)
{
    if (index >= 28) {
        char *base = (char *)0x80010000;
        __asm__("" : "=r"(base) : "0"(base));
        return base + 0x233C;
    }
    return D_8009C8CC[index];
}
