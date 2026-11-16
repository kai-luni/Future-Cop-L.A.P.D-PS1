// Categories: Graphics > Scene Graph > Attached Parts | Graphics > Renderer > Polygon Pass | Graphics > Culling | Graphics > Clipping
#include "game/types.h"

struct Pair8 {
    u8 b[8];
};

struct Obj4C5C8 {
    u8 pad00[0x28];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    u8 pad34[0x0C];
    u16 unk40;
    u8 unk42;
};

struct Sub4C5C8 {
    u8 pad00[0x20];
    struct Pair8 unk20;
    u8 unk28;
    u8 unk29;
    u16 unk2A;
    s16 unk2C;
    s16 unk2E;
    u8 pad30[0x18];
    struct Sub4C5C8 *unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
};

struct Vec3 {
    s32 v[3];
};

struct Node4C5C8 {
    struct Node4C5C8 *next;
    u8 pad04[0x0B];
    u8 unk0F;
    u8 pad10[0x0C];
    struct Obj4C5C8 *obj;
    u8 pad20[0x5B];
    u8 unk7B;
    u8 pad7C[0x04];
    u32 unk80;
    struct Sub4C5C8 *unk84;
};

struct Unk8009F9E4 {
    u8 pad00[0x28];
    s32 unk28[0x15];
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Work50 {
    u8 pad00[0x20];
    struct Pair8 unk20;
};

struct Frame {
    s32 mat[9];
    s32 out34[3];
    s32 vec[3];
    s32 pad4C;
    struct Work50 work;
    s32 buf78[9];
    struct Vec3 src;
    u8 out[4];
    s32 padAC;
};

extern struct Unk8009F9E4 *gCamera __asm__("D_8009F9E4");

extern s32 *func_8003D9C4(struct Node4C5C8 *node);
extern s32 func_8001A100(s32 *vec);
extern void func_80013AA0(s32 low, s32 high);
extern void func_800188D0(s16 a, s16 b, s32 c, s32 *out);
extern void func_80010820(s32 *p);
extern void func_8001006C(s32 *src, s32 *dst, s32 n);
extern void func_80010750(s32 *m);
extern void func_8001047C(struct Sub4C5C8 *sub, struct Work50 *w, u8 *out, u8 idx);
extern void func_800172A8(s32 idx, u8 a1, s32 a2, struct Work50 *work);

/**
 * @brief Walks one scene-node list and submits the attached parts of every node
 *        that is close enough to the reference point.
 *
 * @param arg0 Head of the node list. Fields used: next at +0x0, object pointer
 *             at +0x1C, child/part list at +0x84, suppression byte at +0x0F,
 *             flags at +0x7B, extra value at +0x80. Returns immediately when it
 *             is null.
 * @param arg1 Three reference coordinates (the caller passes the shared position
 *             from D_800A1070 / D_8009F9A0); each node position is compared
 *             with them and the camera offset is applied separately.
 *
 * @return Nothing.
 *
 * @par Globals
 * - D_8009F9E4 (0x8009F9E4): camera block; eye at +0x7C..0x84 and the view
 *   matrix at +0x28.
 * - D_8009F580 (written indirectly through func_80013AA0): the packed 16-bit
 *   colour pair, set from obj+0x40 / obj+0x41 and cleared afterwards.
 *
 * @par Control flow
 * Confirmed:
 * 1. For every node: skip when node+0x0F & 8 or node+0x7B & 0x18.
 * 2. base = func_8003D9C4(node), the mesh slot; the node position is
 *    base[9], base[10], base[11] at +0x24, +0x28, +0x2C, minus arg1.
 * 3. Distance cull: func_8001A100(vec) > 0x1FFFF skips the node (the constant is
 *    built with lui 1 / ori 0xFFFF).
 * 4. obj = node->0x1C; when it is non-null and its u16 at +0x40 is non-zero,
 *    func_80013AA0(obj[0x40], obj[0x41]) installs the per-object colour pair.
 * 5. Part loop over sub = node->0x84: idx = sub+0x28 & 0x7F;
 *    func_800188D0(sub+0x2C, sub+0x2E, 0, matrix) builds the part rotation from
 *    two s16 angles. The origin is either the triple at sub+0x4C or, when
 *    sub+0x28 has bit 0x80, the triple at *(sub+0x4C) + 0x28.
 * 6. Camera relative vector, func_80010820(camera + 0x28),
 *    func_8001006C(vec, out, 1), func_8001006C(matrix, mat, 3),
 *    func_80010750(mat) - so the part matrix is transformed by the view matrix.
 * 7. func_8001047C(sub, work, flags, idx) projects the part's vertex list and
 *    writes one clip byte per vertex.
 * 8. Skip test: the OR of bytes work[0], work[1], work[2] and work[idx - 1]
 *    (addressed as 0xA7 + idx of the frame) must have bit 0 clear. Then the
 *    8-byte colour field at sub+0x20 is copied into the work block (lwl/lwr
 *    pair) and func_800172A8(idx, sub+0x29, node+0x80 + sub+0x2A, work) emits the
 *    polygon.
 * 9. sub = sub+0x48 next part; after the loop, when obj+0x40 is non-zero,
 *    func_80013AA0(0, 0) restores the colour pair.
 *
 * @par Calls
 * func_8003D9C4, func_8001A100, func_80013AA0 (up to twice per node),
 * func_800188D0, func_80010820, func_8001006C (twice), func_80010750,
 * func_8001047C, func_800172A8.
 *
 * @par Side effects
 * Emits one polygon per visible part through func_800172A8, writes the global
 * colour pair around each node, and leaves the GTE light/rotation state at the
 * last part that was drawn.
 *
 * @par Likely role
 * The "attached parts" pass of the layer draw: func_8003F6E4 calls a fixed
 * sequence of passes with the same position argument, and this one receives
 * D_800A1070+0x10. Culling against 0x1FFFF units and the per-part index taken
 * from a 7-bit field read as a layer/batch selector.
 *
 * @par Unknown
 * - What node+0x80 and sub+0x2A mean; their sum is func_800172A8's third
 *   argument, so it is most likely a texture or palette index.
 * - What sub+0x29 selects (second argument of func_800172A8).
 * - Why the clip test ORs bytes 0, 1, 2 and idx - 1 only: idx vertices were
 *   projected, so bytes 3..idx-2 are never consulted.
 *
 * @par Callers
 * func_8003F6E4, recorded at 0x8003F6E4 (direct call at 0x8003F750), the layer
 * dispatcher that also calls func_8006E988, func_800445B0, func_80070158 and
 * func_80074104 with the same position triple.
 */
void func_8004C5C8(struct Node4C5C8 *arg0, s32 *arg1)
{
    struct Frame f;
    register struct Work50 *wp __asm__("$20");
    register s32 *pos __asm__("$21");
    struct Node4C5C8 *node;
    struct Node4C5C8 *cur;
    struct Sub4C5C8 *sub;
    struct Obj4C5C8 *obj;
    struct Unk8009F9E4 *g;
    s32 *base;
    u8 idx;

    node = arg0;
    pos = arg1;
    if (node == 0) {
        return;
    }
    wp = &f.work;

    for (; node != 0; node = node->next) {
        if (node->unk0F & 8) {
            continue;
        }
        cur = node;
        if (node->unk7B & 0x18) {
            continue;
        }
        sub = node->unk84;
        base = func_8003D9C4(node);
        f.vec[0] = base[9] - pos[0];
        f.vec[1] = base[10] - pos[1];
        f.vec[2] = base[11] - pos[2];
        if (func_8001A100(f.vec) > 0x1FFFF) {
            continue;
        }
        obj = node->obj;
        if (obj != 0 && obj->unk40 != 0) {
            func_80013AA0(((u8 *)obj)[0x40], ((u8 *)obj)[0x41]);
        }
        while (sub != 0) {
            idx = sub->unk28 & 0x7F;
            func_800188D0(sub->unk2C, sub->unk2E, 0, f.buf78);
            if (sub->unk28 & 0x80) {
                f.src = *(struct Vec3 *)((u8 *)sub->unk4C + 0x28);
            } else {
                f.src = *(struct Vec3 *)&sub->unk4C;
            }
            g = gCamera;
            f.vec[0] = f.src.v[0] - g->unk7C;
            f.vec[1] = f.src.v[1] - g->unk80;
            f.vec[2] = f.src.v[2] - g->unk84;
            func_80010820(&g->unk28[0]);
            func_8001006C(f.vec, f.out34, 1);
            func_8001006C(f.buf78, f.mat, 3);
            func_80010750(f.mat);
            func_8001047C(sub, wp, f.out, idx);
            if (!((f.out[0] | f.out[1] | f.out[2] | f.out[idx - 1]) & 1)) {
                f.work.unk20 = sub->unk20;
                func_800172A8(idx, sub->unk29, cur->unk80 + sub->unk2A, wp);
            }
            sub = sub->unk48;
        }
        if (cur->obj != 0 && cur->obj->unk40 != 0) {
            func_80013AA0(0, 0);
        }
    }
}
