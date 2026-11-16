// Categories: Platform > Controller Input | BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

typedef void (*Handler)(s32 port);

extern s32 D_8009C760;
extern s32 *D_8009C788;
extern s32 D_8009C75C;
extern Handler D_8009C72C;
extern u8 D_800A3B60[];
extern s32 D_800A3B70[];

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008CA00(s32 arg0, u8 *arg1);
extern void func_8008CA10(s32 arg0, u8 *arg1);
extern void func_8008CA20(s32 arg0, s32 arg1);

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
 */
void func_8008A5BC(void)
{
    u8 *buf;
    register s32 *state __asm__("$3");
    register s32 *end __asm__("$2");
    register s32 arg __asm__("$4");
    register s32 *base __asm__("$1");

    __asm__ volatile("lui %0, %%hi(D_8009C760)" : "=r"(base));
    base[-0x38A0 / 4] = 0;
    func_8008C9A0();
    __asm__ volatile("" ::: "$16");
    buf = D_800A3B60;
    func_8008CA10(2, buf);
    func_8008CA00(2, buf);

    arg = 3;
    __asm__ volatile("" : "+r"(arg) :: "memory");
    state = D_8009C788;
    state[0] = -2;
    __asm__ volatile("" ::: "memory");
    state[1] |= 1;
    func_8008CA20(arg, 0);

    func_8008C9B0();

    D_8009C72C(D_8009C75C);
    D_8009C72C(D_8009C75C + 0xF0);

    end = D_800A3B70;
    __asm__ volatile("" : "+r"(end));
    end[1] = 0;
    end[0] = 0;
    D_8009C760 = 1;
}
