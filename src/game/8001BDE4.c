extern unsigned int func_8001BC44(unsigned int, unsigned int, unsigned int);

unsigned int func_8001BDE4(unsigned int color)
{
    unsigned int r, g, b;
    r = ((unsigned short)color >> 10) & 0x1F;
    r = (r << 3) | (r >> 2);
    g = ((unsigned short)color >> 5) & 0x1F;
    g = (g << 3) | (g >> 2);
    b = color & 0x1F;
    b = (b << 3) | (b >> 2);
    return func_8001BC44(r, g, b);
}
