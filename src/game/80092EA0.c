// Categories: Platform > GPU | Graphics > GPU > Debug Validation
typedef unsigned int u32;
typedef struct { int pad[2]; void (*fn)(void *, void *, int, u32); void *ctx; } Device;
extern Device *gVramWriteCallback __asm__("D_8009DDE8");
extern char D_80012A88[];
extern void func_80092CF4(char *, void *);
/**
 * @brief Debug device write: formats a message through the GPU debug path
 *        and sends an 8-byte command to the debug device.
 *
 * @param arg0 Message buffer forwarded to the formatter and the device.
 * @param arg1 Low byte of the command word.
 * @param arg2 Byte 1 of the command word.
 * @param arg3 Byte 2 of the command word.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80092CF4(D_80012A88, arg0) formats/validates the message, then
 * D_8009DDE8->fn(ctx, arg0, 8, 0x80000000 | (arg2 & 255) << 8 |
 * (arg3 & 255) << 16 | (arg1 & 255)) sends it. The device record holds the
 * callback at +8 and its context at +0xC.
 *
 * @par Likely role
 * A development-only trace channel (the format string lives at D_80012A88
 * in rodata): retail builds keep the stub but no caller references it.
 *
 * @par Unknown
 * Which device D_8009DDE8 points at in development builds.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely dead retail code.
 */
void func_80092EA0(void *arg0, int arg1, int arg2, int arg3)
{
    register void *a __asm__("$19") = arg0;
    register int b __asm__("$18");
    register int c __asm__("$16");
    register int d __asm__("$17");
    register void *a5 __asm__("$5");
    register char *fmt __asm__("$4") = D_80012A88;
    register Device *device __asm__("$3");
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
    c = ((c & 255) << 8) | 0x80000000;
    __asm__("" : "+r"(c));
    d |= c;
    __asm__("" : "+r"(d));
    b &= 255;
    __asm__("" : "+r"(b));
    device = gVramWriteCallback;
    __asm__("" : "+r"(device));
    device->fn(device->ctx, a5, 8, d | b);
    __asm__ volatile(".set reorder");
}
