// Categories: BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

extern void func_8008DBB0(void);
extern void func_8008C9A0(void);
extern void func_8008DB28(void);
extern void func_8008C9B0(void);
extern void func_8008D7B0(s32);
extern void func_8008D964(void);
extern void func_8008DAF0(s32, s32, s32, s32);
extern s32 D_8009C800;

/**
 * @brief Initializes BIOS/kernel device tables, installs IRQ node D_800A3E00, and invokes BIOS B0:0x15.
 *
 * Clears 9 kernel words via func_8008DBB0, patches jump tables and 11 words via func_8008DB28 inside a critical section, runs func_8008D7B0(0), installs priority-1 IRQ node D_800A3E00 via func_8008D964, forwards `(arg0..arg3)` to func_8008DAF0 (`0xB0:0x15`), and sets D_8009C800 = 1. Category BIOS > System Calls | Platform > Interrupts follows the BIOS vector and IRQ-chain helpers.
 *
 * @param arg0, arg1 First two words forwarded unchanged to func_8008DAF0.
 * @param arg2, arg3 Last two words forwarded unchanged to func_8008DAF0.
 *
 * @par Globals
 * D_8009C800 - active/initialized status word set to `1` at exit.
 *
 * @par Control flow
 * Straight-line seven-call setup sequence with no branches or loops.
 *
 * @par Calls
 * func_8008DBB0, func_8008DB28 - clear kernel words (`B0:0x57`) and set jtbl_800A3E18/1C; func_8008C9A0/func_8008C9B0 - critical section around func_8008DB28.
 * func_8008D7B0 - calls `B0:0x5B` with `0`; func_8008D964 - enqueues IRQ node D_800A3E00; func_8008DAF0 - BIOS `B0:0x15` with `(arg0..arg3)`.
 *
 * @par Side effects
 * Updates kernel BIOS tables, installs priority-1 IRQ handler D_800A3E00, invokes `B0:0x15`, and sets D_8009C800 = 1.
 *
 * @par Callers
 * Confirmed: none / Likely: unreferenced BIOS/device init variant (sibling of func_8008D86C) / Unknown: any runtime caller.
 */
void func_8008D7DC(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    func_8008DBB0();
    func_8008C9A0();
    func_8008DB28();
    func_8008C9B0();
    func_8008D7B0(0);
    func_8008D964();
    func_8008DAF0(arg0, arg1, arg2, arg3);
    D_8009C800 = 1;
    __asm__ volatile("" : : : "memory");
}
