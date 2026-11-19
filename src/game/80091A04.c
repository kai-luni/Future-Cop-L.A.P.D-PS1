// Categories: Platform > Hardware State
#include "game/intc_globals.h"

#include "game/dma_globals.h"

typedef unsigned short u16;
typedef unsigned int u32;
extern char gIrqDispatchState[] __asm__("D_8009CC88");
extern volatile u16 *gInterruptStatusHalfPtr __asm__("D_8009DD14");
extern volatile u16 *gInterruptMaskPtr __asm__("D_8009DD18");
extern volatile u32 *gDmaControlPtr __asm__("D_8009DD1C");
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void func_80091B70(void);
void *gStopIrqSubsystem(void) __asm__("func_80091A04");


/**
 *  @brief Saves and clears hardware state when the global record is active.
 *
 *  The active record receives values read through global hardware pointers;
 *  the routine then clears two 16-bit locations and masks a 32-bit location.
 *  @return The global record pointer if active, otherwise null.
 *  @par Globals
 *  D_8009CC88 is the record; D_8009DD14, D_8009DD18, and D_8009DD1C
 *  point to the hardware locations read or written.
 *  @par Control flow
 *  Returns immediately if the record's first halfword is zero; otherwise saves
 *  state, clears and masks hardware locations, and marks the record inactive.
 *  @par Calls
 *  func_8008C9A0 runs before the hardware accesses; func_80091B70 runs after.
 *  @par Side effects
 *  Writes record offsets 0, 0x32, and 0x34 and modifies hardware locations.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Stops the interrupt and DMA callback subsystem when D_8009CC88 is active by
 * entering a critical section via func_8008C9A0, saving *D_8009DD18 (I_MASK)
 * and *D_8009DD1C (DPCR) into D_8009CC88 + 0x32 and +0x34, clearing I_MASK
 * and I_STAT, and masking DPCR with 0x77777777. It then calls func_80091B70,
 * clears the active halfword at D_8009CC88, and returns D_8009CC88.
 */
void *gStopIrqSubsystem(void)
{
    register char *p __asm__("$16") = gIrqDispatchState;
    volatile u16 *a;
    volatile u16 *b;
    volatile u32 *c;
    u32 temp;
    __asm__("" : "+r"(p));
    if (*(u16 *)p == 0)
        return 0;
    gEnterCriticalSection();
    b = gInterruptMaskPtr;
    c = gDmaControlPtr;
    *(u16 *)(p + 0x32) = *b;
    temp = *c;
    a = gInterruptStatusHalfPtr;
    *(u32 *)(p + 0x34) = temp;
    *b = 0;
    *a = *b;
    *(u32 *)gDmaControlPtr = *gDmaControlPtr & 0x77777777;
    func_80091B70();
    *(u16 *)p = 0;
    return p;
}
