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

extern void func_80018DC0(s32, s32, s32, struct Matrix *);
extern void func_80010820(struct Matrix *);
extern void func_8001006C(struct Matrix *, struct Matrix *, s32);

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

        func_80018DC0(rx, ry, rz, &matrix);

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

        func_80010820(parent);
        func_8001006C(&matrix, dest, 4);

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
