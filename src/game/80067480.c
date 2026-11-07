typedef unsigned char u8;
typedef signed char s8;
extern u8 D_8009AFE8[][4];
extern u8 D_8009B1A4[];

int func_80067480(s8 *data, unsigned int index, int x, int y, int *out)
{
    int a, b, c, d;
    int left, right;
    int mode;
    u8 *p = D_8009AFE8[index];
    a = data[p[0]];
    b = data[p[1]];
    c = data[p[2]];
    if (index >= 60) {
        d = data[p[3]];
        left = (a << 7) + (((b - a) * x) >> 9);
        right = (d << 7) + (((c - d) * x) >> 9);
    } else {
        mode = D_8009B1A4[index];
        __asm__("" : "+r"(mode));
        switch (mode >> 3) {
        case 0:
            if (y + x > 65536) return 0;
            left = (a << 7) + (((b - a) * x) >> 9);
            right = (c << 7) + (((b - c) * x) >> 9);
            break;
        case 1:
            if (y + x <= 65535) return 0;
            left = (c << 7) + (((a - c) * x) >> 9);
            right = (c << 7) + (((b - c) * x) >> 9);
            break;
        case 2:
            if (y < x) return 0;
            left = (a << 7) + (((b - a) * x) >> 9);
            right = (c << 7) + (((b - c) * x) >> 9);
            break;
        case 3:
            if (x < y) return 0;
            left = (a << 7) + (((b - a) * x) >> 9);
            right = (a << 7) + (((c - a) * x) >> 9);
            break;
        }
    }
    *out = (left << 1) + (((right - left) * y) >> 15);
    return 1;
}
