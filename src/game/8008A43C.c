// Categories: Input > Port Processing
#include "game/sio_globals.h"

extern int D_8009C778, D_8009C77C, D_8009C76C, D_8009C770;
extern int gPadSlotIndex __asm__("D_8009C768");
extern int gPadPresentFlag __asm__("D_8009C760");
extern int D_800A3B70, D_800A3B74;
extern char *D_8009C75C;
extern short *gSio0RegBasePtr __asm__("D_8009C78C");
extern void (*D_8009C728)(int);
extern int func_8008A6CC(char *);
extern void func_8008A8E0(char *);

/**
 *  @brief Processes the configured range of port objects when the subsystem is ready.
 *
 *  Two counters are conditionally incremented up to 150 before the object range is serviced.
 *  The routine also writes a status value through the configured short pointer.
 *  @return Always zero.
 *  @par Globals
 *  D_8009C778 and D_8009C77C bound the range; D_8009C760 gates processing; D_8009C75C points to objects.
 *  D_800A3B70 and D_800A3B74 are counters; D_8009C76C, D_8009C768 and D_8009C770 hold processing state.
 *  D_8009C728 is a dispatch pointer and D_8009C78C receives the status write; an assembly-addressed global is also set to 1.
 *  @par Control flow
 *  If ready and the range is valid, the first object is checked and a loop processes objects through the upper bound.
 *  @par Calls
 *  func_8008A6CC checks the first object; D_8009C728 is dispatched on a zero result; func_8008A8E0 processes each loop iteration.
 *  @par Side effects
 *  Updates counters, processing globals and status storage, and invokes the processing callbacks.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_8008A43C(void)
{
    register int v asm("$3") = D_8009C778;
    int one = 1;
    register int *r asm("$1");
    asm("" : "+r"(v));
    asm("lui %0,%%hi(D_8009C790)" : "=r"(r) : : "memory");
    *(int *)((char *)r - 14448) = one;
    if (v != 0) {
        register int *p asm("$4") = &D_800A3B70;
        asm("" : "+r"(p));
        if (*p < 150) {
            *p = *p + 1;
        }
    }
    if (D_8009C77C == 0) {
        register int *q asm("$4") = &D_800A3B74;
        asm("" : "+r"(q));
        if (*q < 150) {
            *q = *q + 1;
        }
    }
    if (gPadPresentFlag != 0) {
        if (D_8009C778 <= D_8009C77C) {
            D_8009C76C = 0;
            gPadSlotIndex = D_8009C778;
            if (func_8008A6CC(D_8009C75C + D_8009C778 * 240) == 0) {
                D_8009C728(0xFFFF);
            }
            D_8009C770 = 0;
            while (gPadSlotIndex <= D_8009C77C) {
                func_8008A8E0(D_8009C75C + gPadSlotIndex * 240);
            }
            gSio0RegBasePtr[7] = 0x88;
        }
    }
    return 0;
}
