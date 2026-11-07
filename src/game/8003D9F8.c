typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;
typedef struct Matrix { char rot[0x24]; s32 x, y, z; } Matrix;
typedef struct Parent { char pad[4]; Matrix matrix; } Parent;
extern s32 D_800A10B4[];
extern void func_80010820(Matrix *);
extern void func_8001006C(void *, void *, s32);

Matrix *func_8003D9F8(Parent *arg0, u16 flags, s8 arg2, s32 arg3, Parent *arg4, Matrix *arg5)
{
    register s32 x __asm__("$4");
    register s32 y __asm__("$2");
    register s32 py __asm__("$3");
    register s32 px __asm__("$5");
    if (arg0) {
        if (flags & 0x100) {
            func_80010820(&arg0->matrix);
            func_8001006C(&arg5->x, D_800A10B4, 1);
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
                func_80010820(&arg0->matrix);
                func_8001006C(arg5, (char *)D_800A10B4 - 0x24, 3);
            }
            return (Matrix *)((char *)D_800A10B4 - 0x24);
        }
    } else if ((flags & 0x100) && arg4) {
        func_80010820(&arg4->matrix);
        func_8001006C(&arg5->x, D_800A10B4, 1);
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
            func_80010820(&arg4->matrix);
            func_8001006C(arg5, (char *)D_800A10B4 - 0x24, 3);
        }
        return (Matrix *)((char *)D_800A10B4 - 0x24);
    }
    return arg5;
}
