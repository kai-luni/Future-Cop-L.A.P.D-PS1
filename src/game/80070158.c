#include "game/types.h"

struct Vec70158 {
    s32 x;
    s32 y;
    s32 z;
};

struct Sub70158 {
    u8 unk00[0x34];
    void *model;
};

struct Node70158 {
    struct Node70158 *next;
    u8 unk04[0x0A];
    s8 unk0E;
    u8 unk0F;
    u8 unk10[2];
    s16 unk12;
    u8 unk14[2];
    u16 unk16;
    u8 unk18[2];
    u16 unk1A;
    struct Sub70158 *unk1C;
    u8 unk20[0x14];
    u8 unk34[0x44];
    s32 unk78;
    s32 unk7C;
    u8 unk80[0x0C];
    u16 unk8C;
    u8 unk8E;
    u8 unk8F;
    s16 unk90;
};

struct Camera70158 {
    u8 unk00[0x28];
    u8 unk28[0x54];
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Transform70158 {
    u8 unk00[0x24];
    s32 x;
    s32 y;
    s32 z;
};

extern struct Camera70158 *D_8009F9E4;

extern struct Transform70158 *func_8003D9F8(void *model, s32 flags, s32 part,
                                            s32 count, void *sub, void *matrix);
extern s32 func_8001A100(s32 *vector);
extern void func_80010820(void *matrix);
extern void func_8001010C(void *src, void *dst, s32 arg2);
extern s32 func_80019D90(s16 *point, s32 margin);
extern void func_80010704(u32 *src, u32 *dst);
extern void func_80014490(void *dst, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6);
extern void func_8001484C(void *dst, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6);

void func_80070158(struct Node70158 *node, s32 *origin)
{
    register struct Node70158 *n __asm__("$16");
    register s32 *org __asm__("$21");
    struct Sub70158 *sub;
    struct Transform70158 *t;
    struct Camera70158 *camera;
    void *model;
    s32 vec[3];
    u32 dst[2];
    s16 src[3];
    s32 color;

    n = node;
    org = origin;
    for (; n != 0; n = n->next) {
        if (n->unk0F & 8) {
            continue;
        }

        sub = n->unk1C;
        model = 0;
        if (sub != 0) {
            model = sub->model;
        }
        t = func_8003D9F8(model, n->unk1A, n->unk0E, 0, sub, &n->unk34);

        vec[0] = t->x - org[0];
        vec[1] = t->y - org[1];
        vec[2] = t->z - org[2];
        if (func_8001A100(vec) - (n->unk12 << 4) > 0x1FFFF) {
            continue;
        }

        camera = D_8009F9E4;
        vec[0] = t->x - camera->unk7C;
        vec[1] = t->y - camera->unk80;
        vec[2] = t->z - camera->unk84;
        src[0] = vec[0] >> 4;
        src[1] = vec[1] >> 4;
        src[2] = vec[2] >> 4;
        func_80010820(&camera->unk28);

        func_8001010C(src, dst, 1);
        if ((func_80019D90(dst, n->unk12 << 4) & 0xFF) != 2) {
            continue;
        }
        func_80010704(src, dst);

        if (n->unk8C & 2) {
            func_8001484C(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200,
                          (n->unk90 << 8) | n->unk8F);
        } else if (n->unk8C & 4) {
            func_80014490(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200,
                          (n->unk90 << 8) | n->unk8F);
        } else {
            func_8001484C(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200, 0);
        }
    }
}
