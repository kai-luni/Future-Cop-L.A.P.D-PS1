void func_8008AE7C(unsigned char *state, int a, int b, int c)
{
    state[0x36] = a;
    *(int *)(state + 0x2c) = b;
    state[0x35] = c;
}
