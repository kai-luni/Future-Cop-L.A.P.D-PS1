void func_80092410(unsigned char *obj, int enabled)
{
    unsigned char value;

    if (enabled) {
        value = obj[7] | 2;
    } else {
        value = obj[7] & 0xFD;
    }
    obj[7] = value;
}
