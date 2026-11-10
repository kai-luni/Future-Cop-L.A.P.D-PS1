#include "game/types.h"

struct Model3A10 {
    u8 pad00[0x34];
    void *unk34;
};

struct Node3A10 {
    struct Model3A10 *model;
    s32 mat[3][3];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[8];
    s16 unk48;
    u8 flags4A;
    s8 unk4B;
    u8 flags4C;
    u8 unk4D;
    s16 unk4E;
    union {
        s32 word;
        struct { u16 low; u16 high; } half;
    } unk50;
    u16 unk54;
    s8 unk56;
    u8 unk57;
    struct Node3A10 *next;
};

struct Req3A10 {
    u16 unk00;
    u8 pad02[6];
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s8 unk1C;
    u8 unk1D;
    u8 unk1E;
    s8 unk1F;
    u16 unk20;
    u8 unk22;
    u8 unk23;
    void *unk24;
    void *unk28;
    s32 unk2C;
};

struct Vec3I { s32 x; s32 y; s32 z; };
struct Bucket3A { struct Node3A10 *head; s32 arena; };

extern void *D_8009F934;
extern struct Bucket3A *D_8009F938;
extern s32 D_8009F93C;
extern s32 D_8009FAA4;
extern void *func_800195A4(void *state);
extern void func_80083834(s32 arg0, s32 arg1);
extern void func_80013BD8(struct Node3A10 *node, struct Model3A10 *model, s32 options);
extern u32 func_80019668(void);
extern s32 func_80067684(void *vec, s32 adjust);
extern void func_800188D0(s32 x, s32 y, s32 z, void *arg3);
extern void func_80010820(void *matrix);
extern void func_8001006C(void *src, void *dst, s32 count);
extern s32 func_80083764(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void func_80010750(void *a);
extern void func_80010234(void *a, void *b, s32 c);

u8 func_80083A10(struct Req3A10 *req, struct Model3A10 *model, s32 z2, s32 *arg3)
{
    struct Node3A10 *node;
    s32 mtx[12];
    struct Vec3I dst[4];
    s32 hi;
    s32 lo;
    s32 hi2;
    s32 lo2;
    s32 v;
    register s32 term __asm__("$2");
    s32 px;
    s32 py;
    s32 pz;
    s32 idx;
    s32 adjust;
    register s32 originX __asm__("$4");
    u8 rate;
    struct Node3A10 *head;
    s16 *blk;
    s32 scale;
    s32 i;
    s32 j;
    s8 *q;
    s32 *sp;

    node = (struct Node3A10 *)func_800195A4(D_8009F934);
    if (node == 0) {
        func_80083834(D_8009FAA4 - 0x5DC, 1);
        node = (struct Node3A10 *)func_800195A4(D_8009F934);
        if (node == 0) {
            return 0;
        }
    }
    func_80013BD8(node, model, 0);
    node->unk4B = -1;
    node->unk56 = req->unk22;
    node->unk54 = req->unk00;
    node->flags4C = req->unk1D;
    if (node->flags4C & 1) {
        node->flags4A |= 8;
    }
    if (node->flags4C & 2) {
        node->flags4A |= 4;
    }
    rate = req->unk1E;
    if (rate != 0) {
        v = rate;
        if (v == 1) {
            node->unk4D = 0xFF;
        } else {
            node->unk4D = 300 / v;
        }
    } else {
        node->unk4D = 0;
    }
    if (z2 >= 0) {
        z2 = z2 + D_8009FAA4;
        node->unk50.word = z2;
    } else {
        node->flags4C |= 0x80;
        node->unk50.half.low = (u16)req->unk14;
        node->unk50.half.high = ((s32)req->unk23 << 12) / 360;
    }
    if (arg3 != 0) {
        node->mat[0][0] = arg3[0];
        node->mat[0][1] = arg3[1];
        node->mat[0][2] = arg3[2];
        node->mat[1][0] = arg3[3];
        node->mat[1][1] = arg3[4];
        node->mat[1][2] = arg3[5];
        node->mat[2][0] = arg3[6];
        node->mat[2][1] = arg3[7];
        node->mat[2][2] = arg3[8];
        node->unk28 = arg3[9];
        node->unk2C = arg3[10];
        node->unk30 = arg3[11];
        if (node->flags4C & 0x40) {
            node->unk57 = (u8)req->unk16;
        } else {
            node->unk57 = 0;
        }
    } else {
        *(struct Vec3I *)&node->unk28 = *(struct Vec3I *)&req->unk08;
        v = req->unk1C;
        if (v != -99) {
            adjust = (v == 0) ? -1 : (v == 1) ? -2 : v - 2;
            v = func_80067684(&node->unk28, adjust);
            node->unk2C = v + (req->unk1A << 4);
        }
        {
            register s32 ax __asm__("$4");
            ax = req->unk14;
            px = ax;
            if (ax < 0) {
                px = func_80019668() & 0xFFF;
            }
        }
        {
            register s32 ay __asm__("$5");
            ay = req->unk18;
            py = ay;
            if (ay < 0) {
                py = func_80019668() & 0xFFF;
            }
        }
        pz = 0;
        if (node->flags4C & 0x40) {
            node->unk57 = (u8)req->unk16;
        } else {
            pz = req->unk16;
            if (pz < 0) {
                pz = func_80019668() & 0xFFF;
            }
            node->unk57 = 0;
        }
        func_800188D0(px, py, pz, &node->mat[0][0]);
        i = 0;
        if (req->unk1F != 0x40 || req->unk20 != 0x4040) {
            q = (s8 *)node;
            do {
                scale = ((s8 *)((s32)req + i))[0x1F];
                if (scale < 0) {
                    s32 r;
                    r = func_80019668() % (u32)(-(2 * scale));
                    r += 64;
                    scale += r;
                }
                j = 0;
                sp = (s32 *)(q + 4);
                do {
                    term = *sp * scale;
                    *sp = term >> 6;
                    j = j + 1;
                    sp = sp + 1;
                } while (j < 3);
                i = i + 1;
                q = q + 12;
            } while (i < 3);
        }
    }
    if (node->unk56 != 0) {
        node->mat[1][1] = 0x10000;
        node->mat[2][1] = 0;
        node->mat[1][2] = 0;
        node->mat[1][0] = 0;
        node->mat[0][1] = 0;
    }
    blk = (s16 *)node->model->unk34 + 4;
    mtx[9] = blk[0] << 4;
    mtx[10] = blk[1] << 4;
    mtx[11] = blk[2] << 4;
    mtx[0] = blk[3] << 4;
    mtx[1] = 0;
    mtx[2] = 0;
    mtx[3] = 0;
    mtx[4] = blk[4] << 4;
    mtx[5] = 0;
    mtx[6] = 0;
    mtx[7] = 0;
    mtx[8] = blk[5] << 4;
    func_80010820(&node->mat[0][0]);
    func_8001006C(mtx, dst, 4);
    originX = node->unk28;
    dst[3].y += node->unk2C;
    dst[3].z += node->unk30;
    *(struct Vec3I *)&node->unk34 = dst[3];
    term = dst[3].x;
    hi = mtx[0];
    term += originX;
    lo = hi;
    dst[3].x = term;
    if (hi > 0) {
        lo = -hi;
    } else {
        hi = -hi;
    }
    term = mtx[3];
    if (term > 0) {
        lo -= term;
        hi += term;
    } else {
        lo += term;
        hi -= term;
    }
    term = mtx[6];
    if (term > 0) {
        lo -= term;
        hi += term;
    } else {
        lo += term;
        hi -= term;
    }
    hi2 = mtx[2];
    lo2 = hi2;
    if (hi2 > 0) {
        lo2 = -hi2;
    } else {
        hi2 = -hi2;
    }
    term = mtx[5];
    if (term > 0) {
        lo2 -= term;
        hi2 += term;
    } else {
        lo2 += term;
        hi2 -= term;
    }
    term = mtx[8];
    if (term > 0) {
        lo2 -= term;
        hi2 += term;
    } else {
        lo2 += term;
        hi2 -= term;
    }
    node->unk4E = func_80083764(dst[3].z, dst[3].x,
                               dst[3].z + lo2, dst[3].z + hi2,
                               dst[3].x + lo, dst[3].x + hi);
    idx = (dst[3].z >> 17) + D_8009F93C * (dst[3].x >> 17);
    if (z2 >= 0 || D_8009F938[idx].head == 0) {
        node->next = D_8009F938[idx].head;
        D_8009F938[idx].head = node;
    } else {
        head = D_8009F938[idx].head;
        while (head->next != 0) {
            head = head->next;
        }
        head->next = node;
        node->next = 0;
    }
    D_8009F938[idx].arena = D_8009FAA4;
    if (node->unk57 != 0) {
        register s32 frame __asm__("$4");
        func_80010750(&node->mat[0][0]);
        frame = node->unk48;
        frame <<= 4;
        __asm__("" : "+r"(frame));
        frame += 8;
        func_80010234((u8 *)node->model->unk34 + frame,
                      &node->unk34, 1);
    }
    return 1;
}
