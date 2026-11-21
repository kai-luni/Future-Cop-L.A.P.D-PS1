// Categories: BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

extern s32 gIrqNode __asm__("D_800A3E00");
extern s32 gIrqNodeHandler[] __asm__("D_800A3E04");
extern s32 gIrqNodeLinkEnd __asm__("D_800A3E0C");
extern u8 gSio0IrqChainClearCallback[] __asm__("D_8008DA14");
extern u8 gIrqBit0PersistentCheckCallback[] __asm__("D_8008DA7C");

extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void func_8008CA00(s32, void *);
extern void func_8008CA10(s32, void *);
s32 gInstallPriority1Irq(void) __asm__("func_8008D964");


/**
 * @brief Initializes IRQ-chain node D_800A3E00 and re-registers it at BIOS priority 1 inside a critical section.
 *
 * Enters a critical section, writes handler pointers D_8008DA14 (+0x04) and D_8008DA7C (+0x08) with zeroed links (+0x00/+0x0C) into D_800A3E00, dequeues and re-enqueues the node via BIOS vector `0xC0` subfunctions 3 and 2, and exits the critical section. Category BIOS > System Calls | Platform > Interrupts follows the IRQ-chain node and critical-section syscalls.
 *
 * @param void No parameters.
 * @return Always `1`.
 *
 * @par Globals
 * D_800A3E00, D_800A3E04, D_800A3E0C - 16-byte IRQ-chain node; D_8008DA14, D_8008DA7C - handler/predicate addresses stored at +0x04/+0x08.
 *
 * @par Control flow
 * Straight-line critical-section sequence with no branches or loops; returns `1`.
 *
 * @par Calls
 * func_8008C9A0, func_8008C9B0 - enter/exit BIOS critical section; func_8008CA10 - BIOS `0xC0:0x03` dequeue `(1, node)`; func_8008CA00 - BIOS `0xC0:0x02` enqueue `(1, node)`.
 *
 * @par Side effects
 * Populates D_800A3E00..D_800A3E0C and links it onto the BIOS priority-1 IRQ handler list.
 *
 * @par Callers
 * Confirmed: func_8008D7DC (0x8008D828 in asm/us/main.s) and func_8008D86C (0x8008D8B8 in asm/us/main.s) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Populates the 16-byte IRQ chain node at D_800A3E00..D_800A3E0C with handler
 * pointers D_8008DA14 and D_8008DA7C inside a critical section. Dequeues and
 * re-enqueues the node at BIOS priority 1 using func_8008CA10 (C0:03) and
 * func_8008CA00 (C0:02), then returns 1. Called by func_8008D7DC and
 * func_8008D86C.
 */
s32 gInstallPriority1Irq(void)
{
    register s32 *p __asm__("$3");
    register s32 one __asm__("$4");
    s32 *q;

    gEnterCriticalSection();

    one = 1;
    p = gIrqNodeHandler;
    __asm__ volatile("" : "=r"(p) : "0"(p), "r"(one));
    q = p - 1;
    p[0] = (s32) gSio0IrqChainClearCallback;
    p[1] = (s32) gIrqBit0PersistentCheckCallback;
    gIrqNode = 0;
    gIrqNodeLinkEnd = 0;

    func_8008CA10(one, q);
    func_8008CA00(1, q);
    gExitCriticalSection();

    return 1;
}
