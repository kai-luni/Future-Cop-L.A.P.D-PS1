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

extern u32 D_8009FA50;
extern s32 D_8009FAB0;
extern struct Unk3D9F8Ret *func_8003D9F8(s32 arg0, u16 arg1, s8 arg2, s32 arg3,
                                          struct Unk3D9F8Ctx *arg4, s32 *arg5);
extern void func_80042864(struct Unk43CA8Inner *object, s32 amount);
extern void func_800428FC(struct Unk43CA8 *node, struct Unk43CA8Inner *inner,
                          s32 arg2, s32 arg3, s32 arg4);
extern s32 func_80066F78(s32 *vec, s32 arg1, s32 arg2, s32 arg3);
extern void func_800427A8(struct Unk43CA8 *owner, struct Unk43CA8Inner *prev,
                          struct Unk43CA8Inner *node);

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
    d = D_8009FA50;
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
                D_8009FAB0--;
                goto next_iter;
            }
        }
        rem = cur->unk18 & 0x1FFF;
        if (rem != 0) {
            rem -= arg;
            if ((s16)rem <= 0) {
                func_800427A8(node, prev, cur);
                D_8009FAB0--;
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
