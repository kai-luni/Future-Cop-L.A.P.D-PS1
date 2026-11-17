// Categories: Graphics > DMA Command Lists
#include "game/types.h"

extern u8 gGpuDebugLevel __asm__("D_8009DDF2");
extern  void (*gLogPrintf)(u8 *, s32, s32) __asm__("D_8009DDEC");
extern u8 D_80012AB8[];
extern u32 gSeqFirstWord[] __asm__("D_8009DEB0");
extern u32 gGpuTpageWord[] __asm__("D_8009DE9C");

/**
 *  @brief Link a sequence of words to a global command-list node.
 *
 *  Builds 24-bit next-address links in the supplied words and connects the
 *  final word through a global node to another global address.
 *  The count must be positive for the linking loop to terminate normally.
 *  @param arg0 First word of the sequence to link.
 *  @param arg1 Number of words in the sequence.
 *  @return Pointer to the final word in the sequence.
 *  @par Globals
 *  D_8009DDF2 gates the D_8009DDEC callback using message D_80012AB8.
 *  D_8009DEB0 receives a link to D_8009DE9C with the 0x04000000 flag.
 *  @par Control flow
 *  Optionally dispatches the callback, then links all but the final word.
 *  @par Calls
 *  D_8009DDEC receives the message, sequence address, and original count.
 *  @par Side effects
 *  Rewrites the sequence words and the first word at D_8009DEB0.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u32 *func_800930B0(u32 *arg0, s32 arg1)
{
    register u32 *s0 __asm__("$16");
    register s32 s1 __asm__("$17");
    register u32 a2 __asm__("$6");
    register u32 v1 __asm__("$3");
    register u32 a1 __asm__("$5");
    register u32 *v0 __asm__("$2");
    void (*f)(u8 *, s32, s32);
    u32 *p;
    u32 m;

    s0 = arg0;
    s1 = arg1;

    if (gGpuDebugLevel >= 2) {
        u8 *msg = D_80012AB8;
        f = gLogPrintf;
        __asm__ volatile("" : "+r"(msg) : : "memory");
        __asm__ volatile("" : "+r"(s0) : : "memory");
        f(msg, (s32)s0, s1);
    }

    s1 = s1 - 1;
    if (s1 != 0) {
        m = 0xFFFFFF;
        do {
            s1--;
            p = s0 + 1;
            *(u8 *)((u8 *)s0 + 3) = 0;
            *s0 = (*s0 & 0xFF000000) | ((u32)p & m);
            s0 = p;
        } while (s1 != 0);
    }

    a2 = 0xFFFFFF;
    v0 = s0;
    __asm__ volatile("" : "+r"(v0));
    a1 = (u32)gSeqFirstWord;
    __asm__ volatile("" : "+r"(a1));
    v1 = (u32)gGpuTpageWord;
    v1 = v1 & a2;
    v1 = v1 | 0x4000000;
    *(u32 *)a1 = v1;
    a1 = a1 & a2;
    *v0 = a1;
    return v0;
}
