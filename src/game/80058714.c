#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Model;

struct ModelInstance {
    struct Model *model;
    u8 coord[0x44];
    s16 frame;
};

struct Model {
    u8 unk00[0x0C];
    u32 flags;
    u8 unk10[0x24];
    u8 *unk34;
};

struct UnkDC {
    u8 unk00[8];
    u32 unk08;
    u8 unk0C[0x18];
    u32 unk24;
};

struct Unk5A190 {
    u8 unk00[0x8A];
    u16 unk8A[3];
    u8 unk90[0x1D];
    u8 unkAD;
    u8 unkAE[0x12];
    u8 unkC0;
    u8 unkC1[3];
    struct Vec3s32 unkC4;
    u8 unkD0[2];
    u16 unkD2;
    u8 unkD4[0x229];
    u8 unk2FD;
};

struct Unk58714 {
    u8 unk00[8];
    u32 unk08;
    u8 unk0C[0x0D];
    u8 unk19;
    u8 unk1A[2];
    s16 unk1C;
    u16 unk1E;
    u8 unk20[4];
    u32 unk24;
    struct ModelInstance model;
    u8 unk74[0x24];
    void *unk98;
    u8 unk9C[0x10];
    struct Unk5A190 *unkAC;
    s16 unkB0;
    s16 unkB2;
    s16 unkB4;
    u8 unkB6[0x0E];
    s32 unkC4;
    u8 unkC8[0x14];
    struct UnkDC *unkDC;
};

extern void func_80010750(u8 *coord);
extern void func_80010234(u8 *vertices, s32 *out, s32 count);
extern void func_80017668(struct ModelInstance *instance, s32 part, s32 *out);
extern void func_8005A190(struct Unk5A190 *object, s32 index, s32 value);
extern void func_80051AF8(struct Unk58714 *arg0);
extern void func_8006BE90(struct Unk58714 *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_80069D68(void *arg0, s32 arg1, s32 arg2, struct Unk58714 *arg3);

void func_80058714(struct Unk58714 *arg0)
{
    struct Unk5A190 *s1;
    s32 i;
    s32 d2;

    {
        register u32 mask __asm__("$5");
        mask = 0xFF7FFFFF;
        arg0->unk08 &= mask;
        arg0->unkDC->unk08 &= mask;
    }

    arg0->unk19 &= 0x0F;
    s1 = arg0->unkAC;

    *(struct Vec3s32 *)(arg0->model.coord + 0x24) = s1->unkC4;

    func_80010750(arg0->model.coord);
    func_80010234((u8 *)arg0->model.model->unk34 +
                      ((arg0->model.frame << 4) + 8),
                  (s32 *)(arg0->model.coord + 0x30), 1);
    func_80017668(&arg0->model, 0, (s32 *)((u8 *)arg0->unkDC + 0x50));

    d2 = s1->unkD2;
    arg0->unkB0 = d2;
    if ((s16)d2 < 0) {
        register s32 neg __asm__("$2");
        neg = -d2;
        arg0->unkB0 = neg;
        s1->unk2FD = 1;
        s1->unkAD = 1;
    } else {
        s1->unk2FD = 0;
        s1->unkAD = 0;
    }

    arg0->unkB4 = 0;
    arg0->unkB2 = 0;
    arg0->unkC4 = 0;
    arg0->unk24 &= 0xFCFFFFEF;
    arg0->unkDC->unk24 &= 0xFCFFFFFF;
    s1->unkC0 = 0;

    arg0->unk1C = arg0->unk1E;
    func_8005A190(s1, 0, arg0->unk1C);

    func_8006BE90(arg0, 0, -1, 0);
    func_8006BE90(arg0, 1, -1, 0);
    func_8006BE90(arg0, 2, -1, 0);

    for (i = 2; i >= 0; i--) {
        s1->unk8A[i] = 0;
    }

    func_80051AF8(arg0);

    if (arg0->unk98 != 0) {
        func_80069D68(arg0->unk98, 0, -1, arg0);
    }
}
