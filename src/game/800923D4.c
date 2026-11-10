void func_800923D4(unsigned int *p, unsigned int value)
{
    *p = (*p & 0xff000000U) | (value & 0x00ffffffU);
}
