// Categories: Graphics > Model Renderer | Gameplay > Movement
#include "game/types.h"

struct Matrix32 {
    s32 m[3][3];
};

struct SubStruct1 {
    u8 pad0[0x70];
    s16 unk70;
};

struct SubStruct2 {
    u8 pad0[0x34];
    u8 *unk34;
};

struct SubStruct3 {
    s32 count;
    s32 stride;
};

struct SubStruct4 {
    u8 pad0[0x34];
    struct SubStruct3 *unk34;
};

struct VecAC {
    u8 pad0[0x150];
    s32 x;
    s32 y;
    s32 z;
};

struct Arg0 {
    u8 pad00[0x28];
    struct SubStruct2 *unk28;
    u8 pad2C[0x24];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 pad5C[0x0C];
    s32 unk68;
    u8 pad6C[0x40];
    struct VecAC *unkAC;
    u8 padB0[0x08];
    u8 unkB8;
    u8 padB9[0x23];
    struct SubStruct1 *unkDC;
};

struct Arg1 {
    struct SubStruct4 *unk00;
    struct Matrix32 matrix;
    u8 pad28[0x0C];
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    u8 pad44[0x04];
    s16 unk48;
};

extern s32 gFrameDelta __asm__("D_8009F9FC");

extern void gTransposeMatrix3x3(struct Matrix32 *source, struct Matrix32 *destination) __asm__("func_80019470");
extern s32 func_8001A7AC(s32 *arg0);
extern void func_8001ABE0(s32 scale, s32 *p);
extern void gPushFromPartBox(struct Arg0 *arg0, void *arg1, s32 arg2, void *arg3,
                          struct Matrix32 *arg4, struct Matrix32 *arg5, s32 arg6) __asm__("func_80051CC4");
void gApplyPlayerModelCollisionPush(struct Arg0 *arg0, struct Arg1 *arg1, s32 arg2, s32 arg3) __asm__("func_80051EB8");


/**
 * @brief Feeds one model instance either a camera push or its transform pass.
 *
 * With bit 0 of arg2 set the function works in two dimensions: the delta
 * between the model position (0x34/0x3C) and the owner's anchor (0x50/0x58) is
 * measured with func_8001A7AC, reduced by the two radii, clamped to
 * +/-0x1000 and scaled by D_8009F9FC before func_8001ABE0 applies it to the
 * owner's world position at 0xAC. Otherwise the model matrix is transposed with
 * func_80019470, the part table and the sub-object offset are resolved, and for
 * every part bit in arg2 (starting at 0x8000, shifting down per part) the part
 * is passed to func_80051CC4 together with both the original and the transposed
 * matrix. Both paths mark the model as updated with flag 0x10 in 0xB8.
 *
 * @param arg0 Owner with the model handle at 0xAC and the animation table at 0x28.
 * @param arg1 Model instance with matrix, position and part index.
 * @param arg2 Part bit mask, bit 0 selects the 2D push path.
 * @param arg3 Extra argument forwarded to the part renderer.
 *
 * @par Globals
 * D_8009F9FC - global push scale.
 *
 * @par Control flow
 * Two paths: a short clamped push, or a masked loop over the model parts.
 *
 * @par Calls
 * func_8001A7AC - 2D distance. func_8001ABE0 - scaled vector apply.
 * func_80019470 - matrix transpose. func_80051CC4 - part transform/render.
 *
 * @par Side effects
 * Moves the owner position or transforms the model parts.
 *
 * @par Callers
 * Likely: func_80052098 (same block).
 */
void gApplyPlayerModelCollisionPush(struct Arg0 *arg0, struct Arg1 *arg1, s32 arg2, s32 arg3)
{
    s32 vec[2];
    struct Matrix32 transposed;

    if (arg2 & 1) {
        s32 ret;
        s32 diff;
        s32 scale;

        vec[0] = arg1->unk34 - arg0->unk50;
        vec[1] = arg1->unk3C - arg0->unk58;
        ret = func_8001A7AC(vec);
        diff = ret - (arg1->unk40 + arg0->unk68);
        if (diff < -0x1000) {
            scale = -0x1000;
        } else if (diff <= 0x1000) {
            scale = diff;
        } else {
            scale = 0x1000;
        }
        diff = scale * gFrameDelta;
        func_8001ABE0(diff << 1, vec);
        arg0->unkAC->x += vec[0];
        arg0->unkAC->z += vec[1];
    } else {
        struct Matrix32 *src_mat;
        struct Matrix32 *dst_mat;
        struct SubStruct3 *s3;
        u8 *s2;
        u32 mask;
        s32 count;
        s32 s7;
        u8 *tbl;
        s16 idx;
        void *fp_val;
        s32 offset;

        src_mat = &arg1->matrix;
        dst_mat = &transposed;
        s3 = arg1->unk00->unk34;
        offset = (arg1->unk48 << 4) + 8;
        s2 = (u8 *)s3 + offset;
        gTransposeMatrix3x3(src_mat, dst_mat);

        mask = 0x8000;
        count = s3->count - 1;
        idx = arg0->unkDC->unk70;
        tbl = arg0->unk28->unk34;
        s7 = *(s16 *)(tbl + (idx << 4) + 0x16);
        fp_val = (u8 *)arg0 + 0x28;

        for (; count >= 0; count--) {
            if (arg2 & mask) {
                gPushFromPartBox(arg0, (u8 *)fp_val + 0x34, s7, s2, src_mat, dst_mat, arg3);
            }
            mask >>= 1;
            s2 += s3->stride;
        }
    }

    arg0->unkB8 |= 0x10;
}
