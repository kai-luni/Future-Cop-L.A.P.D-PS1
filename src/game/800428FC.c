typedef unsigned char u8;
void func_800428FC(u8 *a, u8 *b, int x, int y, int z)
{
    if (x > 0) {
        x += b[0x15];
        if (x >= a[0x92]) {
            b[0x15] = a[0x92];
            if (a[0x8c] & 2) b[0x14] ^= 8;
            else b[0x14] |= 1;
        } else b[0x15] = x;
    } else if (x < 0) {
        x += b[0x15];
        if (x <= a[0x8f]) {
            b[0x15] = a[0x8f];
            if (a[0x8c] & 2) b[0x14] ^= 8;
            else b[0x14] |= 1;
        } else b[0x15] = x;
    }
    if (y > 0) {
        y += b[0x16];
        if (y >= a[0x93]) {
            b[0x16] = a[0x93];
            if (a[0x8c] & 2) b[0x14] ^= 16;
            else b[0x14] |= 2;
        } else b[0x16] = y;
    } else if (y < 0) {
        y += b[0x16];
        if (y <= a[0x90]) {
            b[0x16] = a[0x90];
            if (a[0x8c] & 2) b[0x14] ^= 16;
            else b[0x14] |= 2;
        } else b[0x16] = y;
    }
    if (z > 0) {
        z += b[0x17];
        if (z >= a[0x94]) {
            b[0x17] = a[0x94];
            if (a[0x8c] & 2) b[0x14] ^= 32;
            else b[0x14] |= 4;
        } else b[0x17] = z;
    } else if (z < 0) {
        z += b[0x17];
        if (z <= a[0x91]) {
            b[0x17] = a[0x91];
            if (a[0x8c] & 2) b[0x14] ^= 32;
            else b[0x14] |= 4;
        } else b[0x17] = z;
    }
}
