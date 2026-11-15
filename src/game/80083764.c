// Categories: Graphics > Renderer > Map Cell Geometry | Utilities > Mathematics > Geometry
/**
 * @brief Builds a 4x4 subcell coverage mask for a world-space rectangle
 *        inside one map cell.
 *
 * @param x, y The cell's world origin.
 * @param left, right, top, bottom The rectangle edges in world units.
 * @return A 16-bit mask, bit 0x8000 >> (row * 4 + col) set for each covered
 *         subcell.
 *
 * @par Confirmed
 * Each edge is quantised: the cell index is edge >> 17, the subcell index
 * (edge >> 15) & 3. An edge outside the cell clamps to subcell 0 (left/top)
 * or 3 (right/bottom); an edge in a different cell clamps the whole side.
 * The mask loops i = left + top * 4 .. 15 and sets the bit when the subcell
 * lies inside all four bounds.
 *
 * @par Likely role
 * The coverage test of the map renderer: func_80049980 and the model binder
 * func_80083A10 use it to decide which of the 16 subcells a primitive
 * touches, so only those are emitted.
 *
 * @par Callers
 * func_80049980, func_80083A10.
 */
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
