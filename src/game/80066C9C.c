#include "game/types.h"

struct Camera {
    u8 unk00[0x28];
    u8 matrix[0x48];
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 eyeX;
    s32 eyeY;
    s32 eyeZ;
};

struct Sub {
    u8 unk00[0xAC];
    s8 unkAC;
};

struct Entity {
    u8 unk00[0x54];
    s32 unk54;
    u8 unk58[0x54];
    struct Sub *unkAC;
};

struct Pair {
    s32 first;
    s32 second;
};

struct Block {
    u8 unk00[0x190];
};

extern struct Camera *D_8009F9E4;
extern struct Pair D_800A3190[];
extern struct Block D_800AB3B8[];
extern u8 D_8009F834[];

extern void func_80017B60(s32 id);
extern void func_80010820(u8 *matrix);
extern void func_80064460(s32 a0, s32 a1, struct Block *block);

#define FMUL(a, b) ({ s32 r_; __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define FMULU(a, b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })

void func_80066C9C(s32 index, struct Entity *obj)
{
    struct Camera *cam;
    s32 t;
    s32 d;
    s32 v;
    s32 dy;
    s32 rate;
    s32 den;
    s32 a3;
    s32 first;
    s32 second;
    register s32 s1 __asm__("$17");
    register s32 s2 __asm__("$18");
    s32 m;

    func_80017B60(obj->unkAC->unkAC);
    func_80010820(D_8009F9E4->matrix);

    t = obj->unk54;
    d = -t;
    v = ((d >> 31) & d) + t;
    cam = D_8009F9E4;
    dy = cam->eyeY - v;

    if (dy > 0) {
        rate = cam->unk74;
        if (rate <= 0) {
            den = -rate;
            den >>= 1;

#line 901
            a3 = -FMULU(dy, 0x80000000u / (u32)den);
        } else {
            den = rate >> 1;

#line 900
            a3 = FMULU(dy, 0x80000000u / (u32)den);
        }
    } else {
        dy = v - cam->eyeY;
        rate = cam->unk74;
        if (rate > 0) {
            den = rate >> 1;

#line 901
            a3 = -FMULU(dy, 0x80000000u / (u32)den);
        } else {
            den = -rate;
            den >>= 1;

#line 900
            a3 = FMULU(dy, 0x80000000u / (u32)den);
        }
    }

    __asm__ volatile(
        ".set noreorder\n\t"
        "lui $3,%%hi(D_800A3190)\n\t"
        "addiu $3,$3,%%lo(D_800A3190)\n\t"
        "sll $5,%0,3\n\t"
        "lui $4,%%hi(D_8009F9E4)\n\t"
        "lw $4,%%lo(D_8009F9E4)($4)\n\t"
        "addu $5,$5,$3\n\t"
        "lw $2,0x78($4)\n\t"
        "nop\n\t"
        "mult %1,$2\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $9,$9,16\n\t"
        "srl $8,$8,16\n\t"
        "or $6,$9,$8\n\t"
        "lw $2,0x84($4)\n\t"
        "sll $16,%0,1\n\t"
        "subu $2,$2,$6\n\t"
        "sw $2,0($5)\n\t"
        "addu $17,$2,$0\n\t"
        "sra $17,$17,17\n\t"
        "lw $2,0x70($4)\n\t"
        "nop\n\t"
        "mult %1,$2\n\t"
        "mflo $8\n\t"
        "mfhi $9\n\t"
        "sll $9,$9,16\n\t"
        "srl $8,$8,16\n\t"
        "or $2,$9,$8\n\t"
        "addiu $17,$17,-2\n\t"
        "lw $3,0x7c($4)\n\t"
        "addu $4,$17,$0\n\t"
        "addu $6,$16,%0\n\t"
        "sll $6,$6,3\n\t"
        "addu $6,$6,%0\n\t"
        "sll $6,$6,4\n\t"
        "subu $3,$3,$2\n\t"
        "sra $18,$3,17\n\t"
        "addiu $18,$18,-2\n\t"
        "lui $2,%%hi(D_800AB3B8)\n\t"
        "addiu $2,$2,%%lo(D_800AB3B8)\n\t"
        "sw $3,4($5)\n\t"
        "addu $5,$18,$0\n\t"
        ".word 0x0c019118\n\t"
        "addu $6,$6,$2\n\t"
        "lui $2,%%hi(D_8009F834)\n\t"
        "addiu $2,$2,%%lo(D_8009F834)\n\t"
        "addu $16,$16,$2\n\t"
        "sb $17,0($16)\n\t"
        "sb $18,1($16)\n\t"
        ""
        : : "r"(index), "r"(a3) : "$2", "$3", "$4", "$5", "$6", "$8", "$9", "$16", "$17", "$18", "$31", "memory");
}
