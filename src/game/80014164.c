// Categories: Graphics > GTE > Matrix Composition | Graphics > Camera
typedef short s16;
typedef int s32;
struct Input { s16 x, y, z; char pad[6]; s16 a, b; };
struct Matrix { char data[0x24]; s32 x, y, z; };
extern void gBuildRotationMatrixVariant(s32, s32, s32, struct Matrix *) __asm__("func_80018DC0");
extern void gGteLoadRotation(struct Matrix *) __asm__("func_80010820");
void gComposeTransform(struct Input *input, struct Matrix *source, struct Matrix *dest, struct Matrix *parent) __asm__("func_80014164");

extern void gGteTransformWords(struct Matrix *, struct Matrix *, s32) __asm__("func_8001006C");
/**
 * @brief Composes a transform: local rotation, then the parent matrix.
 *
 * @param input Rotation source: two half-words at +0xE/+0x10 and three
 *        coordinates.
 * @param source Four vertices to transform.
 * @param dest Output matrix block.
 * @param parent Parent matrix applied afterwards.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80018DC0(input->a, input->b, 0, &first) builds the local rotation;
 * func_80010820(&first) loads it and func_8001006C(source, &second, 4) transforms
 * four vertices; then the parent matrix is loaded with func_80010820(parent) and
 * func_8001006C(&second, dest, 4) transforms the intermediate result into @c dest.
 * The three coordinates of source/dest/parent are copied through.
 *
 * @par Likely role
 * The hierarchical transform of an attached object: own rotation first, parent
 * (camera or vehicle) second - the order that makes a turret follow its hull.
 *
 * @par Environment path
 * Composes a part transform with its owner's matrix. Its only caller in the
 * US binary is the light setup func_8001428C.
 */
void gComposeTransform(struct Input *input, struct Matrix *source, struct Matrix *dest, struct Matrix *parent)
{
    struct Matrix first;
    struct Matrix second;
    gBuildRotationMatrixVariant(input->a, input->b, 0, &first);
    first.x = input->x << 4;
    first.y = input->y << 4;
    first.z = input->z << 4;
    gGteLoadRotation(&first);
    gGteTransformWords(source, &second, 4);
    second.x += first.x;
    second.y += first.y;
    second.z += first.z;
    gGteLoadRotation(parent);
    gGteTransformWords(&second, dest, 4);
    dest->x += parent->x;
    dest->y += parent->y;
    dest->z += parent->z;
}
