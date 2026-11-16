// Categories: BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

extern u8 D_800A3B60[];

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008CA10(s32 arg0, u8 *arg1);
extern void func_8008CA20(s32 arg0, s32 arg1);

/**
 * @brief Disables BIOS IRQ3 clear control and dequeues interrupt-chain node D_800A3B60 in a critical section.
 *
 * Enters a critical section, calls BIOS vector 0xC0 subfunction 0x0A with `(3, 1)`, unlinks D_800A3B60 from the priority-2 IRQ chain via vector 0xC0 subfunction 3, and exits the critical section. Category BIOS > System Calls | Platform > Interrupts follows the BIOS vector wrappers and node D_800A3B60.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A3B60 - priority-2 BIOS interrupt-chain node registered by func_8008A5BC and unlinked here.
 *
 * @par Control flow
 * Straight-line four-call teardown sequence; no branches or loops.
 *
 * @par Calls
 * func_8008C9A0 - enters BIOS critical section (`syscall 0`, code 1).
 * func_8008CA20 - invokes BIOS vector 0xC0 subfunction 0x0A with `(3, 1)`.
 * func_8008CA10 - invokes BIOS vector 0xC0 subfunction 3 to dequeue `(2, D_800A3B60)`.
 * func_8008C9B0 - exits BIOS critical section (`syscall 0`, code 2).
 *
 * @par Side effects
 * Masks interrupts across the BIOS updates, sets vector 0xC0:0x0A flag 1 for slot 3, and unlinks D_800A3B60.
 *
 * @par Callers
 * Confirmed: func_80089E20 (0x80089E28 in asm/us/main.s, reached from func_8001E910 / func_8001E540) / Likely: none / Unknown: none.
 */
void func_8008A688(void)
{
    func_8008C9A0();
    func_8008CA20(3, 1);
    func_8008CA10(2, D_800A3B60);
    func_8008C9B0();
}
