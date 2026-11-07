#include "game/types.h"

typedef struct { s32 x,y,z; } Vec;
typedef struct { s32 x,y,z; s32 extra; } Vec4;
typedef struct { s32 m[3][3]; } Matrix;
typedef struct { u32 flags; u32 a,b; } Part;
typedef struct { s32 count; Part part[1]; } Parts;
typedef struct { char pad[12]; u32 flags; char pad10[20]; Parts *parts; void *p28; void **p2c; } Model;
typedef struct { Model *model; Matrix matrix; Vec pos; char pad34[20]; s16 index; u8 flags; } Instance;
typedef struct { char data[20]; u8 mode, flag; u16 value; } Work;
extern u16 D_8009F580;
extern void *D_8009F570, *D_8009F574;
extern Vec *D_8009F9E4;
extern s32 D_800A8EA4;
extern void func_80019470(Matrix *, Matrix *);
extern void func_80010820(Matrix *);
extern void func_8001006C(Vec *, Vec *, s32);
extern s32 func_80015B68(Part *, Work *, void *, s32);
extern void func_80016E6C(Part *, Work *, void *, void *);
extern void func_80015F38(Part *, Work *, void *, void *);
void func_80017058(Instance *inst, u32 *enabled, s32 mask)
{
    Matrix transposed;
    Work work;
    Vec4 vec;
    Model *model;
    Part *part;
    void *p28, *p2c, *global, *other;
    s32 count, flag, bit;
    register s32 index __asm__("$16");
    work.value = D_8009F580;
    other = 0;
    index = (s32)other;
    model = inst->model;
    global = D_8009F570;
    part = model->parts->part;
    p28 = model->p28;
    if (!((model->flags >> 25) & 1)) index = inst->index;
    {
        register s32 offset __asm__("$4") = index << 2;
        register void **table __asm__("$2") = model->p2c;
        register u32 flags __asm__("$3") = inst->flags;
        register void **address __asm__("$4");
        __asm__("" : "+r" (offset), "+r" (table), "+r" (flags));
        offset += (s32)table;
        address = (void **)offset;
        __asm__("" : "+r" (address));
        p2c = address[1];
        flag = !(flags & 8) << 8;
        if (flags & 0x40) {
        other = D_8009F574;
        func_80019470(&inst->matrix, &transposed);
        func_80010820(&transposed);
        {
            register Vec *a __asm__("$4") = (Vec *)&vec;
            register Vec *b __asm__("$5") = a;
            register s32 one __asm__("$6") = 1;
            register Vec *p __asm__("$3");
            register s32 iy __asm__("$7");
            register s32 ix __asm__("$9");
            register s32 py __asm__("$2");
            register s32 px __asm__("$8");
            __asm__("" : "+r" (a), "+r" (b), "+r" (one));
            p = D_8009F9E4; iy = inst->pos.y; ix = inst->pos.x; py = p->y; px = p->x;
            __asm__("" : "+r" (p), "+r" (iy), "+r" (ix), "+r" (py), "+r" (px));
            vec.y = py - iy;
            __asm__("" : : "m" (vec.y));
            {
                s32 pz = p->z;
                s32 iz = inst->pos.z;
                vec.x = px - ix;
                vec.z = pz - iz;
            }
            func_8001006C(a, b, one);
        }
        func_80010820(&inst->matrix);
        vec.extra = D_800A8EA4;
        {
            s32 x = vec.x;
            s32 y = vec.y;
            u32 f = model->flags;
            vec.x = x >> 2;
            vec.y = y >> 2;
            work.flag = (f >> 28) & 2;
            vec.z >>= 2;
        }
        __asm__("" : : "m" (vec.extra));
    }
    }
    work.mode = 0;
    count = model->parts->count;
    while (count > 0) {
        bit = 1 << ((part->flags >> 12) & 7);
        __asm__("" : "+r" (bit));
        if ((mask & bit) && (enabled[count / 32] & (1 << (count & 31)))) {
            if (func_80015B68(part, &work, p2c, flag)) {
                if ((part->flags >> 15) & 1) func_80016E6C(part, &work, global, other);
                func_80015F38(part, &work, p28, global);
            }
        }
        count--;
        part++;
    }
}
