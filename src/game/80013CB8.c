// Categories: Graphics > Scene Graph > Node Transform | Graphics > Animation | Graphics > GTE
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

struct Matrix {
    char data[0x24];
    s32 x;
    s32 y;
    s32 z;
};

struct Input {
    s16 pad0[3];
    u8 pad6;
    u8 flags;
    s16 x;
    s16 y;
    s16 z;
    s16 rx;
    s16 ry;
    s16 rz;
};

extern struct Matrix gBoneMatrices[] __asm__("D_800A0300");
extern void func_80018DC0(s32, s32, s32, struct Matrix *);
extern void func_80010820(struct Matrix *);
extern void func_8001006C(struct Matrix *, struct Matrix *, s32);

/**
 * @brief Composes the transform of node arg0 in the matrix stack
 *        D_800A0300 from an input record and its parent slot.
 *
 * @param arg0 Node index; the parent is slot arg0 - 1.
 * @param input Rotation/translation record with per-axis source flags.
 * @param table Animation sample table.
 * @param offset Base index into the table.
 * @return Nothing.
 *
 * @par Confirmed
 * Flags bit 4/2/1 select rx/ry/rz directly, otherwise the value is
 * table[offset + (u16)field]; func_80018DC0(rx, ry, rz, &matrix) builds the
 * rotation. Flags bit 0x20/0x10/0x08 select x/y/z directly, otherwise a
 * table sample; each translation is shifted left by 4.
 * func_80010820(parent) loads the parent matrix into the GTE,
 * func_8001006C(&matrix, dest, 4) composes, then dest->x/y/z += parent->x/y/z.
 *
 * @par Likely role
 * Local-to-parent composition step of the scene graph; the table is an
 * animation track and the offset selects the current sample.
 *
 * @par Callers
 * func_80013F08, func_8001428C.
 */
void func_80013CB8(s32 arg0, struct Input *input, s16 *table, s32 offset)
{
    struct Matrix matrix;
    struct Matrix *dest;
    struct Matrix *parent;
    s32 rx, ry, rz;
    u8 flags;

    flags = input->flags;

    if (flags & 4) {
        rx = input->rx;
    } else {
        rx = table[offset + (u16)input->rx];
    }

    if (flags & 2) {
        ry = input->ry;
    } else {
        ry = table[offset + (u16)input->ry];
    }

    if (flags & 1) {
        rz = input->rz;
    } else {
        rz = table[offset + (u16)input->rz];
    }

    func_80018DC0(rx, ry, rz, &matrix);

    if (flags & 0x20) {
        matrix.x = input->x << 4;
    } else {
        matrix.x = table[offset + (u16)input->x] << 4;
    }

    if (flags & 0x10) {
        matrix.y = input->y << 4;
    } else {
        matrix.y = table[offset + (u16)input->y] << 4;
    }

    if (flags & 0x08) {
        matrix.z = input->z << 4;
    } else {
        matrix.z = table[offset + (u16)input->z] << 4;
    }

    dest = &gBoneMatrices[arg0];
    parent = &gBoneMatrices[arg0 - 1];

    func_80010820(parent);
    func_8001006C(&matrix, dest, 4);

    dest->x += parent->x;
    dest->y += parent->y;
    dest->z += parent->z;
}
