// Categories: Graphics > Renderer > Projection | Utilities > Mathematics
typedef short s16;
typedef int s32;
typedef unsigned int u32;
extern int gPlayerCount __asm__("D_8009FA0C");
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
int gProjectWorldPointAndExtent(s16 *p) __asm__("func_80049F4C");

/**
 * @brief Projects a world point (and its extent) to screen coordinates with
 *        the software perspective divide.
 *
 * @param p Eight s16 values: [0]/[1] the x/y position, [2] the depth,
 *        [8]/[10] the extent; the results are written back in place.
 * @return 1 when the depth is below 65 (too close, skip), 0 otherwise.
 *
 * @par Confirmed
 * scale = 0x10000000 / depth; x' = FMUL(x << 13, scale), y' likewise. The
 * screen x is (x' + offset) >> 15 with offset 0x500000 (0xA00000 in the
 * narrow mode D_8009FA0C == 1); the screen y is (0x780000 - y') >> 15. The
 * extent words [8]/[10] are projected with the same scale and stored back
 * >> 15.
 *
 * @par Likely role
 * The sprite/culling projection of func_8004A048: the 0x780000 constant is
 * the vertical screen centre in the 13-bit fixed-point space, the two
 * offsets are the half-widths of the 320/160 pixel modes.
 *
 * @par Callers
 * func_8004A048.
 */
int gProjectWorldPointAndExtent(s16 *p)
{
    register int retval __asm__("$2");
    register u32 numerator __asm__("$2");
    register s32 a __asm__("$5");
    register s32 b __asm__("$6");
    register s32 scale __asm__("$7");
    register s32 x __asm__("$10");
    register s32 offset __asm__("$11");
    register s32 y __asm__("$12");
    if (p[2] < 65) return 1;
    offset = 0x500000;
    numerator = 0x10000000U;
    scale = numerator / p[2];
    a = p[0];
    b = p[1];
    a <<= 13;
    b <<= 13;
    FMUL(x, a, scale);
    FMUL(y, b, scale);
    if (gPlayerCount == 1) offset = 0xa00000;
    __asm__ volatile(
        "addu $3,$10,$11\n\t"
        "sra $3,$3,15\n\t"
        "sh $3,0($4)\n\t"
        "lui $3,0x78\n\t"
        "subu $3,$3,$12\n\t"
        "lh $5,8($4)\n\t"
        "lh $6,10($4)\n\t"
        "sra $3,$3,15\n\t"
        "sh $3,2($4)\n\t"
        "sll $5,$5,13\n\t"
        "mult $5,$7\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $9,$9,16\n\t"
        "srl $8,$8,16\n\t"
        "or $5,$9,$8\n\t"
        "sll $6,$6,13\n\t"
        "mult $6,$7\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $9,$9,16\n\t"
        "srl $8,$8,16\n\t"
        "or $6,$9,$8\n\t"
        "sra $3,$5,15\n\t"
        "sh $3,8($4)\n\t"
        "sra $3,$6,15\n\t"
        "sh $3,10($4)"
        : "=r"(retval) : "r"(p), "r"(x), "r"(offset), "r"(y), "r"(scale), "0"(0)
        : "$3", "$5", "$6", "$8", "$9", "memory");
    return retval;
}
