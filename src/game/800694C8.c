struct MapBounds {
    int x;
    int unused04;
    int z;
    int unused0c;
    int x_scale;
    int z_scale;
};

extern int D_8009F80C;
extern struct MapBounds D_800A2950;

void func_800694C8(int *out_x, int *out_z, int x, int z)
{
    if (D_8009F80C) {
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
