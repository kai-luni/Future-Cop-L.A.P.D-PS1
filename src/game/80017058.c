// Categories: Graphics > Model Renderer > Draw Path | Graphics > GTE
#include "game/types.h"

typedef struct { s32 x,y,z; } Vec;
typedef struct { s32 x,y,z; s32 extra; } Vec4;
typedef struct { s32 m[3][3]; } Matrix;
typedef struct { u32 flags; u32 a,b; } Part;
typedef struct { s32 count; Part part[1]; } Parts;
typedef struct { char pad[12]; u32 flags; char pad10[20]; Parts *parts; void *p28; void **p2c; } Model;
typedef struct { Model *model; Matrix matrix; Vec pos; char pad34[20]; s16 index; u8 flags; } Instance;
typedef struct { char data[20]; u8 mode, flag; u16 value; } Work;
extern u16 gDepthTagWord __asm__("D_8009F580");
extern void *gVertexSourceAltPtr __asm__("D_8009F574");
extern void *gVertexSourcePtr __asm__("D_8009F570");
extern Vec *gCamera __asm__("D_8009F9E4");
extern s32 gEnvMapTexpageWord __asm__("D_800A8EA4");
extern void func_80019470(Matrix *, Matrix *);
extern void gGteLoadRotation(Matrix *) __asm__("func_80010820");
extern void gGteTransformWords(Vec *, Vec *, s32) __asm__("func_8001006C");
extern s32 gPrepareModelPart(Part *, Work *, void *, s32) __asm__("func_80015B68");
extern void func_80016E6C(Part *, Work *, void *, void *);
extern void func_80015F38(Part *, Work *, void *, void *);
/**
 * @brief Variant of func_80016A64 with an additional per-part enable bit
 *        array and a fixed draw mode 0.
 *
 * @param inst Model instance.
 * @param enabled Bit array: part count indexes enabled[count / 32] bit
 *        (count & 31).
 * @param mask Part group mask (bit (part->flags >> 12) & 7).
 * @return Nothing.
 *
 * @par Confirmed
 * Identical setup to func_80016A64 (frame index, p2c lookup, billboard
 * path for instance flag 0x40 with the transposed matrix and camera
 * delta), but work.mode = 0 and a part draws only when both the group
 * mask bit and the enable bit are set; the count runs downwards while
 * parts advance forwards.
 *
 * @par Likely role
 * The selective draw path (func_800175A0): the enable array lets the
 * caller hide individual parts (damage states, detachable pieces) on top
 * of the group mask.
 *
 * @par Unknown
 * Why the enable index uses the descending count rather than the part
 * index.
 *
 * @par Callers
 * func_800175A0.
 *
 * @par Environment path
 * Draws the parts of a model instance with the same per-part traversal as
 * func_80016A64, but draws a part only when both the caller's group mask and
 * a per-part enable bit are set, and it forces work.mode to 0. The work block
 * starts from the depth/tag word D_8009F580 and, for instances with flag
 * 0x40, it loads the transposed matrix and the camera-relative vector scaled
 * by the env-map texpage word D_800A8EA4. Each accepted part goes through
 * gPrepareModelPart (func_80015B68) and is then emitted by func_80016E6C or
 * func_80015F38. It is called by func_800175A0 after func_8001428C
 * (src/game/800175A0.c:28), that is, it is the part-submission stage of the
 * actor renderer, not the region pass.
 */
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
    work.value = gDepthTagWord;
    other = 0;
    index = (s32)other;
    model = inst->model;
    global = gVertexSourcePtr;
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
        other = gVertexSourceAltPtr;
        func_80019470(&inst->matrix, &transposed);
        gGteLoadRotation(&transposed);
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
            p = gCamera; iy = inst->pos.y; ix = inst->pos.x; py = p->y; px = p->x;
            __asm__("" : "+r" (p), "+r" (iy), "+r" (ix), "+r" (py), "+r" (px));
            vec.y = py - iy;
            __asm__("" : : "m" (vec.y));
            {
                s32 pz = p->z;
                s32 iz = inst->pos.z;
                vec.x = px - ix;
                vec.z = pz - iz;
            }
            gGteTransformWords(a, b, one);
        }
        gGteLoadRotation(&inst->matrix);
        vec.extra = gEnvMapTexpageWord;
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
            if (gPrepareModelPart(part, &work, p2c, flag)) {
                if ((part->flags >> 15) & 1) func_80016E6C(part, &work, global, other);
                func_80015F38(part, &work, p28, global);
            }
        }
        count--;
        part++;
    }
}
