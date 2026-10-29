typedef unsigned short u16;

extern u16 D_800A0B70[];

int func_80028FD4(void)
{
    if (D_800A0B70[0x20] == 0) {
        return 0;
    }
    return (D_800A0B70[0x1F] << 10) / D_800A0B70[0x20];
}
