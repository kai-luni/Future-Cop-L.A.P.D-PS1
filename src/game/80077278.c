extern void func_800770A8(int, int, int);

void func_80077278(int x, int y, unsigned int value)
{
    int shift;
    for (shift = 28; shift >= 0; shift -= 4) {
        func_800770A8(x, y, (value >> shift) & 15);
        x += 7;
    }
}
