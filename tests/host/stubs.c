/* Host-harness globals and no-op stubs for the decomp texture path.
 * Symbol names match the decomp __asm__ labels; MIPS is 32-bit so u32 slots
 * hold pointers exactly as the retail code expects. */
typedef unsigned int u32;
typedef int s32;

u32 D_8009F528 __asm__("D_8009F528");
u32 D_8009F52C __asm__("D_8009F52C");
u32 D_8009F530 __asm__("D_8009F530");
u32 D_8009F534 __asm__("D_8009F534");
u32 D_8009F538 __asm__("D_8009F538");
u32 D_8009F53C __asm__("D_8009F53C");
u32 D_8009F540 __asm__("D_8009F540");
u32 D_8009F544 __asm__("D_8009F544");
u32 D_8009F548 __asm__("D_8009F548");
u32 D_8009F54C __asm__("D_8009F54C");
u32 D_8009F550 __asm__("D_8009F550");
u32 D_8009F554 __asm__("D_8009F554");
u32 D_8009F558 __asm__("D_8009F558");
u32 D_8009F55C __asm__("D_8009F55C");
u32 D_8009F560 __asm__("D_8009F560");
u32 D_8009F564 __asm__("D_8009F564");
u32 D_8009F568 __asm__("D_8009F568");
u32 D_800A8E80[10] __asm__("D_800A8E80");

/* Stubs for func_80063C5C dependencies (LoadImage/DrawSync/font/free/parser). */
static u32 fake_res[16];
void *func_8001996C(void *a, s32 b) { (void)a; (void)b; return fake_res; }
void func_80092F38(void *a, u32 b) { (void)a; (void)b; }
void func_80092C8C(s32 a) { (void)a; }
void func_80048DEC(void) {}
void func_800181D4(void *a) { (void)a; }
void func_80063428(void) { int i; for (i = 0; i < 10; i++) D_800A8E80[i] = 0; }

/* Freestanding libc pieces the compiler may emit. */
void __main(void) {}
void *memcpy(void *d, const void *s, unsigned n)
{
    char *dd = d; const char *ss = s;
    while (n--) *dd++ = *ss++;
    return d;
}
void *memset(void *d, int c, unsigned n)
{
    char *dd = d;
    while (n--) *dd++ = (char)c;
    return d;
}

/* ---- render path (func_80015B68 / func_80016A64) globals and recorders ---- */
typedef unsigned char u8;
typedef short s16;

u32 D_8009F570 __asm__("D_8009F570"); /* gVertexSourcePtr */
u32 D_8009F574 __asm__("D_8009F574"); /* gTransformedVertexPtr */
u32 D_8009F578 __asm__("D_8009F578"); /* gPartVertexBuffer */
u32 D_8009F57C __asm__("D_8009F57C"); /* gClipFlagBuffer */
u32 D_8009F580 __asm__("D_8009F580"); /* gDepthTagWord */
u32 D_8009F9E4 __asm__("D_8009F9E4"); /* gCamera */
u32 D_800A8EA4 __asm__("D_800A8EA4"); /* gEnvOverlayFn */
u32 D_8009FA0C __asm__("D_8009FA0C"); /* width mode */

/* Recorded submissions: the harness asserts these against the golden faces. */
u32 rec_count;
u32 rec_type[64];
u32 rec_xy[64 * 4];
s32 rec_z[64];
u32 overlay_count;

void func_80015F38(void *part, void *work, void *tex, void *a3, void *a4)
{
    u32 *xy = (u32 *)work;
    int i;
    (void)tex; (void)a3; (void)a4;
    if (rec_count >= 64) return;
    rec_type[rec_count] = ((u8 *)part)[1] & 0xF;
    for (i = 0; i < 4; i++) rec_xy[rec_count * 4 + i] = xy[i];
    rec_z[rec_count] = *(s32 *)((u8 *)work + 16);
    rec_count++;
}
void func_80016E6C(void *part, void *work, void *inst, void *mat, void *pos)
{
    (void)part; (void)work; (void)inst; (void)mat; (void)pos;
    overlay_count++;
}
s32 func_80015830(void *part, void *work, void *tex, void *bias)
{
    (void)part; (void)work; (void)tex; (void)bias;
    return 0;
}
void func_8001484C(void *part, void *work, void *bias, void *tex)
{
    (void)part; (void)work; (void)bias; (void)tex;
}
void func_80019470(void *a) { (void)a; }
void func_80010820(void *a, void *b) { (void)a; (void)b; }
void func_8001006C(void *a, void *b, s32 c, s32 d) { (void)a; (void)b; (void)c; (void)d; }