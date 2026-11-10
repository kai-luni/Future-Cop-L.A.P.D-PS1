typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_8009C94C[];
extern u8 D_8001233C[];

u8 *func_8008E120(u32 index)
{
    u32 i = index & 0xFF;

    if (i >= 7) {
        return D_8001233C;
    }
    return D_8009C94C[i];
}
