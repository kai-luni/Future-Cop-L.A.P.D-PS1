// Categories: Platform > Memory Card | BIOS > System Calls | Platform > Interrupts
#include "game/types.h"

extern void func_8008D7B0(s32 arg0);
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern s32 gGetModeClearFlag(void) __asm__("func_8008D7CC");
extern void func_8008DC20(s32 arg0);
extern void gCopyKernelPatch0xDF80(void) __asm__("func_8008DDEC");
extern void gCopyCardPatchTable(void) __asm__("func_8008DCE8");
extern void gInstallCardPatchSlot(void) __asm__("func_8008DD7C");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
void gInitCardAndPatches(s32 mode) __asm__("func_8008D6E0");


/**
 * @brief Initializes BIOS memory-card support and installs kernel card/pad table patches.
 *
 * Calls func_8008D7B0(0) (B0:5B), enters a critical section, clears the mode argument when func_8008D7CC() (D_8009C800) is 0, runs func_8008DC20 (B0:4A, InitCARD), installs the 0xDF80 patch and updates kernel C0/B0 tables, then exits the critical section.
 *
 * @param mode Initialization flag forwarded to func_8008DC20 (overridden to 0 if D_8009C800 == 0).
 *
 * @par Globals
 * None directly; func_8008D7CC reads D_8009C800, while patch helpers write 0xDF80, D_800A3E30, and kernel C0/B0 tables.
 *
 * @par Control flow
 * Conditional override of temp_s0 to 0 when func_8008D7CC() == 0; otherwise straight-line setup sequence.
 *
 * @par Calls
 * func_8008D7B0 (BIOS B0:5B), func_8008C9A0/func_8008C9B0 (critical section), func_8008D7CC (read D_8009C800), func_8008DC20 (BIOS B0:4A), func_8008DDEC (copy patch to 0xDF80), func_8008DCE8/func_8008DD7C (patch C0/B0 BIOS tables).
 *
 * @par Side effects
 * Masks interrupts, configures BIOS card/pad services, writes patch code to 0xDF80, and patches kernel C0/B0 tables.
 *
 * @par Callers
 * Confirmed: gInitMemoryCardBios (0x800200B8).
 *
 * @par Role and path
 * Initializes BIOS memory-card support inside a critical section bracketed by
 * gEnterCriticalSection and gExitCriticalSection. Calls func_8008D7B0(0),
 * overrides mode to 0 if func_8008D7CC returns 0 (D_8009C800), invokes
 * func_8008DC20(temp_s0), and installs kernel patches via func_8008DDEC,
 * func_8008DCE8, and func_8008DD7C. Driven by gInitMemoryCardBios at
 * 0x800200B8.
 */
void gInitCardAndPatches(s32 mode)
{
    s32 temp_s0;

    temp_s0 = mode;
    func_8008D7B0(0);
    gEnterCriticalSection();
    if (gGetModeClearFlag() == 0) {
        temp_s0 = 0;
    }
    func_8008DC20(temp_s0);
    gCopyKernelPatch0xDF80();
    gCopyCardPatchTable();
    gInstallCardPatchSlot();
    gExitCriticalSection();
}
