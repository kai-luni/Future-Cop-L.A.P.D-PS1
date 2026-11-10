typedef unsigned char u8;
typedef unsigned int u32;

int func_8009272C(u8 *a0, u8 *a1)
{
    int v;

    v = a0[3] + a1[3] + 1;
    if (v >= 0x11) {
        return -1;
    }
    a0[3] = v;
    *(u32 *)a1 = 0;
    return 0;
}
