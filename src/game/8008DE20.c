#include "game/types.h"

extern u32 D_8008DE90[];
extern u32 D_8008DE9C[];
extern s32 D_800A3E40;
extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008DBA0(void);

void func_8008DE20(void)
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
