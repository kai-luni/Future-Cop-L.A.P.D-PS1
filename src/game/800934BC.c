// Categories: Graphics > GPU > Drawing Area | Platform > GPU
#include "game/types.h"

struct Prim {
    u16 u0, u2;
    s16 s4, s6, s8, sA, sC, sE;
    u8 b10, b11, b12, b13;
    s32 pad;
};
struct Gfx { u8 pad[0x10]; void (*fn)(u32); };
struct Cache { s16 s[4]; s16 t[4]; s32 w; s32 pad; };

extern u8 gGpuDebugLevel[] __asm__("D_8009DDF2");
extern u8 gGpuReverseFlag __asm__("D_8009DDF3");
extern struct Gfx *gVramWriteCallback __asm__("D_8009DDE8");
extern  void (*gLogPrintf)(void *, void *) __asm__("D_8009DDEC");
extern char gPutDispEnvTraceFormat[] __asm__("D_80012B30");
extern struct Cache gDispEnvCache __asm__("D_8009DE5C");
extern s16 D_8009DE64[];
extern s32 gDispEnvCachedModeWord[] __asm__("D_8009DE6C");
extern int gGetGpuDrawOffset(void) __asm__("func_80091F74");
extern void func_8008CD10(void *, void *, int);
struct Prim *gPutDispEnvCached(struct Prim *p) __asm__("func_800934BC");


/**
 * @brief Applies a drawing-area state block to the GPU library, with change
 *        detection.
 *
 * @param p State block: two packed coordinate words (p->u0/p->u2), four
 *          half-word rectangle values at +0x8..+0xE and a status byte at +0x12.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. Debug hook when the library's level byte D_8009DDF2[0] >= 2.
 * 2. A GP0 0x05 command word is built from p->u0/p->u2 (both masked to 0x3FF and
 *    packed as (upper << 10) | lower) and handed to D_8009DDE8->fn.
 * 3. The four rectangle values are compared with the library's current values at
 *    D_8009DDF2 + 0x72; if any differs, the library is asked for a new origin
 *    (func_80091F74), the status byte is stored and the four target coordinates
 *    are recomputed from a 10-pixel grid.
 *
 * @par Likely role
 * The "set drawing area and offset" call of the 2D layer, with a cache so an
 * unchanged area costs only the two comparisons.
 *
 * @par Callers
 * func_8001BFE4, func_8001C088, func_800773D8, func_8007757C and the sprite
 * paths.
 *
 * @par Environment path
 * Applies a 0x14-byte display/drawing-area state block to the GPU via
 * gVramWriteCallback->fn (GP0/GP1 05h, 06h, 07h, 08h commands) with change
 * detection against gGpuDebugLevel+0x72 and D_8009DE6C, then copies the block
 * into gDispEnvCache via func_8008CD10 (called by func_8001BFE4:38,
 * func_800773D8:75, func_80085C4C:135).
 */
struct Prim *gPutDispEnvCached(struct Prim *p)
{
    register u32 flags __asm__("$19");
    s32 b, t;
    register s32 d __asm__("$5");
    register s32 a __asm__("$6");
    register s32 c __asm__("$7");
    register s32 mul __asm__("$3");
    register s32 val __asm__("$4");
    register s32 mul2 __asm__("$2");
    s32 x0, y0;
    register s32 x1 __asm__("$17");
    register s32 y1 __asm__("$18");
    register u16 *q __asm__("$4");
    register u8 *g __asm__("$17");
    register u32 upper __asm__("$3");
    register u32 lower __asm__("$4");
    register u32 cmd __asm__("$2");
    register struct Gfx *gfx __asm__("$5");

