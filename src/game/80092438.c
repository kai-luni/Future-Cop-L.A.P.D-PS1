void func_80092438(unsigned char *p, int enabled)
{
    unsigned char value;

    if (enabled) {
        value = p[7] | 1;
    } else {
        value = p[7] & 0xFE;
    }
    p[7] = value;
}
