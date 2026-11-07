typedef short s16;
typedef int s32;
typedef unsigned int u32;
extern int D_8009FA0C;
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
int func_80049F4C(s16 *p)
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
    if (D_8009FA0C == 1) offset = 0xa00000;
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
