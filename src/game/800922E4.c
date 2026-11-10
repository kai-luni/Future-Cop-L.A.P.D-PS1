extern unsigned char D_80010000[];
extern void (*D_8009DDEC)(void *, unsigned int, unsigned int);

void func_800922E4(unsigned short value)
{
    unsigned int low = value & 0x3f;
    unsigned int high = value;
    unsigned char *address;
    __asm__("" : : "r"(low), "r"(high));
    address = D_80010000;
    __asm__("" : "=r"(address) : "0"(address));
    D_8009DDEC(address + 0x28b4, low << 4, high >> 6);
}
