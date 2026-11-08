int func_80018580(int x, int y)
{
    unsigned char flags;
    int ax, ay;
    if (x == 0 && y == 0) return 0;
    flags = (unsigned int)x >> 31;
    ax = ((x >> 31) + x) ^ (x >> 31);
    ay = ((y >> 31) + y) ^ (y >> 31);
    if (y < 0) flags |= 2;
    if (ax < ay) flags |= 4;
    switch (flags) {
    case 7: return -64 - ((ax << 5) / ay);
    case 6: return ((ax << 5) / ay) - 64;
    case 5: return ((ax << 5) / ay) + 64;
    case 4: return 64 - ((ax << 5) / ay);
    case 3: return ((ay << 5) / ax) - 128;
    case 2: return -((ay << 5) / ax);
    case 1: return 128 - ((ay << 5) / ax);
    case 0: return (ay << 5) / ax;
    }
    return 0;
}
