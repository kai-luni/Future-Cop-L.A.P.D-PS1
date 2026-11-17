// Categories: Graphics > Effects > Part Motion | Graphics > Effects > Particles | Physics > Motion
#include "game/types.h"

struct Unk3D9F8Ctx {
    u8 pad00[0x34];
    s32 unk34;
};

struct Unk43CA8Inner {
    struct Unk43CA8Inner *next;
    s16 unk04;
    s16 unk06;
    s16 unk08;
    u8 pad0A[2];
    s16 unk0C;
    s16 unk0E;
    s16 unk10;
    s16 unk12;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u16 unk18;
};

struct Unk43CA8 {
    struct Unk43CA8 *next;
    u8 pad04[3];
    s8 unk07;
    u8 pad08[6];
    s8 unk0E;
    u8 unk0F;
    u8 pad10[0xA];
    u16 unk1A;
    struct Unk3D9F8Ctx *unk1C;
    u8 pad20[0x14];
    s32 unk34;
    u8 pad38[0x30];
    struct Unk43CA8Inner *unk68;
    u8 pad6C[6];
    s16 unk72;
    u8 pad74[2];
    s16 unk76;
    s16 unk78;
    s16 unk7A;
    u8 pad7C[0x10];
    u8 unk8C;
    u8 pad8D[9];
    s16 unk96;
    s16 unk98;
    s16 unk9A;
};

struct Unk3D9F8Ret {
    u8 pad00[0x24];
    s32 unk24;
    s32 unk28;
    s32 unk2C;
};

extern u32 gSceneFrameCounter __asm__("D_8009FA50");
extern s32 gRemainingCounter __asm__("D_8009FAB0");
extern struct Unk3D9F8Ret *func_8003D9F8(s32 arg0, u16 arg1, s8 arg2, s32 arg3,
                                          struct Unk3D9F8Ctx *arg4, s32 *arg5);
extern void func_80042864(struct Unk43CA8Inner *object, s32 amount);
extern void func_800428FC(struct Unk43CA8 *node, struct Unk43CA8Inner *inner,
                          s32 arg2, s32 arg3, s32 arg4);
extern s32 func_80066F78(s32 *vec, s32 arg1, s32 arg2, s32 arg3);
extern void func_800427A8(struct Unk43CA8 *owner, struct Unk43CA8Inner *prev,
                          struct Unk43CA8Inner *node);

/**
 * @brief Per-frame update of a particle/segment chain on an effect node:
 *        springs every segment toward the node anchor, ramps its colour between
 *        the node's two colour sets, and retires segments that hit the terrain
 *        or run out of life.
 *
 * @param node Chain owner: parent pointer at +0x1C (its +0x34 word is the
 *        parent record), 0x24-byte matrix at +0x34, active chain at +0x68, Q12
 *        scale at +0x72, anchor point at +0x76/+0x78/+0x7A, mode byte at +0x8C,
 *        rates at +0x96/+0x98/+0x9A, flag word at +0x1A, subtype at +0x0E,
 *        state byte at +0x0F, -1 sentinel at +0x07.
 * @param arg Frame delta in 1/300 s ticks - every rate is divided by 300.
 * @return Always 0.
 *
 * @par Confirmed
 * ctx = node->+0x1C ? its +0x34 word : 0, then
 * func_8003D9F8(ctx, node +0x1A, node +0x0E, 0, node->+0x1C, &node->+0x34) -
 * the same resolution func_80042AF4 performs - and the returned block's
 * +0x24/+0x28/+0x2C are the world translation used by the terrain probe.
 * An empty chain with node +0x07 == -1 sets bit 0 of +0x0F and returns 0; an
 * empty chain with any other +0x07 is accepted.
 * s2 = (arg << 12) / 300 and s4 = (node +0x96 * arg << 2) / 300; while bit 0 of
 * +0x8C is clear also sp38 = (node +0x98 * arg << 2) / 300 and
 * fpv = (node +0x9A * arg << 2) / 300.
 * Per segment (next at +0x00): rot = +0x12, pos = (+0x04/+0x06/+0x08 * node
 * +0x72) >> 12, d = (anchor +0x76/+0x78/+0x7A - +0x0C/+0x0E/+0x10) * rot,
 * step = ((((d << 4) >> 16) + pos) * s2) >> 12, then +0x0C/+0x0E/+0x10 += step
 * and +0x04/+0x06/+0x08 += (that new position * s2) >> 12. All six fields are
 * s16, so every store truncates to 16 bits.
 * Every (D_8009FA50 ^ i) & 3 == 0, where i counts the segments and D_8009FA50
 * is the frame counter: bit 0 of +0x8C calls func_80042864(segment,
 * (segment +0x14 & 1) ? 0 : s4); bit 1 calls func_800428FC(node, segment,
 * signed s4, signed sp38, signed fpv) with the signs from bits 8, 0x10, 0x20 of
 * segment +0x14; otherwise func_800428FC is called with the same three amounts
 * zeroed for segments flagged with 1, 2 or 4.
 * Every (D_8009FA50 ^ i) & 7 == 0, with bit 1 of node +0x1A clear and segment
 * +0x0E negative: vec = (+0x04/+0x06/+0x08 << 4) + world translation, and when
 * vec.y < func_80066F78(vec, 0, 0, 0) the segment is unlinked with
 * func_800427A8(node, prev, segment) and D_8009FAB0 is decremented.
 * Life: (+0x18 & 0x1FFF) - arg; at zero or below the segment is unlinked the
 * same way, otherwise +0x18 = (+0x18 & 0xE000) | life and the segment becomes
 * prev. D_8009FAB0 is the global live-segment counter - func_80042AF4 refuses
 * to spawn once it reaches 0x2BC.
 *
 * @par Likely role
 * One of the two per-node segment updaters behind func_800441DC (flag bit 0 of
 * +0x8D selects this one). func_80042AF4 writes the very same segment layout -
 * drift at +0x04..+0x08, running position at +0x0C..+0x10, colour channels
 * +0x15..+0x17, direction flags +0x14, life plus a 3-bit tier at +0x18 - and
 * starts each channel either at node +0x8F..+0x91 or at +0x92..+0x94 depending
 * on the sign of +0x96/+0x98/+0x9A. This function integrates the drift and
 * walks that colour ramp, while func_800428FC clamps each channel against
 * +0x8F..+0x94 and flips the direction bits in +0x14. The (frame ^ index) masks
 * stagger the clamp over four frames and the terrain probe over eight, so a long
 * chain never pays the whole per-segment cost in one frame.
 *
 * @par Unknown
 * What the three rates +0x96/+0x98/+0x9A and the +0x72 scale are called in the
 * design, and whether the single-rate mode (bit 0 of +0x8C) is a greyscale fade
 * - func_80042864 spends one amount across +0x17, then +0x16, then +0x15 -
 * rather than a per-channel ramp.
 *
 * @par Callers
 * func_800441DC only (its flag-bit-0 branch).
 */
