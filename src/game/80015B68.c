#include "game/types.h"

typedef struct {
    union {
        s32 xy;
        struct {
            s16 x;
            s16 y;
        } s;
    } u;
    s16 z;
    s16 pad;
} Vertex;

typedef struct {
    union {
        u32 flags;
        struct {
            u16 h0;
            u16 h1;
        } h;
    } u0;
    union {
        u32 w4;
        struct {
            u8 b4;
            u8 b5;
            u8 b6;
            u8 b7;
        } b;
    } u4;
    union {
        u32 w8;
        struct {
            u8 b8;
            u8 b9;
            u8 b10;
            u8 b11;
        } b;
    } u8;
} Part;

typedef struct {
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    union {
        s32 xy3;
        struct {
            s16 x3;
            s16 y3;
        } s;
    } u;
    s32 z;
} Work;

typedef struct {
    s32 unk0;
    s16 data[1];
} Arg2;

extern Vertex *D_8009F578;
extern u8 *D_8009F57C;

extern s32 func_80015AF4(s32 origin, s32 first, s32 second, void *out);
extern s32 func_80015830(Part *part, Work *work, Arg2 *arg2, s32 arg3);
extern void func_8001484C(Vertex *vtx, u32 a1, u32 a2, u16 a3, s32 a4, s32 a5, s32 a6);

s32 func_80015B68(Part *part, Work *work, Arg2 *arg2, s32 arg3)
{
    Vertex *vtx = D_8009F578;
    u32 i0, i1, i2, i3;
    u8 flags;
    s32 vx, vy, vz, dx, dy;

    switch ((part->u0.flags >> 8) & 0xF) {
    case 3:
        i0 = part->u4.b.b4;
        i1 = part->u4.b.b5;
        i2 = part->u4.b.b6;
        if (((flags = D_8009F57C[i0] | D_8009F57C[i1] | D_8009F57C[i2]) & 1) || flags != 0xC0) {
            return 0;
        }
        if (func_80015AF4(vtx[i0].u.xy, vtx[i1].u.xy, vtx[i2].u.xy, work)) {
            return 0;
        }
        vz = vtx[i0].z + vtx[i1].z + vtx[i2].z;
        vz += vtx[i0].z + (vz << 2);
        work->z = (vz >> 4) - arg3;
        return 1;

    case 4:
        i0 = part->u4.b.b4;
        i1 = part->u4.b.b5;
        i2 = part->u4.b.b6;
        i3 = part->u4.b.b7;
        if (((flags = D_8009F57C[i0] | D_8009F57C[i1] | D_8009F57C[i2] | D_8009F57C[i3]) & 1) || flags != 0xC0) {
            return 0;
        }
        if (func_80015AF4(vtx[i0].u.xy, vtx[i1].u.xy, vtx[i3].u.xy, work)) {
            return 0;
        }
        work->u.xy3 = vtx[i2].u.xy;
        work->z = ((vtx[i0].z + vtx[i1].z + vtx[i2].z + vtx[i3].z) >> 2) - arg3;
        return 1;

    case 5:
    case 6:
        if (D_8009F57C[part->u4.b.b4] & 1) {
            return 0;
        }
        vtx += part->u4.b.b4;
        vz = vtx->z;
        vx = vtx->u.s.x;
        vy = vtx->u.s.y;
        work->z = vz - arg3;
        dx = (arg2->data[part->u4.b.b6] << 9) / vz;
        dy = dx;
        if (((part->u0.flags >> 8) & 0xF) == 6) {
            dy = arg2->data[part->u4.b.b7];
            dy = (dy << 9) / vz;
        }
        work->x0 = work->x2 = vx - dx;
        work->x1 = work->u.s.x3 = vx + dx;
        work->y0 = work->y1 = vy - dy;
        work->u.s.y3 = work->y2 = vy + dy;
        return 1;

    case 7:
        return func_80015830(part, work, arg2, arg3);

    case 8:
        if (D_8009F57C[part->u4.b.b4] & 1) {
            return 0;
        }
        func_8001484C(&vtx[part->u4.b.b4], part->u4.w4, part->u8.w8, part->u0.h.h1, arg2->data[part->u8.b.b8], arg3, 0);
        break;
    }
    return 0;
}
