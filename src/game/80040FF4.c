#include "game/types.h"

void func_80040FF4(void);

void func_80040FF4(void)
{
    __asm__ volatile(
        ".set\tnoreorder\n\t"
        "lbu $v0,0x45($s1)\n\t"
        "nop\n\t"
        "sb $v0,0x8B($s0)\n\t"
        "lbu $v0,0x46($s1)\n\t"
        "nop\n\t"
        "sb $v0,0x8C($s0)\n\t"
        "lbu $v0,0x47($s1)\n\t"
        "nop\n\t"
        "sb $v0,0x8D($s0)\n\t"
        "lhu $v0,0x48($s1)\n\t"
        "nop\n\t"
        "sh $v0,0x94($s0)\n\t"
        "lw $v0,0x3C($s1)\n\t"
        "nop\n\t"
        "sltu $v0,$zero,$v0\n\t"
        "sb $v0,0x68($s0)\n\t"
        "lw $v0,0x104($gp)\n\t"
        "addiu $v1,$zero,0x1\n\t"
        "sh $zero,0x8($s0)\n\t"
        "sh $v1,0xA($s0)\n\t"
        "addiu $v0,$v0,0x1\n\t"
        "sw $v0,0x104($gp)\n\t"
        "jal func_800403D0\n\t"
        "addu $a0,$s0,$zero\n\t"
        "addu $v0,$s0,$zero\n\t"
        "lw $ra,0x40($sp)\n\t"
        "lw $s1,0x3C($sp)\n\t"
        "lw $s0,0x38($sp)\n\t"
        "addiu $sp,$sp,0x48\n\t"
        ".set\treorder\n\t");
}
