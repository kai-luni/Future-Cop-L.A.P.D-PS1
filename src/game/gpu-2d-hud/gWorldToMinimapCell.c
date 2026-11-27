// Categories: Graphics > Camera > Minimap | Gameplay > Map
struct MapBounds {
    int x;
    int unused04;
    int z;
    int unused0c;
    int x_scale;
    int z_scale;
};

extern int gMapGridHeader __asm__("D_8009F80C");
extern struct MapBounds gMapGridBounds __asm__("D_800A2950");
void gWorldToMinimapCell(int *out_x, int *out_z, int x, int z) __asm__("func_800694C8");


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
 *
 * @par Environment path
 * Converts a world x/z position into cell indices of the 54x54 map grid:
 * origin x/z from D_800A2950 is subtracted, each axis is multiplied by its
 * scale word >> 8, rounded with +0x800000 and shifted right by 24, then
 * clamped to 0..53; both outputs are 0 when no GRDB grid is loaded
 * (D_8009F80C == 0). The origin and scale words are filled by the GRDB loader
 * func_80063DC8 from the map bounds with a 0x360000 (54 << 16) span factor.
 * Callers are gDrawMapView (func_8006680C), gDrawRadarBlip (func_80065834)
 * and func_8005AB8C, so it serves the map/radar overlay each time a marker is
 * placed.
 */
void gWorldToMinimapCell(int *out_x, int *out_z, int x, int z)
{
    if (gMapGridHeader) {
        x -= gMapGridBounds.x;
        x *= gMapGridBounds.x_scale >> 8;
        z -= gMapGridBounds.z;
        z *= gMapGridBounds.z_scale >> 8;
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
