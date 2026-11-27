// Categories: Physics > Spatial Queries
typedef struct { short x, y, z, sx, sy, sz; } Box;
typedef struct { int x, y, z; } Vec;
int gFindFirstOverlappingModelBox(Box *input, Vec *pos, int radius, int count, int input_stride) __asm__("func_800618CC");


/**
 *  @brief Finds the first box containing a position within expanded bounds.
 *
 *  Box coordinates and extents are scaled by 16 before comparison with pos.
 *  Each axis uses a strict absolute-distance comparison against extent plus radius.
 *  @param input First box in the strided array.
 *  @param pos Position to test against the boxes.
 *  @param radius Amount added to each scaled box extent.
 *  @param count Number of boxes in the array.
 *  @param input_stride Byte distance between consecutive boxes.
 *  @return Index of the first matching box, or -1 if none matches.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Skips index zero when count is at least two, then tests axes in x, y, z order.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80061CAC.
 */
int gFindFirstOverlappingModelBox(Box *input, Vec *pos, int radius, int count, int input_stride)
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
