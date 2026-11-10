extern unsigned short *D_8009EEA4;
extern int D_8009EECC;

int func_800978AC(int index, int mode)
{
    register unsigned int value __asm__("$4") = D_8009EEA4[index];
    int result;
    if (mode != -1)
        result = value << D_8009EECC;
    else
        result = value;
    return result;
}
