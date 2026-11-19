// Categories: Graphics > GPU > VRAM Transfer | Platform > GPU
typedef unsigned int u32;
typedef struct { int pad[2]; void (*fn)(void *, void *, int, u32); void *ctx; } Device;
extern Device *gVramWriteCallback __asm__("D_8009DDE8");
extern char D_80012A7C[];
void gClearImage(void *arg0, int arg1, int arg2, int arg3) __asm__("func_80092E10");

extern void func_80092CF4(char *, void *);
/**
 * @brief Builds and sends a four-component data block to the GPU library.
 *
 * @param arg0 Source pointer (the block that is copied).
 * @param arg1, arg2, arg3 Three byte values, packed as
 *        ((arg3 & 0xFF) << 16) | ((arg2 & 0xFF) << 8) | (arg1 & 0xFF).
 * @return Nothing; the library callback at D_8009DDE8->fn runs with a length of
 *         8 words.
 *
 * @par Confirmed
 * A debug hook (func_80092CF4) runs first, then
 * `D_8009DDE8->fn(ctx, arg0, 8, packed)` - the "upload a quad of bytes" form of
 * the library API. Several `register ... __asm__` pins keep the argument
 * registers in the retail order.
 *
 * @par Environment path
 * VRAM fill request: after the rectangle check (tag string ClearImage) it
 * calls the GPU library table slot at +8 with the table argument at +0xC, the
 * rectangle pointer, size 8 and the colour packed as (arg3 and 0xFF) shifted
 * left 16, (arg2 and 0xFF) shifted left 8 and (arg1 and 0xFF). The debug
 * screens func_800773D8 and func_8007757C and the playback reset
 * func_80085AC4 call it with colour 0,0,0 to clear areas to black.
 */
void gClearImage(void *arg0, int arg1, int arg2, int arg3)
{
    register void *a __asm__("$19") = arg0;
    register int b __asm__("$18");
    register int c __asm__("$17");
    register int d __asm__("$16");
    register void *a5 __asm__("$5");
    register char *fmt __asm__("$4") = D_80012A7C;
    __asm__("" : "+r"(a), "+r"(fmt));
    b = arg1;
    a5 = a;
    __asm__("" : "+r"(b), "+r"(a5));
    c = arg2;
    __asm__("" : "+r"(c));
    d = arg3;
    func_80092CF4(fmt, a5);
    a5 = a;
    __asm__("" : "+r"(a5));
    d = (d & 255) << 16;
    __asm__("" : "+r"(d));
    d |= (c & 255) << 8;
    __asm__("" : "+r"(d));
    b &= 255;
    __asm__("" : "+r"(b));
    gVramWriteCallback->fn(gVramWriteCallback->ctx, a5, 8,
                     d | b);
}
