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

extern struct Matrix D_800A0300[];
extern struct Vec3s32 D_800A02E0;
extern struct Vec3s32 D_800A02F0;
extern char D_8009FAE0[];

extern void *D_8009F570;
extern void *D_8009F574;
extern void *D_8009F578;
extern void *D_8009F57C;

extern void func_800178DC(void *coord, struct Matrix *matrix);
extern void func_80010750(struct Matrix *matrix);
extern void func_800101D4(void *src, s16 *dst, s32 count);
extern u8 func_80019E70(s16 *point, s32 margin);
extern void func_80013ECC(struct Vec3s32 *vec);
extern void func_80013CB8(s32 idx, struct Input *input, s16 *table, s32 frame);
extern void func_8001010C(void *src, void *dst, s32 count);
extern void func_8001047C(void *src, void *dst_points, void *dst_flags, s32 count);
extern void func_8001AE14(struct Matrix *matrix, struct Vec3s32 *in, struct Vec3s32 *out);

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

    func_800178DC(&instance->coord, D_800A0300);
    func_80010750(D_800A0300);
    func_800101D4((char *)instance->model->unk34 + (index * 16 + 8), point, 1);

    if (func_80019E70(point, instance->unk40) != 2) {
        return 0;
    }

    flg = D_8009F57C;
    pts = D_8009F578;

    if ((model->flags >> 25) & 1) {
        struct Input *input;
        struct Matrix *mat;
        register char *fae __asm__("$23");
        register char *tmp __asm__("$2");
        register char *p1 __asm__("$2");
        register char *p1data __asm__("$8");
        struct Vec3s32 *f0 = &D_800A02F0;

        mat = D_800A0300;
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

        func_8001AE14(D_800A0300, &D_800A02E0, f0);
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
