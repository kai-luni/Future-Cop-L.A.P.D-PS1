// Categories: Graphics > Model Renderer > Instance Submission | Graphics > GTE > Projection | Graphics > Clipping | Graphics > Animation
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Matrix {
    char data[0x24];
    s32 x;
    s32 y;
    s32 z;
};

struct Input {
    u8 matrixIndex;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 pad5;
    u8 pad6;
    u8 flags;
    s16 x;
    s16 y;
    s16 z;
    s16 rx;
    s16 ry;
    s16 rz;
};

struct Model {
    char pad0[12];
    u32 flags;
    char pad10[4];
    struct Input *inputs;
    s16 *table;
    void **unk1C;
    void **unk20;
    char pad24[0x10];
    void *unk34;
};

struct Instance {
    struct Model *model;
    char coord[0x3C];
    s32 unk40;
    char pad44[4];
    s16 frame;
};

extern struct Matrix gBoneMatrices[] __asm__("D_800A0300");
extern struct Vec3s32 D_800A02E0;
extern struct Vec3s32 D_800A02F0;
extern char D_8009FAE0[];

extern void *D_8009F570;
extern void *D_8009F574;
extern void *gPartVertexBuffer __asm__("D_8009F578");
extern void *gClipFlagBuffer __asm__("D_8009F57C");

extern void func_800178DC(void *coord, struct Matrix *matrix);
extern void func_80010750(struct Matrix *matrix);
extern void func_800101D4(void *src, s16 *dst, s32 count);
extern u8 func_80019E70(s16 *point, s32 margin);
extern void func_80013ECC(struct Vec3s32 *vec);
extern void func_80013CB8(s32 idx, struct Input *input, s16 *table, s32 frame);
extern void func_8001010C(void *src, void *dst, s32 count);
extern void func_8001047C(void *src, void *dst_points, void *dst_flags, s32 count);
extern void func_8001AE14(struct Matrix *matrix, struct Vec3s32 *in, struct Vec3s32 *out);

/**
 * @brief Computes one model instance's matrix, tests it against the view volume
 *        and hands its vertex lists to the polygon clipper.
 *
 * @param instance Instance block. Fields used: model at +0x0, coordinate block
 *                 at +0x4 (0x3C bytes), culling margin at +0x40, animation
 *                 frame at +0x48.
 *
 * @return 1 when the instance submitted geometry, 0 when it was culled by
 *         func_80019E70 (the zero is loaded at 0x80013FB0 and returned from
 *         0x80014130).
 *
 * @par Globals
 * - D_800A0300 (0x800A0300): matrix workspace; the base path uses one 0x24-byte
 *   matrix plus the following position triple, the scripted path indexes it as
 *   an array of 0x30-byte matrices.
 * - D_800A02E0 (0x800A02E0) and D_800A02F0 (0x800A02F0): 12-byte vector
 *   workspace, source and destination of the light setup.
 * - D_8009FAE0 (0x8009FAE0): the buffer that receives the transformed vertices
 *   in the scripted path.
 * - D_8009F570 (0x8009F570, $gp 0x35C), D_8009F574 (0x8009F574, $gp 0x360),
 *   D_8009F578 (0x8009F578, $gp 0x364), D_8009F57C (0x8009F57C, $gp 0x368):
 *   published source pointers - vertex source, vertex source + 4, screen
 *   coordinates and clip flags - read by the polygon emitter.
 *
 * @par Control flow
 * Confirmed:
 * 1. model = instance->0. index = 0 when bit 25 of model+0x0C is set, otherwise
 *    the s16 at instance+0x48.
 * 2. func_800178DC(instance + 0x4, D_800A0300) then func_80010750(D_800A0300):
 *    the instance coordinate block becomes the matrix, and the matrix is loaded
 *    into the GTE with its translation.
 * 3. func_800101D4(model+0x34 + (index << 4) + 8, point, 1) transforms the
 *    frame's reference vertex; the 16-byte-per-frame record with its 8-byte
 *    header is the same +8 offset that func_8002EED8 uses.
 * 4. func_80019E70(point, instance+0x40) must return 2, i.e. the vertex must be
 *    inside the view volume with the instance margin; otherwise return 0.
 * 5. pts = D_8009F578, flg = D_8009F57C.
 * 6. Scripted path (bit 25 of model+0x0C set): D_800A02F0 = D_800A02E0,
 *    D_8009F570 = D_8009FAE0, input = model+0x14,
 *    D_8009F574 = model->0x1C[1] + 4, func_80013ECC(&D_800A02F0). Then per
 *    0x14-byte input entry: func_80013CB8(entry[0], entry, model+0x18,
 *    instance+0x48); func_80010750(D_800A0300 + entry[0] * 48);
 *    func_8001010C(model->0x20[1] + entry[1] * 8 + 4, D_8009FAE0 + entry[1] * 8,
 *    entry[2]); func_8001047C(model->0x1C[1] + entry[3] * 8 + 4,
 *    pts + entry[3] * 8, flg + entry[3], entry[4]). The loop ends after the
 *    entry whose byte +0x7 has bit 0x80 set.
 * 7. Single-list path: func_8001AE14(D_800A0300, &D_800A02E0, &D_800A02F0) and
 *    func_80013ECC(&D_800A02F0) build the light direction, then
 *    list = model->0x1C[index + 1], D_8009F574 = list + 4,
 *    D_8009F570 = model->0x20[index + 1] + 4 and
 *    func_8001047C(list + 4, pts, flg, *list) submits the whole list at once.
 * 8. Return 1.
 *
 * @par Calls
 * func_800178DC, func_80010750 (twice on the scripted path), func_800101D4,
 * func_80019E70, func_80013ECC, func_80013CB8, func_8001010C, func_8001047C,
 * func_8001AE14.
 *
 * @par Side effects
 * Overwrites D_800A0300 and D_800A02F0, republishes D_8009F570 and D_8009F574,
 * and leaves the GTE rotation matrix, translation and light direction at the
 * values of the last submitted part. The clip flag bytes are written into the
 * buffer at D_8009F57C.
 *
 * @par Likely role
 * The per-instance "submit model" primitive: func_80017508 calls it for every
 * instance whose model pointer is non-null and whose flag word does not carry
 * bit 0x20, and then calls func_80016A64 with the result, which is what makes
 * the visible/hidden return value meaningful. The two paths correspond to two
 * model formats: one list of vertices, or a script of 20-byte entries that each
 * select a matrix and two vertex ranges.
 *
 * @par Unknown
 * - What the two buffers at D_8009F578 and D_8009F57C hold exactly; the callers
 *   read them as vertex storage and clip-flag bytes, but nothing in this
 *   function initialises them.
 * - The meaning of entry[2] and entry[4] beyond "count" and "count" for the two
 *   calls they feed.
 * - Whether bit 25 of model+0x0C really means "scripted model" or only "ignore
 *   the animation frame", since it also forces index to 0.
 *
 * @par Callers
 * func_80017508, recorded at 0x80017508; it is the only direct caller.
 */
