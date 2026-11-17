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