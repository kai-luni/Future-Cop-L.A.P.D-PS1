// Categories: Platform > Memory Card | Platform > Serial Port
#include "game/types.h"
extern volatile unsigned char gCdStatusReg __asm__("D_1F801800");
extern volatile unsigned char gCdInterruptReg __asm__("D_1F801803");
extern int gCdCurrentLoc __asm__("D_8009F61C");
extern int gCdTargetLoc __asm__("D_8009F614");
extern int gCdReady(int, void *) __asm__("func_8008E174");
extern int gCdControl(int, void *, void *) __asm__("func_8008E1BC");
extern void func_8008CC80(int);
extern void gCopyWord(int *, int *) __asm__("func_8001C27C");
void gResetCdReadState(void) __asm__("func_8001C680");

extern int gCdControlF(int, void *) __asm__("func_8008E2F8");
/**
 * @brief Initialises the memory-card interface on expansion port 1.
 *
 * @return Nothing.
 *
 * @par Confirmed flow (this is not a graphics unit; the selection filter caught
 *      its hardware register)
 * 1. func_8008E174(1, 0) is polled until it returns 0 (the card interface is
 *    free).
 * 2. D_1F801800 and D_1F801803 are cleared - the two memory-card control/status
 *    registers of the serial port.
 * 3. func_8008E1BC(0xE, &D_8009F610, &D_8009F798) starts the card; on failure
 *    func_8008CC80(-1) raises an error.
 * 4. func_8001C27C(&D_8009F61C, &D_8009F614) sets up the two buffers, and
 *    func_8008E2F8(6, &D_8009F614) opens the card; on failure the first word of
 *    the loaded image (0x80010000) is zeroed - the "no card" marker the loader
 *    checks.
 *
 * @par Globals
 * D_1F801800/D_1F801803 (card registers), D_8009F61C, D_8009F614 (buffer
 * pointers).
 *
 * @par Callers
 * func_8001C808.
 *
 * @par Role and path
 * Polls func_8008E174(1, 0), clears CD-ROM registers gCdStatusReg and
 * gCdInterruptReg, issues CD command 0xE via func_8008E1BC (calling
 * func_8008CC80(-1) on failure), copies gCdTargetLoc to gCdCurrentLoc via
 * func_8001C27C, and issues func_8008E2F8(6, ...) while clearing 0x80010000
 * on failure. Called by func_8001C808 when the transfer retry countdown
 * expires.
 */
void gResetCdReadState(void)
{
    register int *p __asm__("$2");
    register int a0 __asm__("$4");
    register char *a1 __asm__("$5");
    register char *a2 __asm__("$6");
    while (gCdReady(1, 0)) {}
    a0 = 14;
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9f0;
    a2 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a0), "+r"(a1), "+r"(a2));
    gCdStatusReg = 0;
    gCdInterruptReg = 0;
    __asm__ volatile("" : "+r"(a2) : : "memory");
    if (!gCdControl(a0, a1, a2 + 0x798))
        func_8008CC80(-1);
    a0 = 0x800a0000;
    __asm__ volatile("" : "+r"(a0));
    a0 -= 0x9e4;
    __asm__ volatile("" : "+r"(a0));
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    gCopyWord((int *)a0, (int *)a1);
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    if (!gCdControlF(6, a1)) {
        p = (int *)0x80010000;
        __asm__ volatile("" : "+r"(p));
        *p = 0;
    }
}
