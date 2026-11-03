#include "game/types.h"

extern u32 D_800A3E10;
extern u32 jtbl_800A3E18_main;
extern u32 jtbl_800A3E1C_main;
extern void func_8008C9A0(void);
extern void func_8008DBA0(void);

asm(".set jtbl_800A3E18_main, 0x800A3E18");
asm(".set jtbl_800A3E1C_main, 0x800A3E1C");

void func_8008DB28(void)
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
