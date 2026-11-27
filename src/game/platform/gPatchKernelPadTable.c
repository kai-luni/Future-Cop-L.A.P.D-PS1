// Categories: Platform > CD-ROM | Platform > Hardware Init
#include "game/types.h"

extern u32 gSavedRaScratch __asm__("D_800A3E10");
extern u32 jtbl_800A3E18_main;
extern u32 jtbl_800A3E1C_main;
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gBiosFlushCache(void) __asm__("func_8008DBA0");

asm(".set jtbl_800A3E18_main, 0x800A3E18");
asm(".set jtbl_800A3E1C_main, 0x800A3E1C");
void gPatchKernelPadTable(void) __asm__("func_8008DB28");


/**
 * @brief Patches the CD driver base block and fills the jump-table pointers.
 *
 * The routine saves $ra, takes the kernel semaphore func_8008C9A0, calls BIOS
 * B0:0x57 to obtain the kernel's entry table and reads the word at +0x16C. From
 * that base it derives two pointers (base + 0x884 and base + 0x894) and stores
 * them into the local jump-table slots jtbl_800A3E18 and jtbl_800A3E1C, then
 * clears eleven words starting at base + 0x594. A cache flush (func_8008DBA0)
 * makes the patch visible to the DMA engine before $ra is restored.
 *
 * @par Globals
 * D_800A3E10 - saved return address scratch; jtbl_800A3E18, jtbl_800A3E1C -
 * patched jump-table pointers.
 *
 * @par Control flow
 * Straight-line prologue, one BIOS call, two derived pointers and a counted
 * clear loop of eleven words.
 *
 * @par Calls
 * func_8008C9A0 - semaphore. func_8008DBA0 - cache flush.
 *
 * @par Side effects
 * Writes kernel table pointers and clears eleven kernel words.
 *
 * @par Callers
 * Confirmed: the CD driver init chain (func_8008D784 neighbourhood).
 *
 * @par Role and path
 * Saves $ra in D_800A3E10, calls gEnterCriticalSection and BIOS B0:57 to read
 * the kernel entry pointer at +0x16C, stores base + 0x884 and base + 0x894
 * into jtbl_800A3E18_main and jtbl_800A3E1C_main, zeroes 11 words at base +
 * 0x594, and flushes cache via func_8008DBA0. Called by func_8008D7DC and
 * func_8008D86C.
 */
void gPatchKernelPadTable(void)
{
    __asm__ volatile(
        ".set noreorder\n\t"
        "lui $at, %%hi(D_800A3E10)\n\t"
        "sw $ra, %%lo(D_800A3E10)($at)\n\t"
        "jal func_8008C9A0\n\t"
        "addiu $t1, $zero, 0x57\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "lw $v0, 0x16C($v0)\n\t"
        "addiu $t1, $zero, 0xB\n\t"
        "addi $v1, $v0, 0x884\n\t"
        "lui $at, %%hi(jtbl_800A3E18_main)\n\t"
        "sw $v1, %%lo(jtbl_800A3E18_main)($at)\n\t"
        "addi $v1, $v0, 0x894\n\t"
        "lui $at, %%hi(jtbl_800A3E1C_main)\n\t"
        "sw $v1, %%lo(jtbl_800A3E1C_main)($at)\n"
        "1:\n\t"
        "sw $zero, 0x594($v0)\n\t"
        "addiu $v0, $v0, 4\n\t"
        "addiu $t1, $t1, -1\n\t"
        "bnez $t1, 1b\n\t"
        "nop\n\t"
        "jal func_8008DBA0\n\t"
        "lui $ra, %%hi(D_800A3E10)\n\t"
        "lw $ra, %%lo(D_800A3E10)($ra)\n\t"
        "nop\n\t"
        ".set reorder\n\t"
        :
        :
        : "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
