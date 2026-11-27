// Categories: Graphics > Effects > Fade Quad | Graphics > GPU Primitives
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Packet {
    u32 addr : 24;
    u32 len : 8;
    u8 r0, g0, b0, code;
    short x0, y0;
    union { struct { u8 r1, g1, b1, pad1; } b; struct { u16 w, h; } s; } c;
    short x1, y1;
    u8 r2, g2, b2, pad2;
    short x2, y2;
    u8 r3, g3, b3, pad3;
    short x3, y3;
    u8 rest[16];
} Packet;

extern Packet *gPrimPoolNext __asm__("D_8009F9F0");
extern Packet *gPrimPoolEnd __asm__("D_8009FA28");
extern int func_80028FD4(void);
extern void func_8002385C(void);
extern void gLoadQueuedAssets(void) __asm__("func_80024EC8");
extern void func_8001B4A8(int);
void gDrawScreenFadeQuad(void) __asm__("func_80025690");


/**
 * @brief Draws one full-screen fade/wipe quad with a brightness ramp.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * The fade level comes from func_80028FD4() and drives the colour: the quad is a
 * 4-point gouraud polygon (code 0x60) with base 0x60/0x40/0x40 and a computed
 * intensity per corner, written into the 52-byte packet pool. When the pool is
 * exhausted the last slot is reused instead of advancing.
 *
 * @par Likely role
 * The screen fade used by the front-end transitions (func_800259D8 calls it in
 * its idle loop, i.e. once per frame while a screen settles).
 *
 * @par Callers
 * func_800259D8 only.
 *
 * @par Environment path
 * Draws the front-end transition progress/fade bar driven by func_80028FD4().
 * Emits two 0x60 flat rectangles and two 0x38 Gouraud quads into the 52-byte
 * D_8009F9F0 pool, then calls func_8002385C, func_80024EC8, and
 * func_8001B4A8(1).
 */
void gDrawScreenFadeQuad(void)
{
    Packet *p;
    int value;
    int x;
    register int intensity __asm__("$8");
    register int base __asm__("$2");

    value = func_80028FD4();
    if (gPrimPoolNext >= gPrimPoolEnd) {
        p = gPrimPoolNext;
        goto first;
    }
    p = gPrimPoolNext;
    gPrimPoolNext = p + 1;
first:
    p->code = 0x60;
    p->r0 = 0x60;
    p->g0 = 0x40;
    p->b0 = 0x40;
    p->x0 = 0x1f;
    p->y0 = 0x95;
    p->c.s.w = 0x102;
    p->addr = 3;
    p->len = 3;
    p->c.s.h = 10;
    if (gPrimPoolNext >= gPrimPoolEnd) {
        p = gPrimPoolNext;
        goto second;
    }
    p = gPrimPoolNext;
    gPrimPoolNext = p + 1;
second:
    p->code = 0x60;
    p->x0 = 0x20;
    p->y0 = 0x96;
    p->c.s.w = 0x100;
    p->r0 = 0;
    p->g0 = 0;
    p->b0 = 0;
    p->addr = 2;
    p->len = 3;
    p->c.s.h = 8;
    if (gPrimPoolNext >= gPrimPoolEnd) {
        p = gPrimPoolNext;
        goto third;
    }
    p = gPrimPoolNext;
    gPrimPoolNext = p + 1;
third:
    p->code = 0x38;
    p->addr = 1;
    p->len = 8;
    x = value / 8;
    p->r0 = 0x10;
    p->g0 = 0;
    p->b0 = 0;
    intensity = 128;
    p->c.b.r1 = (intensity - x) / 8;
    p->c.b.g1 = x / 8;
    p->c.b.b1 = 0;
    p->r2 = intensity;
    p->g2 = 0;
    p->b2 = 0;
    p->r3 = -128 - x;
    p->g3 = x;
    p->b3 = 0;
    p->x0 = 0x20;
    p->y0 = 0x96;
    p->x1 = 0x20 + value / 4;
    p->y1 = 0x96;
    p->x2 = 0x20;
    p->y2 = 0x9a;
    p->x3 = 0x20 + value / 4;
    p->y3 = 0x9a;
    if (gPrimPoolNext >= gPrimPoolEnd) {
        p = gPrimPoolNext;
        goto fourth;
    }
    p = gPrimPoolNext;
    gPrimPoolNext = p + 1;
fourth:
    p->code = 0x38;
    p->addr = 1;
    p->len = 8;
    x = value / 8;
    base = 128;
    p->r0 = base;
    p->g0 = 0;
    p->b0 = 0;
    p->c.b.r1 = -128 - x;
    p->c.b.g1 = x;
    p->c.b.b1 = 0;
    p->r2 = 0x10;
    p->g2 = 0;
    p->b2 = 0;
    p->r3 = (base - x) / 8;
    p->g3 = x / 8;
    p->b3 = 0;
    p->x0 = 0x20;
    p->y0 = 0x9a;
    p->x1 = 0x20 + value / 4;
    p->y1 = 0x9a;
    p->x2 = 0x20;
    p->y2 = 0x9e;
    p->x3 = 0x20 + value / 4;
    p->y3 = 0x9e;
    func_8002385C();
    gLoadQueuedAssets();
    func_8001B4A8(1);
}
