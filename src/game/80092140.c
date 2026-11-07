typedef struct {
    short x, y, z, size;
    short x2, y2;
    short a, b, c, d;
    short value;
    unsigned char flag, small, pad, e, f, g;
} Object;

extern int func_80091F74(void);

Object *func_80092140(Object *obj, int x, int y, int z, int size)
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
