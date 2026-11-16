// Categories: Physics > Collision | Gameplay > Map
typedef unsigned char u8;
typedef signed char s8;
extern u8 D_8009AFE8[][4];
extern u8 D_8009B1A4[];

/**
 * @brief Samples and interpolates a terrain surface height within a map sub-cell.
 *
 * Fetches corner indices from D_8009AFE8[index] and signed heights from data. For index >= 60 (quads) bilinearly interpolates all four corners; for index < 60 (triangles) dispatches on D_8009B1A4[index] >> 3 (0..3) to test triangle half-plane bounds before interpolating across the active corners into *out.
 *
 * @param data Tile corner height table (s8 array in the map block).
 * @param index Surface descriptor type index (0..59 triangles, >= 60 quads).
 * @param x 16-bit fractional X coordinate within the sub-cell.
 * @param y 16-bit fractional Z coordinate within the sub-cell.
 * @param out Destination receiving the interpolated height when inside the surface.
 * @return 1 when sampled (quad or inside triangle bounds), 0 when outside the triangle.
 *
 * @par Globals
 * D_8009AFE8 - 4-byte corner index table per surface type; D_8009B1A4 - triangle orientation byte table.
 * @par Control flow
 * Branch on index >= 60 vs 4-way switch on D_8009B1A4[index] >> 3 with early return 0 outside each triangle half-plane.
 * @par Calls
 * None.
 * @par Side effects
 * Writes *out when returning 1.
 * @par Callers
 * Confirmed: func_80066F78 (0x80067120), func_80067684 (0x800677CC) / Likely: none / Unknown: none.
 */
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