s32 func_80043CA8(struct Unk43CA8 *node, s32 arg)
{
    struct Unk3D9F8Ret *ret;
    struct Unk43CA8Inner *cur;
    struct Unk43CA8Inner *next;
    s32 ctx;
    s32 i;
    s32 s4;
    struct Unk43CA8Inner *prev;
    s32 s2;
    u32 d;
    s32 sp38;
    s32 fpv;
    s32 s6;
    s16 pos[3];
    s32 vec[3];
    s32 dx;
    s32 rot;
    s32 dy;
    s32 dz;
    s16 rem;
    s32 vx, vy, vz;
    s16 nx, ny, nz;
    s32 px, py, pz;

    ctx = 0;
    if (node->unk1C != 0) {
        ctx = node->unk1C->unk34;
    }
    ret = func_8003D9F8(ctx, node->unk1A, node->unk0E, 0, node->unk1C, &node->unk34);
    cur = node->unk68;
    if (cur == 0 && node->unk07 == -1) {
        node->unk0F |= 1;
        return 0;
    }
    prev = 0;
    s2 = (arg << 12) / 300;
    s4 = (node->unk96 * arg << 2) / 300;
    if ((node->unk8C & 1) == 0) {
        sp38 = (node->unk98 * arg << 2) / 300;
        fpv = (node->unk9A * arg << 2) / 300;
    }
    d = gSceneFrameCounter;
    s6 = node->unk72;
    i = 0;
    while (cur != 0) {
        next = cur->next;
        rot = cur->unk12;
        pos[0] = (cur->unk04 * s6) >> 12;
        pos[1] = (cur->unk06 * s6) >> 12;
        pos[2] = (cur->unk08 * s6) >> 12;
        dx = (node->unk76 - cur->unk0C) * rot;
        dy = (node->unk78 - cur->unk0E) * rot;
        dz = (node->unk7A - cur->unk10) * rot;
        vx = (((dx << 4) >> 16) + pos[0]) * s2 >> 12;
        vy = (((dy << 4) >> 16) + pos[1]) * s2 >> 12;
        vz = (((dz << 4) >> 16) + pos[2]) * s2 >> 12;
        nx = cur->unk0C + vx;
        ny = cur->unk0E + vy;
        nz = cur->unk10 + vz;
        px = nx * s2 >> 12;
        py = ny * s2 >> 12;
        pz = nz * s2 >> 12;
        cur->unk0C = nx;
        cur->unk0E = ny;
        cur->unk10 = nz;
        cur->unk04 += px;
        cur->unk06 += py;
        cur->unk08 += pz;
        if (((d ^ i) & 3) == 0) {
            if (node->unk8C & 1) {
                func_80042864(cur, (cur->unk14 & 1) ? 0 : s4);
            } else if (node->unk8C & 2) {
                func_800428FC(node, cur, (cur->unk14 & 8) ? -s4 : s4,
                              (cur->unk14 & 0x10) ? -sp38 : sp38,
                              (cur->unk14 & 0x20) ? -fpv : fpv);
            } else {
                func_800428FC(node, cur, (cur->unk14 & 1) ? 0 : s4,
                              (cur->unk14 & 2) ? 0 : sp38,
                              (cur->unk14 & 4) ? 0 : fpv);
            }
        }
        if (((d ^ i) & 7) == 0 && !(node->unk1A & 2) && cur->unk0E < 0) {
            vec[0] = (cur->unk04 << 4) + ret->unk24;
            vec[1] = (cur->unk06 << 4) + ret->unk28;
            vec[2] = (cur->unk08 << 4) + ret->unk2C;
            if (vec[1] < func_80066F78(vec, 0, 0, 0)) {
                func_800427A8(node, prev, cur);
                gRemainingCounter--;
                goto next_iter;
            }
        }
        rem = cur->unk18 & 0x1FFF;
        if (rem != 0) {
            rem -= arg;
            if ((s16)rem <= 0) {
                func_800427A8(node, prev, cur);
                gRemainingCounter--;
                goto next_iter;
            }
        }
        prev = cur;
        cur->unk18 = (cur->unk18 & 0xE000) | rem;
next_iter:
        cur = next;
        i++;
    }
    return 0;
}
