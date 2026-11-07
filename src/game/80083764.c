unsigned short func_80083764(int x, int y, int left, int right, int top, int bottom)
{
    int i;
    int mask;

    if ((left >> 17) != (x >> 17))
        left = 0;
    else
        left = (left >> 15) & 3;
    if ((right >> 17) != (x >> 17))
        right = 3;
    else
        right = (right >> 15) & 3;
    if ((top >> 17) != (y >> 17))
        top = 0;
    else
        top = (top >> 15) & 3;
    if ((bottom >> 17) != (y >> 17))
        bottom = 3;
    else
        bottom = (bottom >> 15) & 3;
    i = left + top * 4;
    mask = 0;
    do {
        if ((i & 3) >= left && (i & 3) <= right &&
            (i >> 2) >= top && (i >> 2) <= bottom)
            mask |= 0x8000 >> i;
        i++;
    } while (i < 16);
    return mask;
}
