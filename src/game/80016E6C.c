#include "game/types.h"

typedef struct { u32 tag, color, xy0, uv0, xy1, uv1, xy2, uv2, xy3, uv3, rest[3]; } Packet;
typedef struct { u32 xy0, xy1, xy2, xy3, tag; u8 pad[1], code, pad2[2]; u32 data[4]; } Input;
typedef struct { u32 flags; u8 a[4], b[4]; } Indices;
typedef struct { u32 a, b; } Entry;
extern Packet *D_8009F9F0, *D_8009FA28;
extern void func_80016C88(Entry *, Entry *, void *, void *);
void func_80016E6C(Indices *idx, Input *in, Entry *base1, Entry *base0)
{
    Packet *p;
    u32 uv;
    void *data;
    if (D_8009F9F0 < D_8009FA28) goto alloc;
    __asm__ volatile ("");
    p = D_8009F9F0;
    goto next;
alloc:
    p = D_8009F9F0;
    D_8009F9F0 = p + 1;
next:
    p->tag = (p->tag & 0xff000000) | (in->tag & 0xffffff);
    uv = in->data[3];
    p->xy0 = in->xy0;
    data = &in->data[0];
    p->xy1 = in->xy1;
    p->xy2 = in->xy2;
    func_80016C88(base0 + idx->a[0], base1 + idx->b[0], data, &p->uv0);
    func_80016C88(base0 + idx->a[1], base1 + idx->b[1], data, &p->uv1);
    *(u16 *)((u8 *)&p->uv0 + 2) = uv >> 16;
    *(u16 *)((u8 *)&p->uv1 + 2) = uv;
    switch ((idx->flags >> 8) & 15) {
    case 3:
        ((u8 *)p)[3] = 7;
        p->color = ((u32)in->code << 24) | 0x24808080;
        func_80016C88(base0 + idx->a[2], base1 + idx->b[2], data, &p->uv2);
        break;
    case 4:
        p->xy3 = in->xy3;
        ((u8 *)p)[3] = 9;
        p->color = ((u32)in->code << 24) | 0x2c808080;
        func_80016C88(base0 + idx->a[3], base1 + idx->b[3], data, &p->uv2);
        func_80016C88(base0 + idx->a[2], base1 + idx->b[2], data, &p->uv3);
        break;
    }
}
