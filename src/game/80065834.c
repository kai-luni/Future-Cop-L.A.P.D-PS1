#include "game/types.h"

struct Vec { s32 x, y, z; };
struct SVec { s16 x, y, z, pad; };
struct Bounds { s16 x, y, z, dx, unused, dz; u8 pad[4]; };
struct Model { u8 pad[0x34]; struct Bounds *bounds; };
struct Graphic { struct Model *model; u8 transform[0x3c]; s32 radius; u8 pad44[4]; s16 index; };
struct State { u8 pad[0x5c]; s32 mode; };
struct PacketSmall { u32 tag, color; s16 x, y; u32 size; };
struct PacketLarge { u32 tag, color; s16 x0, y0; u32 color1; s16 x1, y1; u32 color2; s16 x2, y2; };
extern struct State *D_8009FA94;
extern u8 *D_8009FA90, *D_8009F9C0;
extern u8 *D_8009F9F0, *D_8009FA28;
extern u32 D_800A6F60[];
extern u8 D_8009F37C[];
extern void func_800694C8(s32 *, s32 *, s32, s32);
extern void func_80010750(void *);
extern void func_80010234(struct SVec *, struct Vec *, s32);
extern void func_80049100(u8 *, s32, s32, s32);

void func_80065834(u8 *entity, struct Vec *min, struct Vec *max, s32 arg3)
{
    register s32 offset __asm__("$17");
    s32 sx, sy;
    struct SVec p[4];
    struct Vec v[4];
    struct Graphic *graphic;
    struct Bounds *bounds;
    register struct PacketSmall *small __asm__("$5");
    register u8 *smallbase __asm__("$4");
    struct PacketLarge *large;
    s32 type, flags;
    s32 radius;
    s32 pos;
    s32 dx;
    s32 dz;
    s32 x;
    s32 y;
    s32 z;
    s32 i, depth, xm;
    s32 color;
    register struct Vec *cur __asm__("$16");
    register u32 *colors __asm__("$3");
    register s32 index __asm__("$2");

    flags = entity[0x23];
    graphic = (struct Graphic *)(entity + 0x28);
    if ((flags >> 5) == 2 && entity[0x16] == 0x1c && D_8009FA94->mode == 1) {
        offset = 0;
        if (arg3) offset = 0x50;
        func_800694C8(&sx, &sy, *(s32 *)(entity + 0x58), *(s32 *)(entity + 0x50));
        index = flags & 31;
        __asm__ volatile ("" : : "r"(index) : "$3");
        colors = D_800A6F60;
        smallbase = D_8009FA90;
        color = colors[index];
        {
            register u32 test __asm__("$3");
            test = (u32)D_8009F9C0;
            test = (u32)smallbase < test;
            __asm__ volatile ("" : "=r"(test) : "0"(test));
            small = (struct PacketSmall *)smallbase;
            if (test) D_8009FA90 = (u8 *)small + 0x1c;
        }
        {
            register u32 size __asm__("$4");
            register s32 px __asm__("$2");
            register s32 py __asm__("$3");
            sx += offset;
            size = 0x00020000;
            __asm__ volatile ("" : "=r"(size) : "0"(size), "m"(sx));
            small->tag = 0x03000000;
            small->color = color | 0x60000000;
            __asm__ volatile ("" : : : "memory");
            px = (u16)sx;
            py = (u16)sy;
            __asm__ volatile ("" : "=r"(size) : "0"(size), "r"(px), "r"(py));
            small->size = size | 2;
            __asm__ volatile ("" : "=r"(px), "=r"(py) : "0"(px), "1"(py), "m"(small->size));
            small->x = px + 13;
            small->y = py + 17;
        }
    }
    type = entity[0x16];
    radius = graphic->radius;
    if (type == 1) {
        graphic = (struct Graphic *)(*(u8 **)(entity + 0xac) + 0x25c);
        radius = graphic->radius;
        radius += radius * 2;
    }
    pos = *(s32 *)(entity + 0x58);
    if (pos + radius < min->x || max->x < pos - radius) return;
    pos = *(s32 *)(entity + 0x50);
    if (pos + radius < min->y || max->y < pos - radius) return;
    bounds = &((struct Bounds *)((u8 *)graphic->model->bounds + 8))[graphic->index];
    dx = bounds->dx;
    dz = bounds->dz;
    x = bounds->x;
    y = bounds->y;
    z = bounds->z;
    if (type == 1) { dx *= 3; dz *= 3; }
    xm = x - dx;
    p[0].x = xm; p[0].y = y; p[0].z = z + dz;
    p[1].x = x + dx; p[1].y = y; p[1].z = z + dz;
    p[2].x = x + dx; p[2].y = y; p[2].z = z - dz;
    p[3].x = xm; p[3].y = y; p[3].z = z - dz;
    if (entity[0x16] == 1) graphic = (struct Graphic *)(entity + 0x28);
    func_80010750(graphic->transform);
    {
        register struct SVec *pstart __asm__("$4");
        pstart = p;
        __asm__ volatile ("" : : "r"(pstart) : "$16");
        cur = v;
        func_80010234(pstart, cur, 4);
    }
    depth = 0;
    for (i = 0; i < 4; i++) {
        cur->z = ((cur->z - min->x) * 80) >> 16;
        cur->x = ((cur->x - min->y) * 80) >> 16;
        depth += cur->y;
        cur++;
    }
    {
    register u8 *lt __asm__("$3");
    register struct PacketLarge *lp __asm__("$4");
    lt = D_8009F9F0;
    lp = (struct PacketLarge *)lt;
    if (lt >= D_8009FA28) {
        __asm__ volatile ("" : : "r"(lt));
        lt = (u8 *)(flags & 31);
    } else {
        D_8009F9F0 = (u8 *)lp + 0x34;
        lt = (u8 *)(flags & 31);
    }
    large = lp;
    color = D_800A6F60[(s32)lt];
    }
    depth = 0x400 - (depth >> 8);
    if (depth <= 0) depth = 1;
    large->tag = 0x06000000 | depth;
    large->color = 0x30808080;
    large->color2 = color;
    large->color1 = color;
    large->x0 = (v[0].z + v[1].z) / 2;
    large->y0 = (v[0].x + v[1].x) / 2;
    large->x1 = v[2].z; large->y1 = v[2].x;
    large->x2 = v[3].z; large->y2 = v[3].x;
    if (entity[0x16] == 1) {
        register s32 magic __asm__("$4");
        register s32 nx __asm__("$5");
        register s32 ny __asm__("$6");
        register s32 hx __asm__("$8");
        register s32 hy __asm__("$3");
        register s32 mode __asm__("$7");
        register u8 *text __asm__("$4");
        register s32 y3 __asm__("$3");
        register struct State *st __asm__("$2");
        magic = 0x55555556;
        nx = (v[0].z + v[1].z) / 2 + v[2].z + v[3].z;
        __asm__("mult\t%0,%1" : "=r"(nx) : "r"(magic), "0"(nx) : "hi", "lo");
        __asm__ volatile ("" : : "r"(nx) : "memory");
        ny = (v[0].x + v[1].x) / 2 + v[2].x;
        y3 = v[3].x;
        __asm__("mfhi\t%0" : "=r"(hx) : "r"(nx), "r"(ny), "r"(y3));
        ny += y3;
        __asm__("lui\t%0,%%hi(D_8009FA94)\n\tlw\t%0,%%lo(D_8009FA94)(%0)" : "=r"(st) : "r"(ny));
        __asm__("mult\t%0,%1" : "=r"(ny) : "r"(magic), "0"(ny), "r"(hx), "r"(st) : "hi", "lo");
        mode = st->mode;
        __asm__("" : "=r"(nx) : "0"(nx), "r"(mode));
        nx >>= 31;
        mode = (mode == 1) * 8;
        __asm__ volatile ("" : : "r"(mode) : "$4");
        text = D_8009F37C;
        __asm__("" : "=r"(hx) : "0"(hx), "r"(text));
        nx = hx - nx - 4;
        __asm__("" : "=r"(ny) : "0"(ny), "r"(nx));
        ny >>= 31;
        __asm__("mfhi\t%0" : "=r"(hy) : "r"(nx), "r"(ny), "r"(mode), "r"(text));
        ny = hy - ny;
        __asm__("" : "=r"(ny) : "0"(ny));
        func_80049100(text, nx, ny - 4, mode);
    }
}
