// Categories: Graphics > Scene Graph > Object Resolution | Graphics > GTE
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;
typedef struct Matrix { char rot[0x24]; s32 x, y, z; } Matrix;
typedef struct Parent { char pad[4]; Matrix matrix; } Parent;
extern s32 D_800A10B4[];
extern void gGteLoadRotation(Matrix *) __asm__("func_80010820");
extern void gGteTransformWords(void *, void *, s32) __asm__("func_8001006C");

/**
 * @brief Resolves an object's transform: parent chain, flags and the shared
 *        scratch matrix.
 *
 * @param arg0 Primary parent (or NULL).
 * @param flags Mode bits; 0x100 requests the parent transform.
 * @param arg2 Byte that decides whether the shared matrix is used.
 * @param arg3 Scalar used in the same decision.
 * @param arg4 Secondary parent.
 * @param arg5 Caller-provided matrix block.
 * @return A matrix: the caller's block, the resolved one, or the shared scratch
 *         matrix at D_800A10B4 - 0x24.
 *
 * @par Confirmed
 * With a parent, func_80010820(&arg0->matrix) loads its matrix and
 * func_8001006C(&arg5->x, D_800A10B4, 1) / func_8001006C(arg5, D_800A10B4 - 0x24,
 * 3) transform the translation and the axis vectors; then bit 0x100 pulls in
 * arg4's matrix. The two early returns both hand back the shared scratch block.
 *
 * @par Likely role
 * The transform resolver of the node walkers (func_8006E988, func_80075A84,
 * func_80073700): "world matrix of this node, given its parents".
 */
Matrix *func_8003D9F8(Parent *arg0, u16 flags, s8 arg2, s32 arg3, Parent *arg4, Matrix *arg5)
{
    register s32 x __asm__("$4");
    register s32 y __asm__("$2");
    register s32 py __asm__("$3");
    register s32 px __asm__("$5");
    if (arg0) {
        if (flags & 0x100) {
            gGteLoadRotation(&arg0->matrix);
            gGteTransformWords(&arg5->x, D_800A10B4, 1);
            x = arg5->x;
        y = arg5->y;
        py = arg0->matrix.y;
        px = arg0->matrix.x;
        __asm__ volatile ("" : "+r" (x), "+r" (y), "+r" (py), "+r" (px));
        D_800A10B4[1] = y + py;
        y = arg5->z;
        py = arg0->matrix.z;
        __asm__ volatile ("" : "+r" (x), "+r" (y), "+r" (py), "+r" (px));
        D_800A10B4[0] = x + px;
        __asm__ volatile ("" : "+r" (y), "+r" (py) : : "memory");
        D_800A10B4[2] = y + py;
            if ((u8)arg3) {
                gGteLoadRotation(&arg0->matrix);
                gGteTransformWords(arg5, (char *)D_800A10B4 - 0x24, 3);
            }
            return (Matrix *)((char *)D_800A10B4 - 0x24);
        }
    } else if ((flags & 0x100) && arg4) {
        gGteLoadRotation(&arg4->matrix);
        gGteTransformWords(&arg5->x, D_800A10B4, 1);
        x = arg5->x;
        y = arg5->y;
        py = arg4->matrix.y;
        px = arg4->matrix.x;
        __asm__ volatile ("" : "+r" (x), "+r" (y), "+r" (py), "+r" (px));
        D_800A10B4[1] = y + py;
        y = arg5->z;
        py = arg4->matrix.z;
        __asm__ volatile ("" : "+r" (x), "+r" (y), "+r" (py), "+r" (px));
        D_800A10B4[0] = x + px;
        __asm__ volatile ("" : "+r" (y), "+r" (py) : : "memory");
        D_800A10B4[2] = y + py;
        if ((u8)arg3) {
            gGteLoadRotation(&arg4->matrix);
            gGteTransformWords(arg5, (char *)D_800A10B4 - 0x24, 3);
        }
        return (Matrix *)((char *)D_800A10B4 - 0x24);
    }
    return arg5;
}
