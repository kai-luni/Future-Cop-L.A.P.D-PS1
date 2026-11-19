// Categories: Graphics > GPU > Drawing Area | Graphics > 2D Drawing
typedef struct {
    short x, y, z, size;
    short x2, y2;
    short a, b, c, d;
    short value;
    unsigned char flag, small, pad, e, f, g;
} Object;

extern int func_80091F74(void);
Object *gSetDefDrawEnv(Object *obj, int x, int y, int z, int size) __asm__("func_80092140");


/**
 * @brief Initialises a drawing-area object at a position and size.
 *
 * @param obj Target object.
 * @param x, y, z Position.
 * @param size Extent; compared against 0x121 or 0x101 to derive the "small"
 *        flag.
 * @return The object itself (so calls can be chained).
 *
 * @par Confirmed
 * All fields are zeroed first (a/b/c/d and the three bytes e/f/g), flag = 1,
 * size stored, value = 10, pad = 0. The threshold for the small flag is 0x121
 * when the library's origin query func_80091F74 returns non-zero, and 0x101
 * otherwise - i.e. the paletted/small-page mode picks a tighter limit.
 *
 * @par Pairs with
 * func_800921F4 (the sibling initialiser) and func_800934BC (the applier).
 *
 * @par Environment path
 * Initializes a DRAWENV structure with its clip rectangle (x, y, z, size),
 * drawing offset, and default draw-mode/area/offset fields using
 * func_80091F74 (called by func_8001BE68, func_800773D8, func_8007757C,
 * func_80085C4C).
 */
Object *gSetDefDrawEnv(Object *obj, int x, int y, int z, int size)
{
    int mode = func_80091F74();
    obj->x = x;
    obj->y = y;
    obj->z = z;
    obj->a = 0;
    obj->b = 0;
    obj->c = 0;
    obj->d = 0;
    obj->e = 0;
    obj->f = 0;
    obj->g = 0;
    obj->flag = 1;
    obj->size = size;
    if (mode)
        obj->small = size < 0x121;
    else
        obj->small = size < 0x101;
    obj->x2 = x;
    obj->y2 = y;
    obj->value = 10;
    obj->pad = 0;
    return obj;
}
