// Categories: Platform > Interrupts | Game Engine > Init
#include "game/types.h"

extern s32 gGpuLibSavedRa __asm__("D_800A63D0");
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void gBiosFlushCache(void) __asm__("func_8008DBA0");

asm(".set D_80095E5C, 0x80095E5C");
asm(".set D_80095E74, 0x80095E74");
asm(".set D_80095E8C, 0x80095E8C");
void gPatchBiosC0Table(void) __asm__("func_80095DB0");


/**
 * @brief Patches six words of the BIOS C0 table when they match a known
 *        signature, inside a critical section.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Saves $ra to D_800A63D0, EnterCriticalSection (syscall 1), GetC0Table
 * (BIOS B0:0x56) returns the table base; the code reads table[0x18] + 0x28
 * and compares six consecutive words against the signature at
 * D_80095E5C..D_80095E74. On a full match it copies the six replacement
 * words from D_80095E74..D_80095E8C into the table, calls FlushCache
 * (A0:0x44 via func_8008DBA0), ExitCriticalSection (syscall 2) and restores
 * $ra. On a mismatch nothing is written.
 *
 * @par Likely role
 * A kernel redirect installed at boot: the C0 table entry at +0x18 is the
 * interrupt/exception dispatch, and the patch swaps in the game's own
 * handler words (likely the VBlank/DMA routing the sound and movie paths
 * rely on).
 *
 * @par Unknown
 * Which handler the six replacement words point at, and why the patch is
 * signature-gated (BIOS version check).
 *
 * @par Callers
 * func_80095878.
 */
void gPatchBiosC0Table(void)
{
    __asm__ volatile(
        ".set\tnoreorder\n\t"
        "lui $at, %%hi(D_800A63D0)\n\t"
        "sw $ra, %%lo(D_800A63D0)($at)\n\t"
        "jal func_8008C9A0\n\t"
        "nop\n\t"
        "addiu $t1, $zero, 0x56\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "lw $v0, 0x18($v0)\n\t"
        "nop\n\t"
        "addiu $v0, $v0, 0x28\n\t"
        "addu $t7, $v0, $zero\n\t"
        "lui $t2, %%hi(D_80095E5C)\n\t"
        "addiu $t2, $t2, %%lo(D_80095E5C)\n\t"
        "lui $t1, %%hi(D_80095E74)\n\t"
        "addiu $t1, $t1, %%lo(D_80095E74)\n"
        "1:\n\t"
        "lw $v1, 0($t2)\n\t"
        "lw $t3, 0($v0)\n\t"
        "addiu $t2, $t2, 4\n\t"
        "bne $v1, $t3, 9f\n\t"
        "addiu $v0, $v0, 4\n\t"
        "bne $t2, $t1, 1b\n\t"
        "nop\n\t"
        "addu $v0, $t7, $zero\n\t"
        "lui $t2, %%hi(D_80095E74)\n\t"
        "addiu $t2, $t2, %%lo(D_80095E74)\n\t"
        "lui $t1, %%hi(D_80095E8C)\n\t"
        "addiu $t1, $t1, %%lo(D_80095E8C)\n"
        "2:\n\t"
        "lw $v1, 0($t2)\n\t"
        "nop\n\t"
        "sw $v1, 0($v0)\n\t"
        "addiu $t2, $t2, 4\n\t"
        "bne $t2, $t1, 2b\n\t"
        "addiu $v0, $v0, 4\n"
        "9:\n\t"
        "jal func_8008DBA0\n\t"
        "nop\n\t"
        "jal func_8008C9B0\n\t"
        "nop\n\t"
        "lui $ra, %%hi(D_800A63D0)\n\t"
        "lw $ra, %%lo(D_800A63D0)($ra)\n\t"
        "nop\n\t"
        ".set\treorder\n\t"
        :
        :
        : "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
