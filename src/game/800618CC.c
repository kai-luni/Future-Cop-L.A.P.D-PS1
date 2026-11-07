typedef struct { short x, y, z, sx, sy, sz; } Box;
typedef struct { int x, y, z; } Vec;

int func_800618CC(Box *input, Vec *pos, int radius, int count, int input_stride)
{
    register Box *box __asm__("$10") = input;
    register int i __asm__("$9");
    register int stride __asm__("$11") = input_stride;
    register short *end __asm__("$8");
    register int x __asm__("$12");
    register int delta __asm__("$4");
    register int value __asm__("$2");
    register int sign __asm__("$3");
    i = 0;
    if (count >= 2) {
        i = 1;
        box = (Box *)((char *)box + stride);
    }
    if (i < count) {
    x = pos->x;
    end = &box->sz;
    for (; i < count; i++, end = (short *)((char *)end + stride), box = (Box *)((char *)box + stride)) {
        value = box->x << 4;
        delta = x - value;
        sign = delta >> 31;
        value = ((sign + delta) ^ sign) < (end[-2] << 4) + radius;
        if (value) {
            value = end[-4] << 4;
            sign = pos->y;
            delta = sign - value;
            sign = delta >> 31;
            value = ((sign + delta) ^ sign) < (end[-1] << 4) + radius;
            if (value) {
                value = end[-3] << 4;
                sign = pos->z;
                delta = sign - value;
                sign = delta >> 31;
                value = ((sign + delta) ^ sign) < (end[0] << 4) + radius;
                if (value)
                    break;
            }
        }
    }
    }
    return i == count ? -1 : i;
}
