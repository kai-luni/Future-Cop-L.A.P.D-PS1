// Categories: Platform > State Control
#include "game/types.h"

typedef struct {
    u8 pad[0x34];
    void (*fn34)(s32);
} Unk8009DDE8;

extern u8 gGpuQueueMode[] __asm__("D_8009DDF1");
extern char gSetGraphQueueTraceFormat[] __asm__("D_80012A00");
extern  void (*gLogPrintf)(char *, s32) __asm__("D_8009DDEC");
extern Unk8009DDE8 *gVramWriteCallback __asm__("D_8009DDE8");
extern void gSetDmaCallback(s32, s32) __asm__("func_800914E0");

/**
 *  @brief Updates a global byte state and returns its previous value.
 *
 *  An optional logging callback runs according to the adjacent mode byte;
 *  a changed value triggers a state callback and a helper call.
 *  @param arg0 New value to compare with and store in the state byte.
 *  @return Previous unsigned byte value.
 *  @par Globals
 *  D_8009DDF1 holds the state and adjacent mode; D_8009DDEC is the logging
 *  callback, D_80012A00 its string argument, and D_8009DDE8 holds a callback.
 *  @par Control flow
 *  Logs when mode is at least two; only a differing value triggers the update.
 *  @par Calls
 *  D_8009DDEC receives the string and arg0; D_8009DDE8's fn34 receives one;
 *  func_800914E0 receives two and zero after the byte is stored.
 *  @par Side effects
 *  May store the low byte of arg0 and invoke callbacks with further effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80092AE0(s32 arg0)
{
    register u8 *p __asm__("$17") = gGpuQueueMode;
    s32 old;
    u8 mode = p[1];

    old = p[0];
    if (mode >= 2) {
        gLogPrintf(gSetGraphQueueTraceFormat, arg0);
    }
    if (arg0 != p[0]) {
        gVramWriteCallback->fn34(1);
        p[0] = arg0;
        gSetDmaCallback(2, 0);
    }
    return old;
}
