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
extern void gBuildRotationMatrixVariant(s32, s32, s32, struct Matrix *) __asm__("func_80018DC0");
extern void gGteLoadRotation(struct Matrix *) __asm__("func_80010820");
extern void gGteTransformWords(struct Matrix *, struct Matrix *, s32) __asm__("func_8001006C");
void gComposeBoneMatrix(s32 arg0, struct Input *input, s16 *table, s32 offset) __asm__("func_80013CB8");


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
 *
 * @par Environment path
 * Builds the local transform of one hierarchy level and composes it into the
 * bone matrix array D_800A0300: each of the three angles and three
 * translations comes either literally from the input record or from the
 * animation table at offset+field, depending on the flags byte. func_80018DC0
 * builds the rotation, func_80010820 loads the parent slot (index arg0 - 1),
 * func_8001006C concatenates into slot arg0 and the parent translation is
 * added. Its only callers are the hierarchical projection loops of
 * func_80013F08 and func_8001428C, which load the resulting slot into the GTE
 * and transform that part's vertices.
 */
void gComposeBoneMatrix(s32 arg0, struct Input *input, s16 *table, s32 offset)
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

    gBuildRotationMatrixVariant(rx, ry, rz, &matrix);

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

    gGteLoadRotation(parent);
    gGteTransformWords(&matrix, dest, 4);

    dest->x += parent->x;
    dest->y += parent->y;
    dest->z += parent->z;
}
