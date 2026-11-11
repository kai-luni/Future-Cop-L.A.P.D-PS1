#include "game/types.h"

struct V16 { s16 x, y, z, pad; };
struct V32 { s32 x, y, z; };
struct Frame { s16 x, pad2, z, dx, pad8, dz; s16 padc[2]; };
struct Model { u8 pad[0x34]; struct Frame *frames; };
struct Obj { struct Model *model; u8 matrix[0x24]; struct V32 pos; u8 pad34[0x14]; s16 frame; u8 flags; };
struct Camera { u8 pad[0x28]; u8 matrix[0x54]; struct V32 pos; };
struct Packet { u32 tag, c0, p0, uv0, c1, p1, uv1, c2, p2; u16 uv2; u8 pad[14]; };
extern struct Camera *D_8009F9E4;
extern struct Packet *D_8009F9F0, *D_8009FA28;
extern u32 D_8009FAA0;
extern s32 func_80066F78(struct V32 *, s32, s32, void *);
extern void func_80010750(void *);
extern void func_80010234(struct V16 *, struct V32 *, s32);
extern void func_80010820(void *);
extern s32 func_8001047C(struct V16 *, struct V16 *, u8 *, s32);

void func_800164F4(struct Obj *obj)
{
    struct V16 p[5], q[5];
    struct V32 transformed[5];
    u8 codes[8], extra[8];
    register struct Frame *frame __asm__("$3");
    volatile struct Packet *packet;
    register struct V16 *cur __asm__("$16");
    register struct V16 *next __asm__("$7");
    struct V16 *mid;
    register s32 n __asm__("$15");
    s32 ground, shade, dx, dz, x, z, t, i, color, dark;
    void *optional;

    optional = 0;
    if (obj->flags & 0x20) optional = extra;
    ground = func_80066F78(&obj->pos, 0, 0, optional);
    {
        register s32 distance __asm__("$2");
        distance = (s16)((obj->pos.y - ground) >> 4);
        if (distance < -16) return;
        shade = 0x60 - (((distance >> 1) * 3) >> 6);
    }
    if (shade < 0) return;
    {
        register s32 frame_offset __asm__("$2");
        frame_offset = obj->frame << 4;
        __asm__("" : "+r" (frame_offset));
        frame_offset += 8;
        frame = (struct Frame *)((u8 *)obj->model->frames + frame_offset);
    }
    dx = frame->dx; dz = frame->dz; x = frame->x; z = frame->z;
    dx += dx / 4; dz += dz / 4;
    p[0].x = x - dx; p[0].y = 0; p[0].z = z;
    p[1].x = x; p[1].y = 0; p[1].z = z - dz;
    p[2].x = x + dx; p[2].y = 0; p[2].z = z;
    p[3].x = x; p[3].y = 0; p[3].z = z + dz;
    p[4].x = x; p[4].y = 0; p[4].z = z;
    func_80010750(obj->matrix);
    func_80010234(p, transformed, 5);
    {
        struct V32 *src = transformed;
        register struct V16 *dst __asm__("$4");
        register s32 j __asm__("$15");
        j = 0;
        dst = p;
        do {
            dst->x = (src->x - D_8009F9E4->pos.x) >> 4;
            dst->y = (ground - D_8009F9E4->pos.y) >> 4;
            dst->z = (src->z - D_8009F9E4->pos.z) >> 4;
            src++; dst++;
        } while (++j < 5);
    }
    func_80010820(D_8009F9E4->matrix);
    if (func_8001047C(p, p, codes, 5) & 0x1c66000) return;
    t = codes[0] | codes[1] | codes[2] | codes[3];
    if (t & 1) return;
    if (t != 0xc0) return;
    color = shade | (shade << 8) | (shade << 16) | 0x36000000;
    shade >>= 8;
    dark = shade | (shade << 8) | (shade << 16) | 0xff000000;
    {
    s32 cx = p[4].x, ax = p[0].x, bx = p[1].x;
    s32 cy = p[4].y, ay = p[0].y, by = p[1].y;
    s32 bcx = cx << 1;
    s32 bcy = cy << 1;
    q[0].x = (((ax + bx - bcx) * 3) >> 2) + cx;
    q[0].y = (((ay + by - bcy) * 3) >> 2) + cy;
    q[0].z = (p[0].z + p[1].z) >> 1;
    q[1].x = (((p[1].x + p[2].x - bcx) * 3) >> 2) + cx;
    q[1].y = (((p[1].y + p[2].y - bcy) * 3) >> 2) + cy;
    q[1].z = (p[1].z + p[2].z) >> 1;
    q[2].x = (((p[2].x + p[3].x - bcx) * 3) >> 2) + cx;
    q[2].y = (((p[2].y + p[3].y - bcy) * 3) >> 2) + cy;
    q[2].z = (p[2].z + p[3].z) >> 1;
    q[3].x = (((p[3].x + p[0].x - bcx) * 3) >> 2) + cx;
    q[3].y = (((p[3].y + p[0].y - bcy) * 3) >> 2) + cy;
    q[3].z = (p[3].z + p[0].z) >> 1;
    }
    cur = p; mid = q;
    for (n = 3; n >= 0; n--) {
        s32 center, depth;
        u32 tail;
        next = n ? cur + 1 : cur - 3;
        packet = D_8009F9F0 >= D_8009FA28 ? D_8009F9F0 : D_8009F9F0++;
        ((u8 *)packet)[3] = 9;
        center = p[4].z;
        depth = center + cur->z + mid->z;
        packet->c0 = color; packet->c2 = dark; packet->c1 = dark;
        depth += center + (depth << 2);
        {
            register u32 masked_depth __asm__("$2");
            masked_depth = ((depth >> 4) - 0x200) & 0xffffff;
            packet->tag = (packet->tag & 0xff000000) | masked_depth;
        }
        packet->p0 = *(u32 *)&p[4];
        packet->p1 = *(u32 *)cur; tail = *(u32 *)mid; packet->uv0 = 0; packet->p2 = tail;
        __asm__("" : : : "memory");
        packet->uv2 = 1; ((struct Packet *)packet)->uv1 = (D_8009FAA0 << 16) | 0x101;
        __asm__ ("" : : : "memory");
        packet = D_8009F9F0 >= D_8009FA28 ? D_8009F9F0 : D_8009F9F0++;
        ((u8 *)packet)[3] = 9;
        center = p[4].z;
        depth = center + mid->z + next->z;
        packet->c0 = color; packet->c2 = dark; packet->c1 = dark;
        depth += center + (depth << 2);
        packet->tag = (packet->tag & 0xff000000) | (((depth >> 4) - 0x200) & 0xffffff);
        packet->p0 = *(u32 *)&p[4]; packet->p1 = *(u32 *)mid; tail = *(u32 *)next; packet->uv0 = 0; packet->p2 = tail;
        __asm__("" : : : "memory");
        packet->uv2 = 1; ((struct Packet *)packet)->uv1 = (D_8009FAA0 << 16) | 0x101;
        mid++; cur++;
    }
}
