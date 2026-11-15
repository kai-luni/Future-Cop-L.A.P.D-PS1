// Categories: Utilities > Mathematics > Reciprocal | Graphics > Geometry
/* v2: the retail routine is hand-written assembly; emit it as a file-scope asm block so
 * the compiler adds no prologue/epilogue of its own.  Every instruction is transcribed
 * from the oracle, delay slots included. */
/**
 * @brief Table-driven reciprocal of a 16.16 value, using the GTE's leading-zero
 *        counter for normalisation.
 *
 * @param (implicit) $a0 the value.
 * @return In $v0: the normalised table result shifted back, or 0 when the input
 *         is 0 (LZCR == 0x20).
 *
 * @par Confirmed mechanics
 * mtc2 $4,$30 latches the value into LZCS, mfc2 $2,$31 reads LZCR. From LZCR the
 * function derives an exponent (0x13 - (LZCR & ~1)) and halves it, shifts the
 * mantissa to a 0x40-based index, loads a half-word from the table at
 * 0x8009DF20, then shifts the entry left or right by the exponent - the classic
 * Sony "rsqrt/reciprocal by table" pattern.
 *
 * @par Likely role
 * The perspective-correct interpolation divisor: the table sits next to the
 * GTE, and thirteen call sites (func_8001A314, func_8001A7AC, func_800336DC,
 * func_80036624, ...) are all in the transform code.
 *
 * @par Unknown
 * The exact table content at 0x8009DF20 (not decoded here).
 */
__asm__(
    ".text\n\t"
    ".set\tnoreorder\n\t"
    ".set\tnoat\n\t"
    ".globl\tfunc_80095900\n\t"
    ".ent\tfunc_80095900\n"
    "func_80095900:\n\t"
    "mtc2\t$4, $30\n\t"
    "nop\n\t"
    "nop\n\t"
    "mfc2\t$2, $31\n\t"
    "addiu\t$1, $0, 0x20\n\t"
    "beq\t$2, $1, 1f\n\t"
    "nop\n\t"
    "andi\t$8, $2, 0x1\n\t"
    "addiu\t$1, $0, -0x2\n\t"
    "and\t$10, $2, $1\n\t"
    "addiu\t$9, $0, 0x13\n\t"
    "sub\t$9, $9, $10\n\t"
    "sra\t$9, $9, 1\n\t"
    "addi\t$11, $10, -0x18\n\t"
    "bltz\t$11, 2f\n\t"
    "nop\n\t"
    "sllv\t$12, $4, $11\n\t"
    "b\t3f\n\t"
    "2:\taddiu\t$11, $0, 0x18\n\t"
    "sub\t$11, $11, $10\n\t"
    "srav\t$12, $4, $11\n\t"
    "3:\taddi\t$12, $12, -0x40\n\t"
    "sll\t$12, $12, 1\n\t"
    "lui\t$13, 0x800A\n\t"
    "addu\t$13, $13, $12\n\t"
    "lh\t$13, -0x20E0($13)\n\t"
    "nop\n\t"
    "bltz\t$9, 4f\n\t"
    "nop\n\t"
    "sllv\t$2, $13, $9\n\t"
    "jr\t$ra\n\t"
    "nop\n\t"
    "4:\tneg\t$9, $9\n\t"
    "jr\t$ra\n\t"
    "srlv\t$2, $13, $9\n"
    "1:\tjr\t$ra\n\t"
    "addiu\t$2, $0, 0x0\n\t"
    ".set\tat\n\t"
    ".set\treorder\n\t"
    ".end\tfunc_80095900\n"
);
