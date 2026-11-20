// Categories: Platform > Initialization
#include "game/intc_globals.h"

#include "game/dma_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef struct Other {
    int pad;
    void *field4;
    char pad2[12];
    void *field14;
} Other;
extern char gIrqDispatchState[] __asm__("D_8009CC88");
extern char gIrqQueueHeadPtr[] __asm__("D_8009CCC4");
extern volatile u16 *gInterruptStatusHalfPtr __asm__("D_8009DD14");
extern volatile u16 *gInterruptMaskPtr __asm__("D_8009DD18");
extern volatile u32 *gDmaControlPtr __asm__("D_8009DD1C");
extern Other *gIrqCallbackTable __asm__("D_8009DD10");
extern void gZeroWords(void *, unsigned int) __asm__("func_80091B1C");
extern int func_80091B90(void *);
extern void gDispatchIrqs(void) __asm__("func_800916EC");
extern void func_80091B80(void *);
extern void *gInitVSyncHandler(void) __asm__("func_80091BA0");
extern void *gInitDmaCallbacks(void) __asm__("func_80091CC0");
extern void func_80091B48(void *);
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
void *gInitIrqSubsystem(void) __asm__("func_80091614");


/**
 *  @brief Initializes a global subsystem and returns its state address.
 *
 *  The routine initializes register-backed state and a global structure,
 *  installs returned pointers, and invokes the subsystem setup helpers.
 *  @return The address D_8009CC88 on initialization, or null if already set.
 *  @par Globals
 *  D_8009CC88 contains the initialization flag; D_8009CCC4 anchors a
 *  pointer setup; D_8009DD14, D_8009DD18 and D_8009DD1C address registers.
 *  D_8009DD10 points to the structure receiving two helper results.
 *  @par Control flow
 *  A set flag returns immediately; a helper result conditionally triggers setup.
 *  @par Calls
 *  func_80091B1C initializes state; func_80091B90 tests it and
 *  func_800916EC handles a positive result. func_80091B80, func_80091BA0,
 *  func_80091CC0, func_80091B48 and func_8008C9B0 complete setup.
 *  @par Side effects
 *  Writes registers and global state and invokes initialization helpers.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Initializes the interrupt and DMA callback subsystem if D_8009CC88 is zero,
 * clearing *D_8009DD18 (I_MASK) and *D_8009DD14 (I_STAT), setting *D_8009DD1C
 * (DPCR) to 0x33333333, and zeroing 0x41A ints at D_8009CC88 via
 * func_80091B1C. It conditionally runs func_800916EC via func_80091B90, sets
 * up D_8009CCC4 and func_80091B80, populates D_8009DD10 slots +0x14
 * (func_80091BA0) and +0x04 (func_80091CC0), calls func_80091B48 and
 * func_8008C9B0, and returns D_8009CC88.
 */
void *gInitIrqSubsystem(void)
{
    register char *p __asm__("$16") = gIrqDispatchState;
    volatile u16 *a;
    volatile u16 *b;
    __asm__("" : "+r"(p));
    if (*(u16 *)p != 0) return 0;
    a = gInterruptStatusHalfPtr;
    b = gInterruptMaskPtr;
    *b = 0;
    *a = *b;
    *gDmaControlPtr = 0x33333333;
    gZeroWords(p, 0x41a);
    if (func_80091B90(p + 0x38)) gDispatchIrqs();
    p = gIrqQueueHeadPtr;
    __asm__("" : "+r"(p));
    *(void **)p = p + 0xfdc;
    func_80091B80(p - 4);
    *(u16 *)(p - 0x3c) = 1;
    gIrqCallbackTable->field14 = gInitVSyncHandler();
    gIrqCallbackTable->field4 = gInitDmaCallbacks();
    func_80091B48(gIrqCallbackTable);
    p -= 0x3c;
    gExitCriticalSection();
    return p;
}
