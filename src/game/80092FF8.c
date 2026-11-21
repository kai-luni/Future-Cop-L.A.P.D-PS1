// Categories: Graphics > GPU > VRAM Transfer | Platform > GPU
typedef unsigned int u32;
typedef unsigned short u16;
typedef struct { short x,y,w,h; } Rect;
typedef struct { void *pad0; void *pad4; int (*send)(int, void *, int, int); char padC[12]; int arg; } Context;
extern void gValidateGpuRectDebug(char *, Rect *) __asm__("func_80092CF4");
extern char gDebugTagRectEmpty[] __asm__("D_80012AAC");
extern u32 gGpuCmdBlock[] __asm__("D_8009DE90");
int gMoveImage(Rect *r, int a, int b) __asm__("func_80092FF8");

extern Context *gVramWriteCallback __asm__("D_8009DDE8");
/**
 * @brief Sends a rectangle to the GPU library as a 20-byte transfer command.
 *
 * @param r Rectangle (x, y, w, h as half-words).
 * @param a Destination x/y, packed into one word as (b << 16) | (a & 0xFFFF).
 * @param b see @c a.
 * @return The library's result (negative on an empty rectangle).
 *
 * @par Confirmed mechanics
 * func_80092CF4(D_80012AAC, r) runs the debug wrapper, an empty rectangle returns
 * -1, then a three-word command block is built in place at D_8009DE90:
 * [0] = the rectangle's xy, [1] = the packed destination, [2] = its wh - and
 * handed to the library callback as send(arg, block - 2, 20, 0).
 *
 * @par Likely role
 * The "move image in VRAM" primitive (GP0 0x80) used by the double-buffer and
 * screen-copy paths.
 *
 * @par Callers
 * gPlayVideoFile only.
 *
 * @par Environment path
 * VRAM-to-VRAM copy request: after func_80092CF4 validates the rectangle (tag
 * string MoveImage), an empty rectangle returns -1, otherwise
 * gGpuCmdBlock[0..2] receive the source xy, the destination (b shifted left
 * 16) or (a and 0xFFFF), and the wh, and the table send slot is called with
 * its argument, the block address minus 8, size 20 and 0. Called by the video
 * frame processor func_80088120 to move image rectangles within VRAM.
 */
int gMoveImage(Rect *r, int a, int b)
{
    register int aa __asm__("$18") = a;
    register int bb __asm__("$17") = b;
    __asm__("" : "+r"(aa), "+r"(bb));
    gValidateGpuRectDebug(gDebugTagRectEmpty, r);
    if (!r->w || !r->h) return -1;
    {
        register u32 *base __asm__("$3");
        register u32 val __asm__("$2");
        register u32 lo __asm__("$4");
        register u32 first __asm__("$5");
        register Context *ctx __asm__("$7");
        register int size __asm__("$6");
        val = bb << 16;
        __asm__("" : "+r"(val));
        base = gGpuCmdBlock;
        __asm__("" : "+r"(base));
        lo = aa & 0xffff;
        val = val | lo;
        first = *(u32 *)r;
        ctx = gVramWriteCallback;
        __asm__("" : "+r"(val), "+r"(first), "+r"(ctx));
        size = 20;
        __asm__("" : "+r"(size));
        base[1] = val;
        base[0] = first;
        base[2] = *(u32 *)&r->w;
        val = ctx->send(ctx->arg, base - 2, size, 0);
        __asm__ volatile(".set reorder");
        return val;
    }
}
