// Categories: Platform > CD-ROM | Platform > Hardware Init
#include "game/types.h"

extern u32 gCardPatchJumpStubStart[] __asm__("D_8008DCC0");
extern u32 gCardPatchCallStubStart[] __asm__("D_8008DCD4");
extern s32 gCardPatchSavedReturnAddress __asm__("D_800A3E30");
extern void gEnterCriticalSection(void) __asm__("func_8008C9A0");
extern void func_8008DBA0(void);
void gCopyCardPatchTable(void) __asm__("func_8008DCE8");


/**
 * @brief Writes a prepared CD driver table into the kernel base block.
 *
 * BIOS B0:0x56 returns the kernel base; the routine combines the two halfwords
 * at +0x70 and +0x74 into an offset, adds 0x28 and uses the result as the
 * destination inside that base. The local table D_8008DCD4 (bounded by
 * D_8008DCC0) is copied word by word, then the cache is flushed and the
 * kernel's own pointer slot at 0x8009DFFC receives the new address.
 *
 * @par Globals
 * D_8008DCC0 / D_8008DCD4 - local table bounds; D_800A3E30 - saved return
 * address scratch; the kernel base is reached through BIOS B0:0x56.
 *
 * @par Control flow
 * BIOS call, offset computation, one copy loop, flush, one store.
 *
 * @par Calls
 * func_8008C9A0 - semaphore. func_8008DBA0 - cache flush.
 *
 * @par Side effects
 * Overwrites kernel memory and republishes the pointer.
 *
 * @par Callers
 * Confirmed: func_8008D784 (driver init pair). Likely: func_8008DD7C (same shape).
 *
 * @par Role and path
 * Calls gEnterCriticalSection and BIOS B0:56 to compute a kernel destination
 * from +0x70/+0x74 of *($v0 + 0x18) plus 0x28, then copies words from
 * D_8008DCD4 to D_8008DCC0. Flushes cache via func_8008DBA0, writes the
 * destination pointer to 0xDFFC, and restores $ra from D_800A3E30. Called by
 * func_8008D6E0.
 */
void gCopyCardPatchTable(void)
{
    register u32 *dst __asm__("$2");
    register u32 *src __asm__("$10");
    register u32 *end __asm__("$9");
    register u32 value __asm__("$3");

    __asm__ volatile(
        "lui $at, %%hi(D_800A3E30)\n\t"
        "sw $ra, %%lo(D_800A3E30)($at)\n\t"
        "jal func_8008C9A0\n\t"
        "addiu $t1, $zero, 0x56\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "lw $v0, 0x18($v0)\n\t"
        "nop\n\t"
        "lw $v1, 0x70($v0)\n\t"
        "nop\n\t"
        "andi $t1, $v1, 0xFFFF\n\t"
        "sll $t1, $t1, 16\n\t"
        "lw $v1, 0x74($v0)\n\t"
        "nop\n\t"
        "andi $t2, $v1, 0xFFFF\n\t"
        "addu $v1, $t1, $t2\n\t"
        "addiu $v0, $v1, 0x28\n\t"
        "lui $t2, %%hi(D_8008DCC0)\n\t"
        "addiu $t2, $t2, %%lo(D_8008DCC0)\n\t"
        "lui $t1, %%hi(D_8008DCD4)\n\t"
        "addiu $t1, $t1, %%lo(D_8008DCD4)\n\t"
        : "=r"(dst), "=r"(src), "=r"(end)
        :
        : "$1", "$4", "$5", "$6", "$7", "$8", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");

    do {
        value = *src;
        *dst = value;
        src++;
        dst++;
    } while (src != end);

    __asm__ volatile(
        ".set\tnoreorder\n\t"
        "lui $at, 0x1\n\t"
        "jal func_8008DBA0\n\t"
        "sw $v0, -0x2004($at)\n\t"
        "lui $ra, %%hi(D_800A3E30)\n\t"
        "lw $ra, %%lo(D_800A3E30)($ra)\n\t"
        "nop\n\t"
        ".set\treorder\n\t"
        :
        : "r"(dst)
        : "$1", "$4", "$5", "$6", "$7", "$8", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
