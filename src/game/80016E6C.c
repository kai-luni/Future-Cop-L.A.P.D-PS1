// Categories: Graphics > Model Renderer > Polygons | Graphics > GPU Primitives
#include "game/types.h"

typedef struct { u32 tag, color, xy0, uv0, xy1, uv1, xy2, uv2, xy3, uv3, rest[3]; } Packet;
typedef struct { u32 xy0, xy1, xy2, xy3, tag; u8 pad[1], code, pad2[2]; u32 data[4]; } Input;
typedef struct { u32 flags; u8 a[4], b[4]; } Indices;
typedef struct { u32 a, b; } Entry;
extern Packet *gPrimPoolNext __asm__("D_8009F9F0");
extern Packet *gPrimPoolEnd __asm__("D_8009FA28");
extern void func_80016C88(Entry *, Entry *, void *, void *);
/**
 * @brief Emits one textured polygon packet (triangle or quad) from an index
 *        record.
 *
 * @param idx    Index record: flags (bit 8-11 = face type) and four index bytes
 *               per corner (two arrays, a[0..3] and b[0..3]).
 * @param in     Vertex/UV block: three XY words, the tag, the four UV words and
 *               the colour mode byte.
 * @param base1, base0 The two base tables the indices are relative to (passed to
 *               func_80016C88, which resolves UVs).
 * @return Nothing; the packet comes from the D_8009F9F0 pool.
 *
 * @par Confirmed
 * The tag is copied, the three XY words and the first two UV pairs are filled via
 * func_80016C88, then the face type decides: 3 = triangle (length 7, colour
 * 0x24808080 | code << 24), 4 = quad (length 9, colour 0x2C808080 | code << 24,
 * fourth XY word and fourth UV pair). Any other type emits the packet with only
 * the three shared vertices - the polygon keeps whatever length was last set,
 * which is why the type check sits after the stores.
 *
 * @par Callers
 * func_80016C88's callers, i.e. the mesh emitters of func_8006DC24.
 *
 * @par Environment path
 * Turns one model part into a textured polygon packet: the tag and the first
 * three XY words come from the input block, func_80016C88 resolves each UV
 * pair from the two base tables, and the face type in bits 8..11 of the flags
 * word selects triangle (packet length 7, colour 0x24808080) or quad (length
 * 9, colour 0x2C808080, fourth corner and fourth UV pair). It is called only
 * from the two per-part traversals func_80016A64 (line 137) and func_80017058
 * (line 128), i.e. from the model layer that func_8006E988 walks through
 * func_8006DC24 inside one environment view. It takes its storage from the
 * D_8009F9F0 packet pool that the whole render path shares, so it is a leaf
 * of the environment model layer, not of the region drawers.
 */
void func_80016E6C(Indices *idx, Input *in, Entry *base1, Entry *base0)
{
    Packet *p;
    u32 uv;
    void *data;
    if (gPrimPoolNext < gPrimPoolEnd) goto alloc;
    __asm__ volatile ("");
    p = gPrimPoolNext;
    goto next;
alloc:
    p = gPrimPoolNext;
    gPrimPoolNext = p + 1;
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
