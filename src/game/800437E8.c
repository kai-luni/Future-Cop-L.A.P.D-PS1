#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk437E8Model {
    u8 unk00[0x34];
    void *model;
};

struct Unk437E8Transform {
    s32 matrix[9];
    struct Vec3s32 pos;
};

struct Unk437E8Part {
    struct Unk437E8Part *next;
    s16 unk04;
    s16 unk06;
    s16 unk08;
    u16 unk0A;
    s16 unk0C;
    s16 unk0E;
    s16 unk10;
    s16 unk12;
    u8 unk14;
    u8 total;
    u8 first;
    u8 second;
    u16 unk18;
};

struct Unk437E8 {
    struct Unk437E8 *next;
    u8 pad04[3];
    s8 unk07;
    u8 pad08[6];
    s8 unk0E;
    u8 unk0F;
    u8 pad10[0x0A];
    u16 unk1A;
    struct Unk437E8Model *sub;
    u8 pad20[0x14];
    u8 matrix[0x24];
    u8 pad58[0x0C];
    struct Unk437E8Part *free;
    struct Unk437E8Part *active;
    void *unk6C;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    s16 unk76;
    s16 unk78;
    s16 unk7A;
    u8 pad7C[0x10];
    u8 unk8C;
    u8 pad8D[9];
    s16 unk96;
    s16 unk98;
    s16 unk9A;
    u8 pad9C[8];
    u16 count;
};

extern s32 D_8009FA50[];
extern s32 D_8009FAB0;

extern struct Unk437E8Transform *func_8003D9F8(void *model, s32 flags, s32 part,
                                               s32 count, void *sub, void *matrix);
extern void func_80042864(struct Unk437E8Part *object, s32 amount);
extern void func_800428FC(struct Unk437E8 *node, struct Unk437E8Part *cur,
                          s32 a2, s32 a3, s32 a5);
extern void func_800427A8(struct Unk437E8 *owner, struct Unk437E8Part *previous,
                          struct Unk437E8Part *node);
extern s32 func_80066F78(s32 *pos, s32 arg1, s32 arg2, s32 arg3);

s32 func_800437E8(struct Unk437E8 *node, s32 arg)
{
    register struct Unk437E8 *nd __asm__("$17");
    register struct Unk437E8Part *cur __asm__("$16");
    s32 scale;
    register s32 i __asm__("$19");
    s32 v96;
    register struct Unk437E8Part *prev __asm__("$21");
    register struct Unk437E8Part *next __asm__("$22");
    register struct Unk437E8Transform *transform __asm__("$23");
    s32 v9A;
    s32 frame;
    s32 unk72;
    s32 v98;
    s32 vec[3];
    register s32 h __asm__("$2");
    s32 v;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 p;
    s32 q;
    s32 r;
    s32 result;

    nd = node;
    transform = func_8003D9F8(nd->sub != 0 ? nd->sub->model : 0,
                              nd->unk1A, nd->unk0E, 0, nd->sub, nd->matrix);
    cur = nd->active;
    if (cur == 0) {
        __asm__ volatile("" : "+r"(cur));
        if (nd->unk07 == -1) {
            nd->unk0F |= 1;
            return 0;
        }
    }
    result = 0;
    scale = (arg << 12) / 300;
    v96 = (nd->unk96 * arg << 2) / 300;
    prev = 0;
    if (!(nd->unk8C & 1)) {
        v98 = (nd->unk98 * arg << 2) / 300;
        v9A = (nd->unk9A * arg << 2) / 300;
    }
    __asm__ volatile("lui %0, %%hi(D_8009FA50)\n\tlw %0, %%lo(D_8009FA50)(%0)" : "=r"(frame));
    unk72 = nd->unk72;
    i = 0;
    while (cur != 0) {
        dx = (nd->unk76 - cur->unk0C) * cur->unk12;
        dy = (nd->unk78 - cur->unk0E) * cur->unk12;
        dz = (nd->unk7A - cur->unk10) * cur->unk12;
        p = ((dx << 4) >> 16) * scale;
        q = (((dy << 4) >> 16) + unk72) * scale;
        r = ((dz << 4) >> 16) * scale;
        cur->unk0C += p >> 12;
        cur->unk04 += ((s16)cur->unk0C * scale) >> 12;
        cur->unk0E += q >> 12;
        cur->unk06 += ((s16)cur->unk0E * scale) >> 12;
        cur->unk10 += r >> 12;
        cur->unk08 += ((s16)cur->unk10 * scale) >> 12;
        next = cur->next;

        if (((frame ^ i) & 3) == 0) {
            if (nd->unk8C & 1) {
                s32 amt;
                amt = 0;
                if (!(cur->unk14 & 1))
                    amt = v96;
                func_80042864(cur, amt);
            } else if (nd->unk8C & 2) {
                s32 a2;
                s32 a3;
                a2 = v96;
                if (cur->unk14 & 8)
                    a2 = -v96;
                a3 = v98;
                if (cur->unk14 & 0x10)
                    a3 = -v98;
                func_800428FC(nd, cur, a2, a3,
                              (cur->unk14 & 0x20) ? -v9A : v9A);
            } else {
                s32 a2;
                s32 a3;
                a2 = 0;
                if (!(cur->unk14 & 1))
                    a2 = v96;
                a3 = 0;
                if (!(cur->unk14 & 2))
                    a3 = v98;
                func_800428FC(nd, cur, a2, a3,
                              (cur->unk14 & 4) ? 0 : v9A);
            }
        }
        if (((frame ^ i) & 7) == 0 && !(nd->unk1A & 2) && cur->unk0E < 0) {
            vec[0] = (cur->unk04 << 4) + transform->pos.x;
            vec[1] = (cur->unk06 << 4) + transform->pos.y;
            vec[2] = (cur->unk08 << 4) + transform->pos.z;
            if (vec[1] < func_80066F78(vec, 0, 0, 0))
                goto remove;
        }
        h = cur->unk18 & 0x1FFF;
        v = h;
        if (h != 0 && ((h -= arg, v = h, ({ __asm__ volatile("" : "+r"(h)); h << 16; })) <= 0)) {
        remove:
            func_800427A8(nd, prev, cur);
            D_8009FAB0--;
        } else {
            prev = cur;
            cur->unk18 = v | (cur->unk18 & 0xE000);
        }
        cur = next;
        i++;
    }
    return 0;
}
