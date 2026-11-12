#include "game/types.h"

typedef struct {
    u32 w[13];
} Prim17;

typedef struct { u8 flags, pad; u16 off; u8 pad4[4]; u8 index[4]; } Part17;
typedef struct { u32 xy[4]; u32 link; u8 mode, pad; u16 value; } Work17;

extern Prim17 *D_8009F9F0;
extern Prim17 *D_8009FA28;
extern void func_80015F14(void *a0, void *a1, void *a2);

void func_80015F38(Part17 *part, Work17 *work, u8 *texture, u8 *light)
{
    Prim17 *prim;
    u32 *texw;
    u32 tmp, fl;
    register u32 t0 __asm__("$4");
    register u32 t1 __asm__("$5");
    register u32 t2 __asm__("$6");
    u32 c1, c2, lo, hs, v;
    s32 n;

    texw = (u32 *)(texture + part->off);
    fl = part->flags;
    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    prim = D_8009F9F0;
    goto L_done;
L_alloc:
    prim = D_8009F9F0;
    D_8009F9F0 = (Prim17 *)((u8 *)prim + 0x34);
L_done:
    tmp = (fl & 2) << 24;
    prim->w[0] = (prim->w[0] & 0xFF000000) | (work->link & 0xFFFFFF);
    c1 = texw[0];
    c1 >>= 8;
    if (work->mode)
        tmp |= (c1 & 0xff) | ((c1 & 0xfcfc00) >> 1);
    else
        tmp |= c1;
    t0 = work->xy[0]; t1 = work->xy[1]; t2 = work->xy[2];
    switch (fl >> 4) {
    case 0:
    case 4:
        prim->w[2] = t0;
        prim->w[3] = t1;
        prim->w[4] = t2;
        if (fl & 0x40) {
            prim->w[5] = work->xy[3];
            ((u8 *)prim)[3] = 5;
            tmp |= 0x28000000;
        } else {
            ((u8 *)prim)[3] = 4;
            tmp |= 0x20000000;
        }
        if (fl & 1)
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
        else
            prim->w[1] = tmp;
        break;
    case 2:
    case 6:
        prim->w[2] = t0;
        prim->w[4] = t1;
        prim->w[6] = t2;
        if (fl & 0x40) {
            n = 3;
            prim->w[8] = work->xy[3];
            ((u8 *)prim)[3] = 8;
            tmp |= 0x38000000;
        } else {
            n = 2;
            ((u8 *)prim)[3] = 6;
            tmp |= 0x30000000;
        }
        if (fl & 1) {
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
            func_80015F14(light + part->index[1] * 8, &tmp, &prim->w[3]);
            func_80015F14(light + part->index[n] * 8, &tmp, &prim->w[5]);
            if (n == 3)
                func_80015F14(light + part->index[2] * 8, &tmp, &prim->w[7]);
        } else {
            prim->w[7] = tmp;
            prim->w[5] = tmp;
            prim->w[3] = tmp;
            prim->w[1] = tmp;
        }
        break;
    case 1:
        prim->w[2] = t0;
        prim->w[4] = t1;
        prim->w[6] = t2;
        ((u8 *)prim)[3] = 7;
        tmp |= 0x24000000;
        if (fl & 1)
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
        else
            prim->w[1] = tmp;
        c2 = texw[3];
        c1 = texw[1];
        lo = ((c1 & 0xFFFF) + work->value) | (c2 & 0xFFFF0000);
        prim->w[3] = lo;
        { u32 t; t = (c1 >> 16) + work->value; prim->w[5] = t | (c2 << 16); }
        *(s16 *)((u8 *)prim + 0x1C) = *(u16 *)&texw[2] + work->value;
        break;
    case 5:
        prim->w[2] = t0;
        prim->w[4] = t1;
        prim->w[6] = t2;
        prim->w[8] = work->xy[3];
        ((u8 *)prim)[3] = 9;
        tmp |= 0x2C000000;
        if (fl & 1)
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
        else
            prim->w[1] = tmp;
        c2 = texw[3];
        c1 = texw[1];
        lo = ((c1 & 0xFFFF) + work->value) | (c2 & 0xFFFF0000);
        prim->w[3] = lo;
        { u32 t; t = (c1 >> 16) + work->value; prim->w[5] = t | (c2 << 16); }
        c1 = texw[2];
        *(s16 *)((u8 *)prim + 0x24) = work->value + c1;
        *(s16 *)((u8 *)prim + 0x1C) = work->value + (c1 >> 16);
        break;
    case 3:
        prim->w[2] = t0;
        prim->w[5] = t1;
        prim->w[8] = t2;
        ((u8 *)prim)[3] = 9;
        tmp |= 0x34000000;
        if (fl & 1) {
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
            func_80015F14(light + part->index[1] * 8, &tmp, &prim->w[4]);
            func_80015F14(light + part->index[2] * 8, &tmp, &prim->w[7]);
        } else {
            prim->w[7] = tmp;
            prim->w[4] = tmp;
            prim->w[1] = tmp;
        }
        c2 = texw[3];
        c1 = texw[1];
        lo = ((c1 & 0xFFFF) + work->value) | (c2 & 0xFFFF0000);
        prim->w[3] = lo;
        { u32 t; t = (c1 >> 16) + work->value; prim->w[6] = t | (c2 << 16); }
        *(s16 *)((u8 *)prim + 0x24) = *(u16 *)&texw[2] + work->value;
        break;
    case 7:
        prim->w[2] = t0;
        prim->w[5] = t1;
        prim->w[8] = t2;
        prim->w[11] = work->xy[3];
        ((u8 *)prim)[3] = 12;
        tmp |= 0x3C000000;
        if (fl & 1) {
            func_80015F14(light + part->index[0] * 8, &tmp, &prim->w[1]);
            func_80015F14(light + part->index[1] * 8, &tmp, &prim->w[4]);
            func_80015F14(light + part->index[3] * 8, &tmp, &prim->w[7]);
            func_80015F14(light + part->index[2] * 8, &tmp, &prim->w[10]);
        } else {
            prim->w[10] = tmp;
            prim->w[7] = tmp;
            prim->w[4] = tmp;
            prim->w[1] = tmp;
        }
        c2 = texw[3];
        c1 = texw[1];
        lo = ((c1 & 0xFFFF) + work->value) | (c2 & 0xFFFF0000);
        prim->w[3] = lo;
        { u32 t; t = (c1 >> 16) + work->value; prim->w[6] = t | (c2 << 16); }
        c1 = texw[2];
        *(s16 *)((u8 *)prim + 0x30) = work->value + c1;
        *(s16 *)((u8 *)prim + 0x24) = work->value + (c1 >> 16);
        break;
    }
}
