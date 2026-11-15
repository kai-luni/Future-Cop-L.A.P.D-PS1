// Categories: Physics > Motion | Graphics > Effects > Particles
typedef unsigned char u8;
/**
 * @brief Advances a segment position by per-axis deltas with clamping and
 *        boundary-hit flags.
 *
 * @param a Bounds record: +0x8C mode flag, +0x8F..+0x91 the lower bounds,
 *        +0x92..+0x94 the upper bounds (x/y/z).
 * @param b Segment: +0x14 flag byte, +0x15..+0x17 the position bytes.
 * @param x, y, z Per-axis deltas.
 * @return Nothing.
 *
 * @par Confirmed
 * Each non-zero delta adds to its position byte; crossing a bound clamps
 * to it and either sets the axis bit (1/2/4) in b[0x14] or, when a[0x8C]
 * bit 1 is set, toggles the bounce bit (8/16/32) instead.
 *
 * @par Likely role
 * The bounded drift of the trail segments behind func_800437E8/
 * func_80043CA8: mode bit 1 selects bounce (toggle) over stick (set), the
 * bounds likely describe the emitter's local box.
 *
 * @par Callers
 * func_800437E8, func_80043CA8.
 */
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
