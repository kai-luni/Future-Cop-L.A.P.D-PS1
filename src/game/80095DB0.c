#include "game/types.h"

extern s32 D_800A63D0;
extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008DBA0(void);

asm(".set D_80095E5C, 0x80095E5C");
asm(".set D_80095E74, 0x80095E74");
asm(".set D_80095E8C, 0x80095E8C");

void func_80095DB0(void)
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
