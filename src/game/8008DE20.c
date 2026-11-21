// Categories: BIOS > System Calls | Platform > BIOS
#include "game/types.h"

extern u32 gCdKernelPatchWords[] __asm__("D_8008DE90");
extern u32 gCdKernelPatchWordsEnd[] __asm__("D_8008DE9C");
extern s32 gRaParkPatch __asm__("D_800A3E40");
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void gExitCriticalSection(void) __asm__("func_8008C9B0");
extern void func_8008DBA0(void);
void gPatchCdKernelBlock(void) __asm__("func_8008DE20");


/**
 * @brief Copies 3 words from D_8008DE90..D_8008DE9C into offset +0x70 of B0(0x56)+0x18 and flushes cache.
 *
 * Saves `$ra` in D_800A3E40, enters a critical section, calls BIOS vector `0xB0` function `0x56`, dereferences `*($v0 + 0x18)`, copies the three words D_8008DE90..D_8008DE98 to `base + 0x70..+0x78`, flushes cache via func_8008DBA0, and exits the critical section. Category BIOS > System Calls | Platform > BIOS follows BIOS vectors `0xB0:0x56` and `0xA0:0x44`.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A3E40 - scratch word holding saved `$ra`; D_8008DE90..D_8008DE9C - 3-word source range (all zero in asm/us/main.s).
 *
 * @par Control flow
 * Straight-line BIOS query, 3-iteration `do-while` word copy (`src != end`), cache flush, and critical-section exit.
 *
 * @par Calls
 * func_8008C9A0 - enters BIOS critical section (`syscall 0`, code 1).
 * func_8008DBA0 - invokes BIOS vector `0xA0` function `0x44` (cache flush).
 * func_8008C9B0 - exits BIOS critical section (`syscall 0`, code 2).
 *
 * @par Side effects
 * Saves/restores `$ra` via D_800A3E40, overwrites 12 bytes at `*(B0(0x56)+0x18) + 0x70`, and flushes cache.
 *
 * @par Callers
 * Confirmed: func_8008D784 (0x8008D794 in asm/us/main.s, reached from func_800200F0) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Saves $ra in gRaParkPatch (D_800A3E40), enters a critical section via
 * gEnterCriticalSection, queries BIOS B0:56 +0x18, and copies 3 words from
 * D_8008DE90..D_8008DE9C into base + 0x70. Flushes cache via func_8008DBA0
 * and exits the critical section via gExitCriticalSection. Called by
 * func_8008D784.
 */
void gPatchCdKernelBlock(void)
{
    register u32 value __asm__("$3");
    register u32 *src __asm__("$10");
    register u32 *end __asm__("$9");
    register u32 *base __asm__("$2");

    __asm__ volatile(
        "lui $at, %%hi(D_800A3E40)\n\t"
        "sw $ra, %%lo(D_800A3E40)($at)\n\t"
        "jal func_8008C9A0\n\t"
        "addiu $t1, $zero, 0x56\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "lw $v0, 0x18($v0)\n\t"
        "lui $t2, %%hi(D_8008DE90)\n\t"
        "addiu $t2, $t2, %%lo(D_8008DE90)\n\t"
        "lui $t1, %%hi(D_8008DE9C)\n\t"
        "addiu $t1, $t1, %%lo(D_8008DE9C)\n\t"
        : "=r"(base), "=r"(src), "=r"(end)
        :
        : "$1", "$4", "$5", "$6", "$7", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");

    do {
        value = *src;
        *(u32 *)((u8 *)base + 0x70) = value;
        __asm__ volatile("" : "+r"(src) : : "memory");
        src++;
        base++;
    } while (src != end);

    __asm__ volatile(
        ".set noreorder\n\t"
        "jal func_8008DBA0\n\t"
        "jal func_8008C9B0\n\t"
        "lui $ra, %%hi(D_800A3E40)\n\t"
        "lw $ra, %%lo(D_800A3E40)($ra)\n\t"
        "nop\n\t"
        ".set reorder\n\t"
        :
        :
        : "$1", "$4", "$5", "$6", "$7", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
