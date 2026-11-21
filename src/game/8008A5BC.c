// Categories: Platform > Controller Input | BIOS > System Calls | Platform > Interrupts
#include "game/intc_globals.h"

#include "game/types.h"

typedef void (*Handler)(s32 port);

extern s32 gPadPresentFlag __asm__("D_8009C760");
extern s32 *gInterruptStatusPtr __asm__("D_8009C788");
extern s32 gPadBasePort __asm__("D_8009C75C");
extern Handler gPadResetPortHook __asm__("D_8009C72C");
extern u8 gPadIrqNode[] __asm__("D_800A3B60");
extern s32 gPadPort0IdleCount[] __asm__("D_800A3B70");

extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void gBiosEnqueueInterruptHandler(s32 arg0, u8 *arg1) __asm__("func_8008CA00");
extern void gBiosSysDeqIntRp(s32 arg0, u8 *arg1) __asm__("func_8008CA10");
extern void gBiosChangeClearRCnt(s32 arg0, s32 arg1) __asm__("func_8008CA20");
void gInitPadDriverIrq(void) __asm__("func_8008A5BC");


/**
 * @brief Installs the pad interrupt handler in BIOS chain 2, enables VSync IRQ0, and resets pad ports.
 *
 * Enters a critical section, re-enqueues the interrupt packet D_800A3B60 on BIOS
 * chain 2, acknowledges and unmasks VSync IRQ0 in I_STAT/I_MASK (D_8009C788),
 * configures root counter 3 via func_8008CA20, resets both pad ports, and arms D_8009C760.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009C760 - pad polling enable flag; D_8009C788 - 0x1F801070 (I_STAT/I_MASK); D_800A3B60 - IRQ handler node; D_8009C72C/D_8009C75C - port reset callback/array; D_800A3B70 - 2-word pad timer/status array.
 *
 * @par Control flow
 * Linear hardware/BIOS sequence bracketed by func_8008C9A0 and func_8008C9B0, followed by two port-reset calls via D_8009C72C.
 *
 * @par Calls
 * func_8008C9A0/func_8008C9B0 - BIOS Enter/ExitCriticalSection; func_8008CA10/func_8008CA00 - BIOS SysDeqIntRP/SysEnqIntRP(2); func_8008CA20 - ChangeClearRCnt(3, 0); D_8009C72C - resets ports 0 and 1.
 *
 * @par Side effects
 * Updates I_STAT/I_MASK (0x1F801070/74), BIOS IRQ chain 2, both pad port structs, D_800A3B70[0..1] = 0, and D_8009C760 = 1.
 *
 * @par Callers
 * Confirmed: func_80089E00 (0x80089E08, pad subsystem start wrapper).
 *
 * @par Role and path
 * Enters a critical section via func_8008C9A0, dequeues and re-enqueues
 * D_800A3B60 on BIOS chain 2 via func_8008CA10 and func_8008CA00,
 * acknowledges and unmasks VSync IRQ0 in gInterruptStatusPtr, configures root
 * counter 3 via func_8008CA20, and exits the critical section via
 * func_8008C9B0. It then resets both pad ports via D_8009C72C, clears
 * D_800A3B70[0..1], and sets gPadPresentFlag to 1. Called by func_80089E00.
 */
void gInitPadDriverIrq(void)
{
    u8 *buf;
    register s32 *state __asm__("$3");
    register s32 *end __asm__("$2");
    register s32 arg __asm__("$4");
    register s32 *base __asm__("$1");

    __asm__ volatile("lui %0, %%hi(D_8009C760)" : "=r"(base));
    base[-0x38A0 / 4] = 0;
    gEnterCriticalSection();
    __asm__ volatile("" ::: "$16");
    buf = gPadIrqNode;
    gBiosSysDeqIntRp(2, buf);
    gBiosEnqueueInterruptHandler(2, buf);

    arg = 3;
    __asm__ volatile("" : "+r"(arg) :: "memory");
    state = gInterruptStatusPtr;
    state[0] = -2;
    __asm__ volatile("" ::: "memory");
    state[1] |= 1;
    gBiosChangeClearRCnt(arg, 0);

    gExitCriticalSection();

    gPadResetPortHook(gPadBasePort);
    gPadResetPortHook(gPadBasePort + 0xF0);

    end = gPadPort0IdleCount;
    __asm__ volatile("" : "+r"(end));
    end[1] = 0;
    end[0] = 0;
    gPadPresentFlag = 1;
}
