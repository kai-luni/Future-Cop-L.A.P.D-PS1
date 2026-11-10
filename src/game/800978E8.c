extern unsigned int *D_8009EEB4;

void func_800978E8(int arg0)
{
    *D_8009EEB4 &= 0xFFF8FFFFU;
    if (arg0) {
        *D_8009EEB4 |= 0x30000U;
    } else {
        *D_8009EEB4 |= 0x50000U;
    }
}
