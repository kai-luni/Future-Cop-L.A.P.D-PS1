// Categories: Graphics > GTE > Depth Cueing | Graphics > Lighting
/* Handwritten function */
/**
 * @brief DPCT: depth-cues three colours at once (triple vector form).
 *
 * @param (implicit) $a0/$a1/$a2 pointers to the three RGB words, loaded into
 *        RGB0/RGB1/RGB2 ($20/$21/$22); $a2 is reused as RGBC ($6, the colour
 *        that is multiplied in) and $a3 is the IR0 weight ($8). Three further
 *        pointers arrive on the stack at 0x10/0x14/0x18 and receive the three
 *        results.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Three lwc2 + lwc2 $6 + mtc2 $8, one nop, `0x4AF8002A` = DPCT (real opcode
 * 2Ah, sf = 1), then the three stack pointers are used to store RGB0/RGB1/RGB2
 * back with swc2.
 *
 * @par Callers
 * None in the symbol table (the callers are inlined in the polygon shading
 * path).
 */
void gGteShadeDpct(void) __asm__("func_80095A70");

__asm__(
    ".set noreorder\n"
    ".globl func_80095A70\n"
    ".ent func_80095A70\n"
    "func_80095A70:\n"
    "lwc2 $20, 0x0($a0)\n"
    "lwc2 $21, 0x0($a1)\n"
    "lwc2 $22, 0x0($a2)\n"
    "lwc2 $6, 0x0($a2)\n"
    "mtc2 $a3, $8\n"
    "nop\n"
    ".word 0x4AF8002A\n"
    "lw $t0, 0x10($sp)\n"
    "lw $t1, 0x14($sp)\n"
    "lw $t2, 0x18($sp)\n"
    "swc2 $20, 0x0($t0)\n"
    "swc2 $21, 0x0($t1)\n"
    "swc2 $22, 0x0($t2)\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095A70\n"
    ".set reorder\n"
);
