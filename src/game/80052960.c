// Categories: Graphics > Model Renderer > Animation > Joint Chain | Graphics > GTE > Batch Transform | Graphics > Geometry
#include "game/types.h"

struct Matrix {
    s32 m[12];
};

struct Rec {
    u8 index;
    u8 pad[6];
    u8 flags;
    s16 x;
    s16 y;
    s16 z;
    s16 rx;
    s16 ry;
    s16 rz;
};

struct Sub {
    u8 pad[0x14];
    struct Rec *entries;
    s16 *table;
};

struct Args {
    struct Sub *sub;
    struct Matrix matrix;
    u8 pad[0x14];
    s16 offset;
};

extern void gBuildRotationMatrixVariant(s32, s32, s32, struct Matrix *) __asm__("func_80018DC0");
extern void gGteLoadRotation(struct Matrix *) __asm__("func_80010820");
extern void gGteTransformWords(struct Matrix *, struct Matrix *, s32) __asm__("func_8001006C");

#define COPY_MATRIX(d, s) \
    do { \
        (d)->m[0] = (s)->m[0]; \
        (d)->m[1] = (s)->m[1]; \
        (d)->m[2] = (s)->m[2]; \
        (d)->m[3] = (s)->m[3]; \
        (d)->m[4] = (s)->m[4]; \
        (d)->m[5] = (s)->m[5]; \
        (d)->m[6] = (s)->m[6]; \
        (d)->m[7] = (s)->m[7]; \
        (d)->m[8] = (s)->m[8]; \
        (d)->m[9] = (s)->m[9]; \
        (d)->m[10] = (s)->m[10]; \
        (d)->m[11] = (s)->m[11]; \
    } while (0)

/**
 * @brief Walks a short joint record list and builds the world matrices of a limb
 *        by concatenating rotation and translation per level.
 *
 * @param a0 Description block: +0x00 the sub-block, +0x04 a 3x4 matrix, +0x48 a
 *        16-bit table offset.
 * @param first Receives the matrix of the fifth joint; the parameter name is
 *        misleading, it is written last.
 * @param second Receives the matrix of the third joint.
 * @return Nothing. A chain shorter than the level being written leaves the
 *         corresponding output untouched.
 *
 * @par Confirmed data
 * The sub-block holds the record array at +0x14 (record size 0x14: index byte,
 * six padding bytes, flags byte, then six 16-bit values x, y, z, rx, ry, rz) and
 * the 16-bit value table at +0x18. The record index selects both the destination
 * slot of a four-entry local matrix array and, minus one, the parent slot.
 *
 * @par Confirmed sequence
 * The input matrix is copied into local slot 0. For each record i = 1.. the three
 * angles are taken literally when flags bits 4/2/1 are set and otherwise from
 * table[offset + value]; the same rule applies to the translation with bits
 * 0x20/0x10/0x08. func_80018DC0 builds the 3x3 rotation, the translation is
 * placed at words 9..11 scaled by 16, the parent matrix is loaded into the GTE by
 * func_80010820, and func_8001006C(&matrix, dest, 4) concatenates them - the count
 * of 4 covers the three basis rows plus the translation row. The parent
 * translation is then added to the result words 9..11.
 * Flag 0x80 marks the last record and ends the walk; at level 3 the destination
 * is copied into second, at level 5 into first, which also ends the walk. Note
 * that func_8001006C negates the y component of every transformed triple, so the
 * whole chain lives in the mirrored y convention the model pipeline uses.
 *
 * @par Likely
 * The limb solver behind an animated model: a record chain of up to five joints,
 * with per-joint constants or shared-table values, returning the matrices needed
 * to place an upper and a lower limb segment (third and fifth joint).
 *
 * @par Unknown
 * What the caller does with the two matrices is not visible here, and record
 * index 0 would address the root slot itself, so the records presumably start at
 * index 1; the file does not enforce it.
 *
 * @par Callers
 * func_80052D7C.
 *
 * @par Environment path
 * Walks a short joint record list and builds the world matrices of a limb:
 * for each record the three angles and the translation come literally or from
 * the shared sixteen-bit table depending on the flags byte, func_80018DC0
 * builds the 3x3 rotation, the translation is placed at words 9 to 11 scaled
 * by 16, and func_80010820 plus func_8001006C concatenate the result onto the
 * parent slot selected by the record index before the parent translation is
 * added. Level 3 is copied into the second output, level 5 into the first,
 * and flag 0x80 ends the walk. Its caller is func_80052D7C, which transforms
 * both parts of a two-part model, so this is part geometry inside the model
 * pipeline; the function uses no global data.
 */
void func_80052960(struct Args *a0, struct Matrix *first, struct Matrix *second)
{
    struct Matrix matrix;
    struct Matrix local[4];
    struct Matrix *dest;
    struct Matrix *parent;
    struct Sub *sub;
    struct Rec *rec;
    s16 *table;
    s32 offset;
    s32 i;
    u8 flags;
    s32 rx, ry, rz;
    s32 m0;

    sub = a0->sub;
    m0 = a0->matrix.m[0];
    local[0].m[1] = a0->matrix.m[1];
    local[0].m[2] = a0->matrix.m[2];
    local[0].m[3] = a0->matrix.m[3];
    local[0].m[4] = a0->matrix.m[4];
    local[0].m[5] = a0->matrix.m[5];
    local[0].m[6] = a0->matrix.m[6];
    local[0].m[7] = a0->matrix.m[7];
    local[0].m[8] = a0->matrix.m[8];
    local[0].m[9] = a0->matrix.m[9];
    local[0].m[10] = a0->matrix.m[10];
    local[0].m[11] = a0->matrix.m[11];
    local[0].m[0] = m0;

    rec = sub->entries;
    table = sub->table;
    offset = a0->offset;
    i = 1;

    while (1) {
        u8 index = rec->index;
        flags = rec->flags;

        if (flags & 4) {
            rx = rec->rx;
        } else {
            rx = table[offset + rec->rx];
        }
        if (flags & 2) {
            ry = rec->ry;
        } else {
            ry = table[offset + rec->ry];
        }
        if (flags & 1) {
            rz = rec->rz;
        } else {
            rz = table[offset + rec->rz];
        }

        gBuildRotationMatrixVariant(rx, ry, rz, &matrix);

        if (flags & 0x20) {
            matrix.m[9] = rec->x << 4;
        } else {
            matrix.m[9] = table[offset + rec->x] << 4;
        }
        if (flags & 0x10) {
            matrix.m[10] = rec->y << 4;
        } else {
            matrix.m[10] = table[offset + rec->y] << 4;
        }
        if (flags & 8) {
            matrix.m[11] = rec->z << 4;
        } else {
            matrix.m[11] = table[offset + rec->z] << 4;
        }

        dest = &local[index];
        parent = &local[index - 1];

        gGteLoadRotation(parent);
        gGteTransformWords(&matrix, dest, 4);

        dest->m[9] += parent->m[9];
        dest->m[10] += parent->m[10];
        dest->m[11] += parent->m[11];

        if (rec->flags & 0x80) {
            break;
        }
        if (i == 3) {
            COPY_MATRIX(second, dest);
        }
        if (i == 5) {
            COPY_MATRIX(first, dest);
            break;
        }
        rec++;
        i++;
    }
}
