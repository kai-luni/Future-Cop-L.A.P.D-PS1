#include "game/types.h"

extern u32 D_8008DCD4[];
extern s32 D_800A3E30;
extern void func_8008C9A0(void);
extern void func_8008DBA0(void);
extern void func_8008DCE8(void);

void func_8008DD7C(void)
{
    register u32 value __asm__("$8");
    register u32 *src __asm__("$10");
    register u32 *end __asm__("$9");
    register u32 *base __asm__("$2");

    __asm__ volatile(
        "lui $at, %%hi(D_800A3E30)\n\t"
        "sw $ra, %%lo(D_800A3E30)($at)\n\t"
        "jal func_8008C9A0\n\t"
        "addiu $t1, $zero, 0x57\n\t"
        "addiu $t2, $zero, 0xB0\n\t"
        "jalr $t2\n\t"
        "nop\n\t"
        "lw $v0, 0x16C($v0)\n\t"
        "nop\n\t"
        "lw $v1, 0x9C8($v0)\n\t"
        "lui $t2, %%hi(D_8008DCD4)\n\t"
        "addiu $t2, $t2, %%lo(D_8008DCD4)\n\t"
        "lui $t1, %%hi(func_8008DCE8)\n\t"
        "addiu $t1, $t1, %%lo(func_8008DCE8)\n\t"
        : "=r"(base), "=r"(src), "=r"(end)
        :
        : "$1", "$3", "$4", "$5", "$6", "$7", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");

    do {
        value = *src;
        *(u32 *)((u8 *)base + 0x9C8) = value;
        __asm__ volatile("" : "+r"(src) : : "memory");
        src++;
        base++;
    } while (src != end);

    __asm__ volatile(
        "jal func_8008DBA0\n\t"
        "lui $ra, %%hi(D_800A3E30)\n\t"
        "lw $ra, %%lo(D_800A3E30)($ra)\n\t"
        "nop\n\t"
        :
        :
        : "$1", "$4", "$5", "$6", "$7", "$11", "$12", "$13", "$14", "$15", "$24", "$25", "memory");
}
