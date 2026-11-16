// Categories: Utilities > Diagnostics
#include "game/types.h"
/* Test the return sequence under a different compiler. */

extern  void (*gLogPrintf)(const char *, ...) __asm__("D_8009DDEC");
struct Values { s16 values[8]; u8 first; u8 second; };

/**
 *  @brief Prints the eight values and two byte fields of a structure.
 *
 *  Uses four fixed format strings to emit two groups of four signed values
 *  followed by the two individual byte fields.
 *  @param arg0 Structure containing the values to print.
 *  @par Globals
 *  D_8009DDEC supplies the variadic print function pointer.
 *  @par Control flow
 *  Makes four sequential print calls without branches or loops.
 *  @par Calls
 *  D_8009DDEC is invoked with format strings at fixed addresses and the field values.
 *  @par Side effects
 *  Produces output through the configured print function.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092868(struct Values *arg0)
{
    register int a __asm__("$5");
    register int b __asm__("$6");
    register int c __asm__("$7");
    register int last __asm__("$3");
    register void (*print)(const char *, ...) __asm__("$2");
    register char *format __asm__("$4");
    a = arg0->values[0];
    b = arg0->values[1];
    c = arg0->values[2];
    __asm__("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
    last = arg0->values[3];
    __asm__("" : "=r"(last) : "0"(last));
    print = gLogPrintf;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x291c, a, b, c, last);
    a = arg0->values[4];
    b = arg0->values[5];
    c = arg0->values[6];
    __asm__("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
    last = arg0->values[7];
    __asm__("" : "=r"(last) : "0"(last));
    print = gLogPrintf;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2938, a, b, c, last);
    a = arg0->first;
    __asm__("" : "=r"(a) : "0"(a));
    print = gLogPrintf;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2954, a);
    a = arg0->second;
    __asm__("" : "=r"(a) : "0"(a));
    print = gLogPrintf;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2960, a);
    __asm__ volatile(".set\tnoreorder\n\t.set  reorder");
}
