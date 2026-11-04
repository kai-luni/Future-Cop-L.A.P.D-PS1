#include "game/types.h"

typedef struct {
    u16 tpage;
    u8 u;
    u8 pad03;
    u8 v;
    u8 pad05;
    union {
        u16 w;
        u8 w8;
    } dw;
    union {
        u16 h;
        u8 h8;
    } dh;
    u16 clut;
} Entry82B0C;

typedef struct {
    u16 type;
    u16 count;
    u32 magic;
    Entry82B0C entries[1];
} Table82B0C;

typedef struct {
    union {
        u32 word;
        u8 bytes[4];
    } hdr;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x0;
    s16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    s16 x1;
    s16 y1;
    u8 u1;
    u8 v1;
    u16 tpage;
    s16 x2;
    s16 y2;
    u8 u2;
    u8 v2;
    u8 pad1E[2];
    s16 x3;
    s16 y3;
    u8 u3;
    u8 v3;
    u8 pad26[0x34 - 0x26];
} Prim82B0C;

extern Table82B0C *D_8009F45C;
extern Prim82B0C *D_8009F9F0;
extern Prim82B0C *D_8009FA28;

Prim82B0C *func_80082B0C(register s32 idx, s32 x, s32 y, s32 w, s32 h, s32 tag, s32 r, s32 g, s32 b, s8 flagX, s8 flagY)
{
    Table82B0C *table;
    Entry82B0C *e;
    Prim82B0C *prim;
    register s32 x2 __asm__("$11");
    register s32 y2 __asm__("$4");
    u16 tpage;

    prim = 0;
    table = D_8009F45C;
    if (table != 0 && idx >= 0 && idx < table->count) {
        e = &table->entries[idx];
        if (w == -1) {
            w = e->dw.w;
        }
        if (h == -1) {
            h = e->dh.h;
        }

        switch (flagX) {
        case 4:
            x -= w / 2;
            x2 = x + w;
            break;
        case 2:
            x2 = x;
            x = x2 - w;
            break;
        default:
            x2 = x + w;
            break;
        }

        switch (flagY) {
        case 0x20:
            y -= h / 2;
            y2 = y + h;
            break;
        case 0x10:
            y2 = y;
            y = y2 - h;
            break;
        default:
            y2 = y + h;
            break;
        }

        if (D_8009F9F0 < D_8009FA28) goto L_alloc;
        __asm__("");
        prim = D_8009F9F0;
        goto L_done;
    L_alloc:
        prim = D_8009F9F0;
        D_8009F9F0 = (Prim82B0C *)((u8 *)prim + 0x34);
    L_done:

        prim->hdr.bytes[3] = 9;
        prim->code = 0x2C;
        prim->r = r;
        prim->g = g;
        prim->b = b;
        prim->x0 = x;
        prim->y0 = y;
        prim->x1 = x2;
        prim->y1 = y;
        prim->x2 = x;
        prim->y2 = y2;
        prim->x3 = x2;
        prim->y3 = y2;
        prim->u0 = e->u;
        prim->v0 = e->v;
        prim->u1 = e->u + e->dw.w8;
        prim->v1 = e->v;
        prim->u2 = e->u;
        prim->v2 = e->v + e->dh.h8;
        prim->u3 = e->u + e->dw.w8;
        prim->v3 = e->v + e->dh.h8;
        prim->clut = e->clut;
        tpage = e->tpage;
        prim->hdr.word = (prim->hdr.word & 0xFF000000) | (tag & 0xFFFFFF);
        prim->tpage = tpage;
    }
    return prim;
}
