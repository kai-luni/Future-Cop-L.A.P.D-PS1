#include "game/types.h"
typedef struct {
    u16 vx;
    u16 vy;
} DVECTOR;

typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u32 unk04;
    s16 x0, y0;
    u32 unk0C;
    s16 x1, y1;
    u32 unk14;
    s16 x2, y2;
    u32 unk1C;
    s16 x3, y3;
    u8 pad24[0x34 - 0x24];
} Prim;
extern Prim *D_8009F9F0;
extern Prim *D_8009FA28;

void func_8006F21C(DVECTOR v0, DVECTOR v1, DVECTOR v2, DVECTOR v3, s32 dx, s32 dy_in, u32 unk04, u32 unk1C, u16 tag_in) {
    register s32 dy __asm__("$8") = dy_in;
    register u16 tag __asm__("$7") = tag_in;
    Prim *prim;

    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    prim = D_8009F9F0;
    goto L_done;
L_alloc:
    prim = D_8009F9F0;
    D_8009F9F0 = (Prim *)((u8 *)prim + 0x34);
L_done:

    prim->hdr.bytes[3] = 8;
    prim->unk04 = unk04;
    prim->unk1C = unk1C;
    prim->unk14 = 0;
    prim->unk0C = 0;

    prim->x2 = v0.vx;
    prim->y2 = v0.vy;
    {
        register u32 mask __asm__("$5") = 0x00FFFFFF;
        register s32 v __asm__("$2");
        s32 y3;
        v = v0.vx + dx; prim->x0 = v;
        v = v0.vy + dy; prim->y0 = v;
        v = v2.vx + dx; prim->x1 = v;
        v = v2.vy + dy; prim->y1 = v;
        v = v2.vx;      prim->x3 = v;
        v = (s16)tag;
        y3 = v2.vy;
        prim->hdr.word = (prim->hdr.word & 0xFF000000) | (v & mask);
        prim->y3 = y3;
    }
}
