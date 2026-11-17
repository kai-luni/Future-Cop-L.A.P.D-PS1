// Categories: Platform > GPU | Graphics > Display Setup
typedef unsigned char u8;
extern u8 gGpuDebugLevel[] __asm__("D_8009DDF2");
extern char D_800129D4[];
extern  void (*gLogPrintf)(char *, int, int, int) __asm__("D_8009DDEC");

/**
 * @brief Exchanges a library flag byte and notifies the debug hook when
 *        the new value is nonzero.
 *
 * @param value New value for D_8009DDF2[0].
 * @return The previous byte.
 *
 * @par Confirmed
 * old = D_8009DDF2[0]; D_8009DDF2[0] = value. When (u8)value != 0, the
 * function pointer D_8009DDEC runs with (D_800129D4, value, p[-2], p[1])
 * - the tag string plus the three bytes around the flag (D_8009DDF0,
 * the new value, D_8009DDF3).
 *
 * @par Likely role
 * A display/debug switch whose change is reported through the library's
 * printf-style hook.
 *
 * @par Callers
 * func_8001BE68.
 */
int func_80092A84(int value)
{
    register u8 *p __asm__("$3") = gGpuDebugLevel;
    int old;
    __asm__ volatile("" : "+r"(p));
    old = p[0];
    p[0] = value;
    if ((u8)value) {
        register void (*fn)(char *, int, int, int) __asm__("$2") = gLogPrintf;
        register int a1 __asm__("$5") = p[0];
        register int a2 __asm__("$6") = p[-2];
        register int a3 __asm__("$7") = p[1];
        __asm__ volatile("" : "+r"(fn), "+r"(a1), "+r"(a2), "+r"(a3));
        fn(D_800129D4, a1, a2, a3);
    }
    return old;
}
