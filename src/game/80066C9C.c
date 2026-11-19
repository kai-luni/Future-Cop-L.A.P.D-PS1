// Categories: Graphics > Camera > Position Update | Graphics > HUD > Minimap
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

extern struct Camera *gCamera __asm__("D_8009F9E4");
extern struct Pair gViewGroundAnchors[] __asm__("D_800A3190");
extern struct Block gCoverageGrid[] __asm__("D_800AB3B8");
extern u8 gGridOriginX[] __asm__("D_8009F834");

extern void gSelectViewRecord(s32 id) __asm__("func_80017B60");
extern void gGteLoadRotation(u8 *matrix) __asm__("func_80010820");
extern void gSampleCellWindow(s32 a0, s32 a1, struct Block *block) __asm__("func_80064460");

#define FMUL(a, b) ({ s32 r_; __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define FMULU(a, b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
void gUpdateCameraAnchor(s32 index, struct Entity *obj) __asm__("func_80066C9C");


/**
 * @brief Projects an entity's height against the camera eye and stores the
 *        resulting ground position pair plus two scale bytes for one view.
 *
 * @param index View/player index into D_800A3190 and D_8009F834.
 * @param obj Entity block; +0x54 the signed height, +0xAC a sub-block whose
 *        +0xAC byte selects the draw id.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80017B60(sub->unkAC) selects the draw id, func_80010820 loads the
 * camera matrix of D_8009F9E4. v = |obj->unk54|; the eye difference dy is
 * scaled by the reciprocal of half the camera rate word +0x74 (sign flipped
 * with the rate), FMULU(dy, 0x80000000 / den). The inline block then writes
 * D_800A3190[index].first = cam[0x84] - FMUL(a3, cam[0x78]) and
 * .second = cam[0x7C] - FMUL(a3, cam[0x70]), stores (first >> 17) - 2 and
 * (second >> 17) - 2 as the two bytes at D_8009F834[index * 2], and calls
 * func_80064460 with a 0x190-byte block of D_800AB3B8 at a derived offset.
 *
 * @par Likely role
 * The per-view camera anchor update: D_800A3190 is the position pair table
 * that func_80069580 reads back, so this feeds the scene traversal and the
 * minimap with the current ground position; the two bytes are a coarse
 * 17-bit-shifted scale for the map window.
 *
 * @par Unknown
 * The meaning of camera words +0x70/+0x74/+0x78/+0x7C/+0x84 and the
 * -2 bias on the scale bytes.
 *
 * @par Callers
 * func_80026E5C.
 *
 * @par Environment path
 * Per-view look-at update: it selects the player's view record with
 * gSelectViewRecord, loads the camera rotation, and intersects the camera's
 * view direction (gCamera + 0x70/+0x74/+0x78) from the eye (+0x7C/+0x84) with
 * the entity's height, writing the resulting ground point to
 * gViewGroundAnchors[index]. From that point it derives the coarse grid
 * origin ((coordinate >> 17) - 2) for both axes, stores the two bytes at
 * D_8009F834[index * 2], and fills the view's 400-byte block of gCoverageGrid
 * through gSampleCellWindow (func_80064460). It runs once per player and
 * frame through the scratchpad trampoline func_80026E5C, which both
 * func_800271A4 and gRunSessionFrame (func_80027510) call.
 */
void gUpdateCameraAnchor(s32 index, struct Entity *obj)
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

    gSelectViewRecord(obj->unkAC->unkAC);
    gGteLoadRotation(gCamera->matrix);

    t = obj->unk54;
    d = -t;
    v = ((d >> 31) & d) + t;
    cam = gCamera;
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
