// Categories: Input > Controller Communication
#include "game/sio_globals.h"

typedef signed int s32;
typedef unsigned short u16;
struct E { char pad[0xF0]; };
struct H { char pad[10]; u16 v; };
extern s32 gPadSlotIndex __asm__("D_8009C768");
extern s32 gPadDispatchIndex __asm__("D_8009C76C");
extern s32 gPadEndSlot __asm__("D_8009C77C");
extern s32 gPadPendingCounts[] __asm__("D_8009C780");
extern struct H *gSio0CtrlPtr __asm__("D_8009C7C0");
extern struct E gCompareAddress[] __asm__("D_800A3C20");
extern void gUpdatePadTimer(struct E *) __asm__("func_8008C77C");
extern void gConsumePadCmd(struct E *) __asm__("func_8008C06C");
extern s32 gBeginPadSioTransfer(struct E *) __asm__("func_8008A6CC");
void gAdvancePadSlot(s32 arg) __asm__("func_8008BF78");


/**
 *  @brief Process controller entries from the current index until traversal ends.
 *
 *  Uses the initial argument to skip processing, clear a table entry, or run
 *  two entry handlers; subsequent iterations use a nonzero processing value.
 *  @param arg Initial action selector: -9 skips, zero clears, others process.
 *  @par Globals
 *  D_800A3C20 holds entries; D_8009C768 is the current index and D_8009C77C
 *  bounds traversal. D_8009C780 holds per-entry values; D_8009C76C and
 *  D_8009C7C0->v are cleared on each iteration.
 *  @par Control flow
 *  Loops until the incremented index exceeds the bound or the probe succeeds.
 *  @par Calls
 *  func_8008C77C and func_8008C06C process an entry; func_8008A6CC probes
 *  the next entry when it is within the bound.
 *  @par Side effects
 *  Advances the global index, clears shared fields, and may process entries.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Iterates over 0xF0-byte port entries in gCompareAddress (D_800A3C20)
 * starting at gPadSlotIndex until gPadSlotIndex exceeds D_8009C77C or
 * func_8008A6CC returns nonzero. For each visited slot it skips when arg is
 * -9, zeroes D_8009C780[gPadSlotIndex] when arg is 0, or calls func_8008C77C
 * and func_8008C06C otherwise, then clears gPadDispatchIndex and
 * gSio0CtrlPtr->v before incrementing gPadSlotIndex.
 */
void gAdvancePadSlot(s32 arg)
{
    register s32 a __asm__("$5") = arg;
    register s32 base __asm__("$17") = (s32)gCompareAddress;
    register s32 neg __asm__("$19") = -9;
    register s32 *tab __asm__("$18") = gPadPendingCounts;
    s32 done;
    register s32 zero __asm__("$0");
    __asm__("" : "=r"(zero));
    __asm__("" : "+r"(a));
    __asm__("" : "+r"(base), "+r"(tab), "+r"(neg));
    do {
        struct E *p = (struct E *)(gPadSlotIndex * 240 + base);
        if (a != neg) {
            if (!a) {
                tab[gPadSlotIndex] = zero;
            } else {
                gUpdatePadTimer(p);
                gConsumePadCmd(p);
            }
        }
        gPadDispatchIndex = 0;
        gSio0CtrlPtr->v = 0;
        gPadSlotIndex++;
        done = (gPadEndSlot < gPadSlotIndex) ? 1 : gBeginPadSioTransfer((struct E *)(gPadSlotIndex * 240 + base));
        a = 0xFFFF;
    } while (!done);
}
