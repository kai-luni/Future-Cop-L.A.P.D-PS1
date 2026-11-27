// Categories: Graphics > GTE > Depth Sort | Platform > GTE
/* Handwritten function */
/**
 * @brief AVSZ3 - averages the Z values of three vertices into OTZ (register 7).
 *
 * @param (implicit) $a0/$a1/$a2 packed XY words of the three vertices; the Z
 *        components are read from the already loaded registers $17/$18/$19
 *        (SZ1, SZ2, SZ3).
 * @return In $v0: OTZ, the ordering-table depth used to bucket the primitive.
 *
 * @par Confirmed mechanics
 * mtc2 $a0,$17 / $a1,$18 / $a2,$19 load the three screen Z values, two nops let
 * the pipeline settle, then the fixed-point command 0x4B58002D (= AVSZ3 with
 * sf = 1) runs and `mfc2 $v0,$7` reads OTZ back.
 *
 * @par Callers
 * None in this executable - the unit is referenced only through the GTE
 * dispatch table, i.e. the caller was inlined into the renderer.
 *
 * @par Sibling
 * func_80095B40 is the four-vertex form (AVSZ4).
 */
void gGteAverageVertexZ3(void) __asm__("func_80095B20");

__asm__(
    ".set noreorder\n"
    ".globl func_80095B20\n"
    ".ent func_80095B20\n"
    "func_80095B20:\n"
    "mtc2 $a0, $17\n"
    "mtc2 $a1, $18\n"
    "mtc2 $a2, $19\n"
    "nop\n"
    ".word 0x4B58002D\n"
    "mfc2 $v0, $7\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095B20\n"
    ".set reorder\n"
);
