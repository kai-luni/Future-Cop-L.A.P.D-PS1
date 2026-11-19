// Categories: BIOS > System Calls | Platform > BIOS
#include "game/types.h"

extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void func_8008DBA0(void);

/**
 * @brief Zeroes 9 kernel words at offset +0x62C of the B0(0x57)+0x16C table and flushes instruction cache.
 *
 * Saves `$ra` in D_800A3E20, enters a critical section, queries BIOS vector `0xB0` function `0x57`, clears 9 consecutive 32-bit words starting at `*(*($v0 + 0x16C)) + 0x62C`, flushes cache via func_8008DBA0, and exits the critical section. Category BIOS > System Calls | Platform > BIOS follows handwritten BIOS vector `0xB0:0x57` and `0xA0:0x44` calls.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A3E20 - scratch word holding saved `$ra` across the inline-assembly BIOS calls.
 *
 * @par Control flow
 * Straight-line BIOS setup wrapping a 9-iteration countdown loop (`$t2 = 9`) clearing `0($v1)` by 4-byte steps.
 *
 * @par Calls
 * func_8008C9A0 - enters BIOS critical section (`syscall 0`, code 1).
 * func_8008DBA0 - invokes BIOS vector `0xA0` function `0x44` (cache flush).
 * func_8008C9B0 - exits BIOS critical section (`syscall 0`, code 2).
 *
 * @par Side effects
 * Saves/restores `$ra` via D_800A3E20, zeroes 36 bytes in kernel structure `+0x16C -> +0x62C..+0x64C`, and flushes cache.
 *
 * @par Callers
 * Confirmed: func_8008D7DC (0x8008D800 in asm/us/main.s) and func_8008D86C (0x8008D890 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8008DBB0(void)
{
    __asm__ volatile(
        ".set noreorder\n\t"
        "lui $at, 0x800A\n\t"
        "sw $ra, 0x3E20($at)\n\t"
        "jal func_8008C9A0\n\t"
        "addiu $t1, $zero, 0x57\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "addiu $t2, $zero, 0x9\n\t"
        "lw $v0, 0x16C($v0)\n\t"
        "nop\n\t"
        "addi $v1, $v0, 0x62C\n"
        "1:\n\t"
        "sw $zero, 0($v1)\n\t"
        "addiu $v1, $v1, 4\n\t"
        "addiu $t2, $t2, -1\n\t"
        "bnez $t2, 1b\n\t"
        "nop\n\t"
        "jal func_8008DBA0\n\t"
        "jal func_8008C9B0\n\t"
        "lui $ra, 0x800A\n\t"
        "lw $ra, 0x3E20($ra)\n\t"
        "nop\n\t"
        ".set reorder"
        :
        :
        : "$1", "$2", "$3", "$4", "$5", "$6", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
