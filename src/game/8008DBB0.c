#include "game/types.h"

extern void func_8008C9A0(void);
extern void func_8008C9B0(void);
extern void func_8008DBA0(void);

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
