// Categories: Platform > Memory Card | Resources > File Access
typedef unsigned char u8;
typedef struct { int a, b, c, pos, e, f; } Entry;
typedef struct { int state, a, b, c, index, d, callback, saved, buffer; } State;
extern State func_80010000;
extern Entry gFileSlotTable[] __asm__("D_800A6D78");
extern int gCdCommandParam[] __asm__("D_8009F610");
extern int gCdTargetLoc[] __asm__("D_8009F614");
extern int gCdCurrentLoc[] __asm__("D_8009F61C");
extern u8 gEventBuffer[] __asm__("D_800A0798");
extern volatile u8 gCdStatusReg __asm__("D_1F801800");
extern volatile u8 gCdInterruptReg __asm__("D_1F801803");
extern void gByteOffsetToCdlLoc(int *, int) __asm__("func_8001C494");
extern int gCdReady(int, int) __asm__("func_8008E174");
extern int gCdControl(int, void *, void *) __asm__("func_8008E1BC");
extern void gBiosExit(int) __asm__("func_8008CC80");
extern int gSetCdSyncCallback(void *) __asm__("func_8008E194");
extern void gCdSeekDoneCallback(void) __asm__("func_8001C614");
extern void gCopyWord(int *, int *) __asm__("func_8001C27C");
void gStartCdDirRead(void) __asm__("func_8001C718");

extern int gCdControlF(int, void *) __asm__("func_8008E2F8");
/**
 * @brief Reads the directory block from the memory card into the loader buffer.
 *
 * @return Nothing.
 *
 * @par Confirmed flow (follow-up of func_8001C680)
 * 1. The loader's current file slot is taken from D_800A6D78[func_80010000.index]
 *    and its position is passed to func_8001C494 together with the card buffer
 *    D_8009F614.
 * 2. The card interface is polled with func_8008E174(1, 0), the control
 *    registers D_1F801800/D_1F801803 are cleared and func_8008E1BC(0xE, ...)
 *    starts a directory read; a failure raises func_8008CC80(-1).
 * 3. The loader state (at 0x80010000, i.e. the first word of the program image)
 *    is set to 1 and func_8008E194(&func_8001C614) registers the callback that
 *    finishes the sequence.
 * 4. func_8001C27C(&D_8009F61C, &D_8009F614) refreshes the two buffers and
 *    func_8008E2F8(6, ...) opens the card.
 *
 * @par Globals
 * D_8009F610, D_8009F614, D_8009F61C (card buffers), D_800A6D78 (loader slots),
 * D_80010000 (loader state block).
 *
 * @par Callers
 * func_8001C808.
 *
 * @par Role and path
 * Encodes gFileSlotTable[0x80010000.index].pos into gCdTargetLoc via
 * func_8001C494, polls func_8008E174(1, 0), clears CD-ROM registers
 * gCdStatusReg and gCdInterruptReg, and issues CD command 0xE via
 * func_8008E1BC. Sets 0x80010000.state = 1, installs func_8001C614 via
 * func_8008E194, copies gCdTargetLoc to gCdCurrentLoc via func_8001C27C, and
 * calls func_8008E2F8(6, ...); called by func_8001C808.
 */
void gStartCdDirRead(void)
{
    register State *state __asm__("$16");
    int result;
    register int saved __asm__("$2");
    register State *p __asm__("$3");
    register int a0 __asm__("$4");
    register char *a1 __asm__("$5");
    register char *a2 __asm__("$6");
    a1 = (char *)gFileSlotTable[func_80010000.index].pos;
    __asm__ volatile("" : "+r"(a1));
    a0 = (int)gCdTargetLoc;
    __asm__ volatile("" : "+r"(a0));
    gByteOffsetToCdlLoc((int *)a0, (int)a1);
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
    result = gCdControl(a0, a1, a2 + 0x798);
    state = (State *)0x80010000;
    if (!result)
        gBiosExit(-1);
    __asm__ volatile("" : "+r"(state));
    a0 = (int)gCdSeekDoneCallback;
    __asm__ volatile("" : "+r"(a0));
    state->state = 1;
    __asm__ volatile("" : : : "memory");
    saved = gSetCdSyncCallback((void *)a0);
    __asm__ volatile("" : "+r"(saved));
    a0 = 0x800a0000;
    __asm__ volatile("" : "+r"(a0));
    a0 -= 0x9e4;
    __asm__ volatile("" : "+r"(a0));
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1), "+r"(saved));
    __asm__ volatile("addiu %0,%1,0" : "=r"(p) : "r"(state));
    p->saved = saved;
    gCopyWord((int *)a0, (int *)a1);
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    if (!gCdControlF(6, a1))
        state->state = 0;
}