    g = gGpuDebugLevel;
    __asm__("" : "+r"(g));
    flags = 0x8000000;
    if (g[0] >= 2) {
        gLogPrintf(gPutDispEnvTraceFormat, p);
    }
    cmd = 0x5000000;
    upper = p->u2;
    lower = p->u0;
    gfx = gVramWriteCallback;
    __asm__("" : "+r"(cmd), "+r"(upper), "+r"(lower), "+r"(gfx));
    gfx->fn((((upper & 0x3FF) << 10) | ((lower & 0x3FF) | cmd)));
    q = (u16 *)(g + 0x72);
    __asm__("" : "+r"(q));
    t = *(u16 *)(g + 0x72); b = p->s8; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) goto rect_changed;
    t = q[1]; b = p->sA; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) goto rect_changed;
    t = q[2]; b = p->sC; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) goto rect_changed;
    t = q[3]; b = p->sE; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) {
rect_changed:
        t = gGetGpuDrawOffset();
        val = p->s8;
        __asm__("" : "+r"(val), "+r"(t));
        p->b12 = t;
        t = (u8)t;
        mul = val * 10;
        __asm__("" : "+r"(mul));
        val = p->sA;
        __asm__("" : "+r"(val));
        a = mul + 0x260;
        if (t) {
            x1 = val + 0x13;
        } else {
            x1 = val + 0x10;
        }
        mul = p->sC;
        __asm__("" : "+r"(mul));
        if (mul) {
            mul2 = mul * 10;
            __asm__("" : "+r"(mul2));
            c = a + mul2;
        } else {
            c = a + 0xA00;
        }
        if (p->sE) {
            y1 = x1 + p->sE;
        } else {
            y1 = x1 + 0xF0;
        }
        if (a < 0x1F4) d = 0x1F4;
        else if (a < 0xCDB) d = a;
        else d = 0xCDA;
        a = d;
        if (c < a + 0x50) d = a + 0x50;
        else if (c < 0xCDB) d = c;
        else d = 0xCDA;
        c = d;
        if (x1 >= 0x10) {
            if (p->b12 ? (x1 >= 0x137) : (x1 >= 0x101)) val = p->b12 ? 0x136 : 0x100;
            else val = x1;
        } else val = 0x10;
        x1 = val;
        d = x1 + 2;
        if (y1 >= d) {
            if (p->b12 ? (y1 >= 0x139) : (y1 >= 0x103)) mul = p->b12 ? 0x138 : 0x102;
            else mul = y1;
        } else mul = d;
        y1 = mul;
        upper = c & 0xFFF;
        upper <<= 12;
        lower = a & 0xFFF;
        cmd = 0x6000000;
        gfx = gVramWriteCallback;
        __asm__("" : "+r"(upper), "+r"(lower), "+r"(cmd), "+r"(gfx));
        lower |= cmd;
        __asm__("" : "+r"(lower), "+r"(upper), "+r"(gfx));
        gfx->fn(upper | lower);
        upper = y1 & 0x3FF;
        upper <<= 10;
        lower = x1 & 0x3FF;
        cmd = 0x7000000;
        gfx = gVramWriteCallback;
        __asm__("" : "+r"(upper), "+r"(lower), "+r"(cmd), "+r"(gfx));
        lower |= cmd;
        __asm__("" : "+r"(lower), "+r"(upper), "+r"(gfx));
        gfx->fn(upper | lower);
    }
    __asm__("" ::: "memory");
    q = (u16 *)gDispEnvCachedModeWord;
    __asm__("" : "+r"(q));
    if (*(s32 *)q != *(s32 *)&p->b10) goto mode_changed;
    t = q[-8]; b = (s16)p->u0; __asm__("" : "+r"(t), "+r"(b));
    q -= 8;
    if ((t << 16 >> 16) != b) goto mode_changed;
    t = q[1]; b = (s16)p->u2; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) goto mode_changed;
    t = q[2]; b = p->s4; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) goto mode_changed;
    t = q[3]; b = p->s6; __asm__("" : "+r"(t), "+r"(b));
    if ((t << 16 >> 16) != b) {
mode_changed:
        p->b12 = gGetGpuDrawOffset();
        if (p->b12 == 1) flags |= 8;
        if (p->b11) flags |= 0x10;
        if (p->b10) flags |= 0x20;
        if (gGpuReverseFlag) flags |= 0x80;
        if (p->s4 < 0x119) {
        } else if (p->s4 < 0x161) {
            flags |= 1;
        } else if (p->s4 < 0x191) {
            flags |= 0x40;
        } else if (p->s4 < 0x231) {
            flags |= 2;
        } else {
            flags |= 3;
        }
        cmd = p->b12;
        mul = p->s6;
        __asm__("" : "+r"(cmd), "+r"(mul));
        if (cmd) t = mul < 0x121; else t = mul < 0x101;
        if (!t) flags |= 0x24;
        gVramWriteCallback->fn(flags);
    }
    __asm__("" ::: "memory");
    func_8008CD10(&gDispEnvCache, p, 0x14);
    __asm__(".set reorder");
    return p;
}
