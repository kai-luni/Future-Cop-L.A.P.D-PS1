// Categories: Graphics > Camera > Minimap | Gameplay > Map
struct MapBounds {
    int x;
    int unused04;
    int z;
    int unused0c;
    int x_scale;
    int z_scale;
};

extern int gTileMapTable __asm__("D_8009F80C");
extern struct MapBounds D_800A2950;

/**
 * @brief Converts a world position into 0..53 minimap cell coordinates.
 *
 * @param out_x, out_z Out-parameters (cell indices).
 * @param x, z World coordinates.
 * @return Nothing; both outputs are 0 when no map is loaded (D_8009F80C == 0).
 *
 * @par Confirmed
 * The position is translated by the map origin D_800A2950, scaled by
 * (x_scale >> 8) and (z_scale >> 8), biased by 0x800000, shifted right by 24 and
 * then clamped to 0..53 - a 54x54 cell grid.
 *
 * @par Likely role
 * The minimap/radar projection: 54 cells is a typical 2.3 km grid at the map
 * scale the game uses.
 *
 * @par Callers
 * func_80019310, func_800336DC, func_8004E5F0 and the map overlay code.
 */
void func_800694C8(int *out_x, int *out_z, int x, int z)
{
    if (gTileMapTable) {
        x -= D_800A2950.x;
        x *= D_800A2950.x_scale >> 8;
        z -= D_800A2950.z;
        z *= D_800A2950.z_scale >> 8;
        x += 0x800000;
        x >>= 24;
        z += 0x800000;
        z >>= 24;
        if (x >= 0) {
            if (x < 54) {
                *out_x = x;
            } else {
                *out_x = 53;
            }
        } else {
            *out_x = 0;
        }
        if (z >= 0) {
            if (z < 54) {
                *out_z = z;
            } else {
                *out_z = 53;
            }
        } else {
            *out_z = 0;
        }
    } else {
        *out_z = 0;
        *out_x = 0;
    }
}