s32 func_80013F08(struct Instance *instance)
{
    struct Model *model;
    s32 index;
    s16 point[4];
    void *pts;
    void *flg;

    model = instance->model;
    index = 0;
    if (!((model->flags >> 25) & 1)) {
        index = instance->frame;
    }

    func_800178DC(&instance->coord, gBoneMatrices);
    func_80010750(gBoneMatrices);
    func_800101D4((char *)instance->model->unk34 + (index * 16 + 8), point, 1);

    if (func_80019E70(point, instance->unk40) != 2) {
        return 0;
    }

    flg = gClipFlagBuffer;
    pts = gPartVertexBuffer;

    if ((model->flags >> 25) & 1) {
        struct Input *input;
        struct Matrix *mat;
        register char *fae __asm__("$23");
        register char *tmp __asm__("$2");
        register char *p1 __asm__("$2");
        register char *p1data __asm__("$8");
        struct Vec3s32 *f0 = &D_800A02F0;

        mat = gBoneMatrices;
        __asm__ volatile("" :: "r"(f0));
        tmp = D_8009FAE0;
        __asm__ volatile("" : "+r"(tmp));
        fae = tmp;
        __asm__ volatile("" : "+r"(fae));
        input = model->inputs;
        D_8009F570 = fae;
        p1 = (char *)model->unk1C[1];
        *f0 = D_800A02E0;
        p1data = p1 + 4;
        __asm__ volatile("" : "+r"(p1data));
        D_8009F574 = p1data;
        func_80013ECC(f0);

        while (1) {
            u8 matrixIndex = input->matrixIndex;
            s32 u1;
            u8 unk2;
            s32 u3;
            u8 unk4;

            func_80013CB8(matrixIndex, input, model->table, instance->frame);
            func_80010750((struct Matrix *)(matrixIndex * 48 + (u32)mat));
            u1 = input->unk1;
            unk2 = input->unk2;
            func_8001010C((char *)model->unk20[1] + (u1 * 8 + 4),
                          (void *)((u1 * 8) + (u32)fae),
                          unk2);
            u3 = input->unk3;
            unk4 = input->unk4;
            func_8001047C((char *)model->unk1C[1] + (u3 * 8 + 4),
                          (char *)pts + (u3 * 8),
                          (char *)flg + u3,
                          unk4);
            if (input->flags & 0x80) {
                break;
            }
            input++;
        }
    } else {
        register s32 *p1C __asm__("$7");
        register char *t0 __asm__("$8");
        char *p20;
        s32 count;
        struct Vec3s32 *f0 = &D_800A02F0;
        register void *a1_pts __asm__("$5");
        register s32 idx __asm__("$3");

        func_8001AE14(gBoneMatrices, &D_800A02E0, f0);
        func_80013ECC(f0);

        a1_pts = pts;
        __asm__ volatile("" : "+r"(a1_pts));
        idx = index << 2;
        __asm__ volatile("" : "+r"(idx));
        p1C = *(s32 **)((char *)idx + (u32)model->unk1C + 4);
        t0 = (char *)p1C + 4;
        p20 = *(char **)((char *)idx + (u32)model->unk20 + 4) + 4;
        count = *p1C;
        D_8009F574 = t0;
        D_8009F570 = p20;
        func_8001047C(t0, a1_pts, flg, count);
    }

    return 1;
}
