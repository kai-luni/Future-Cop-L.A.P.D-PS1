void func_8001BD2C(unsigned int color, unsigned int *dst)
{
    dst[0] += color & 0xFF0000;
    dst[1] += ((color >> 8) & 0xFF) << 16;
    dst[2] += (color & 0xFF) << 16;
}
