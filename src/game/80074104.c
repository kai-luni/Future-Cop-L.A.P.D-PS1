#include "game/types.h"

#define W(p,o) (*(s32 *)((u8 *)(p)+(o)))
#define H(p,o) (*(s16 *)((u8 *)(p)+(o)))
#define UH(p,o) (*(u16 *)((u8 *)(p)+(o)))
#define B(p,o) (*(s8 *)((u8 *)(p)+(o)))
#define UB(p,o) (*(u8 *)((u8 *)(p)+(o)))
#define PTR(p,o) (*(u8 **)((u8 *)(p)+(o)))

typedef struct { s16 x, y, z, pad; } Point;
typedef struct { s32 x, y, z; } Vec;
typedef struct { s32 e[9]; } Matrix;
typedef struct { s32 page, x, y, w, h; } Texture;
extern Texture D_8009B234[];
extern u8 *D_8009F9F0, *D_8009FA28;
extern u8 D_800A3548[];
extern Point D_800A6418[];
extern u8 D_800AB388[];
extern s16 D_800A7840[], D_800A7898[];
extern u8 *D_8009F9E4;
extern u8 *func_8003D9F8();
extern u8 *func_8003D9C4();
extern s32 func_8001A100();
extern void func_8001A81C();
extern void func_80010820();
extern void func_8001006C();
extern void func_80010750();
extern void func_8001047C();

void func_80074104(u8 *input, s32 *pos)
{
    register u8 *obj asm("$23") = input;
    register u8 *owner asm("$19");
    register s32 count asm("$20");
    register u8 *base asm("$21");
    register s32 next asm("$22");
    struct { Vec v; s32 pad0; Matrix mat; Vec projected, trans; s32 pad1; u32 texpage0, texpage1, uv0, uv1; u16 uv2; u8 pad2[6]; u16 uv3; } frame;
    u8 *node, *m, *p, *cursor;
    register u8 *prim asm("$6");
    register s32 z asm("$2");
    register s32 *pp asm("$25");
    Texture *tex;
    register Texture *textures asm("$15");
    s32 previous, i, a, b, width, q, t, depth, mat0, original_dx;
    register s32 offset asm("$10");
    u32 ux, uy, uw, uh;
    s16 *dx, *dy;
    register Point *out asm("$4");
    register Point *cur asm("$7");
    register Point *following asm("$4");
    Point *points;
    while (obj) {
        if (obj[0xF] & 8) goto advance;
        next = 1;
        count = 0;
        previous = 0;
        node = PTR(obj, 0x68);
        owner = obj;
        if (!node) goto advance;
        p = PTR(obj, 0x1c);
        base = func_8003D9F8(p ? W(p, 0x34) : 0, UH(obj, 0x1a), B(obj, 0xe), 1, p, obj + 0x34);
        m = func_8003D9C4(obj);
        if (!(UB(obj,0x10) & 1)) {
            if (m) {
                __asm__("" : "=r"(pp) : "0"(pos));
                frame.v.x = W(m,0x24) - pp[0];
                frame.v.y = W(m,0x28) - pp[1];
                z = W(m,0x2c);
            } else {
                __asm__("" : "=r"(pp) : "0"(pos));
                frame.v.x = W(base,0x24) - pp[0];
                frame.v.y = W(base,0x28) - pp[1];
                z = W(base,0x2c);
            }
            frame.v.z = z - pp[2];
            if (func_8001A100(&frame.v) > 0x1ffff) goto advance;
        }
        if (!(UH(owner,0x8e) & 0x2000)) {
            UH(D_800A3548,0x10) = UH(owner,0x12);
            UB(D_800A3548,0x15) = UB(owner,0x73);
            UB(D_800A3548,0x16) = UB(owner,0x76);
            PTR(D_800A3548,0) = node;
            UB(D_800A3548,0x17) = UB(owner,0x79);
            if (UH(owner,0x1a) & 0x100) {
                func_8001A81C(m, owner+0x58, &frame.trans);
                D_800A6418[count].x = frame.trans.x >> 4;
                D_800A6418[count].y = frame.trans.y >> 4;
                D_800A6418[count].z = frame.trans.z >> 4;
            } else if (UH(owner,0x8e) & 0x10) {
                frame.trans = *(Vec *)(owner+0x28);
                D_800A6418[count].x = (frame.trans.x-W(D_8009F9E4,0x7c)) >> 4;
                D_800A6418[count].y = (frame.trans.y-W(D_8009F9E4,0x80)) >> 4;
                D_800A6418[count].z = (frame.trans.z-W(D_8009F9E4,0x84)) >> 4;
            } else {
                func_80010820(m);
                func_8001006C(owner+0x28, &frame.trans, 1);
                frame.trans.x = W(owner,0x28)+W(m,0x24);
                frame.trans.y = W(owner,0x2c)+W(m,0x28);
                frame.trans.z = W(owner,0x30)+W(m,0x2c);
                D_800A6418[count].x = (frame.trans.x-W(D_8009F9E4,0x7c)) >> 4;
                D_800A6418[count].y = (frame.trans.y-W(D_8009F9E4,0x80)) >> 4;
                D_800A6418[count].z = (frame.trans.z-W(D_8009F9E4,0x84)) >> 4;
            }
            count++;
        }
        __asm__("" : "+r"(count));
        out = &D_800A6418[count];
        while (node) {
            if (UH(node,0x18) & 1) break;
            if (UH(owner,0x1a) & 0x100) {
                out->x = W(node,4) >> 4;
                out->y = W(node,8) >> 4;
                out->z = W(node,12) >> 4;
            } else {
                out->x = (W(node,4) - W(D_8009F9E4,0x7c)) >> 4;
                out->y = (W(node,8) - W(D_8009F9E4,0x80)) >> 4;
                out->z = (W(node,12) - W(D_8009F9E4,0x84)) >> 4;
            }
            ++out;
            node = PTR(node,0);
            count++;
        }
        if (count < 2) goto advance;
        __asm__("" : "=r"(points) : "0"(D_800A6418), "r"(count));
        if (UH(owner,0x1a) & 0x100) {
            frame.v.x = W(m,0x24)-W(D_8009F9E4,0x7c);
            frame.v.y = W(m,0x28)-W(D_8009F9E4,0x80);
            frame.v.z = W(m,0x2c)-W(D_8009F9E4,0x84);
            func_80010820(D_8009F9E4+0x28);
            func_8001006C(m, &frame.mat, 3);
        } else {
            frame.v.x = W(base,0x24)-W(D_8009F9E4,0x7c);
            frame.v.y = W(base,0x28)-W(D_8009F9E4,0x80);
            frame.v.z = W(base,0x2c)-W(D_8009F9E4,0x84);
            mat0 = W(D_8009F9E4,0x28);
            frame.mat.e[1] = W(D_8009F9E4,0x2c);
            frame.mat.e[2] = W(D_8009F9E4,0x30);
            frame.mat.e[3] = W(D_8009F9E4,0x34);
            frame.mat.e[4] = W(D_8009F9E4,0x38);
            frame.mat.e[5] = W(D_8009F9E4,0x3c);
            frame.mat.e[6] = W(D_8009F9E4,0x40);
            frame.mat.e[7] = W(D_8009F9E4,0x44);
            frame.mat.e[8] = W(D_8009F9E4,0x48);
            frame.mat.e[0] = mat0;
        }
        func_80010820(D_8009F9E4+0x28);
        func_8001006C(&frame.v, &frame.projected, 1);
        node = (u8 *)&frame.mat;
        if (UH(owner,0x1a) & 0x100) func_80010750(node);
        else func_80010820(node);
        func_8001047C(D_800A6418, D_800A6418, D_800AB388, count);
        if (!(UH(owner,0x8e) & 0x2000)) node = D_800A3548;
        else node = PTR(owner,0x68);
        dx = D_800A7840; dy = D_800A7898;
        for (i=0; i<count; i++) {
            t = D_800AB388[i] | D_800AB388[previous] | D_800AB388[next];
            if (!(t & 1) && t == 0xc0) {
                width = (H(node,0x10) << 9) / D_800A6418[previous].z;
                if (width < 3) width = 3;
                *dx = D_800A6418[next].y - D_800A6418[previous].y;
                *dy = D_800A6418[previous].x - D_800A6418[next].x;
                original_dx = *dx; a = original_dx; if (a < 0) a = -a;
                b = *dy; if (b < 0) b = -b;
                if (a < b) { a ^= b; b ^= a; a ^= b; }
                if ((a >> 1) < b) {
                    q = ((a+b)<<6) + ((a+b)<<3) + ((a+b)<<1) + a + (b<<5);
                    b = a-b;
                    a = q >> 7;
                }
                q = a + (b >> 2) - (b >> 6);
                if (q > 0) { width = (width << 10) / q; *dx = (original_dx*width)>>10; *dy = (*dy*width)>>10; }
                else { *dy = 0; *dx = 0; }
                __asm__("" : : "r"(next));
            }
            next = i+2;
            node = PTR(node,0);
            previous = i;
            if (next >= count) next = count-1;
            dx++; dy++;
        }
        if (!(UH(owner,0x8e) & 0x2000)) node = D_800A3548;
        else node = PTR(owner,0x68);
        if (!(UH(owner,0x8e) & 4)) {
        if (B(owner,0x72) >= 0) {
            s32 tx, ty, tw;
            tex = D_8009B234;
            if (B(owner,0x72) < 2) tex += B(owner,0x72);
            t = tex->page;
            frame.uv1 = (((t & 1) << 4) | (((((t >> 1) << 7) + 0x180) & 0x3ff) >> 6) | 0xa0) << 16;
            frame.uv0 = (((((t & 3) + ((t << 5) | 0xfc)) << 6) | (((u32)(t << 6 & 0x100) + 0x180) >> 4))) << 16;
            ty = tex->y; tx = tex->x; tw = tex->w;
            ty <<= 8;
            frame.uv0 = *(volatile u32 *)&frame.uv0 | (tx | ty);
            frame.uv1 = *(volatile u32 *)&frame.uv1 | ((tx + tw - 1) | ty);
            ux = UH(tex,4); uw = UH(tex,12); uy = UH(tex,8); uh = UH(tex,16);
            frame.uv2 = (ux + uw - 1) | ((uy + uh - 1) << 8);
            frame.uv3 = ux | ((uy + uh - 1) << 8);
        }
        } else tex = 0;
        textures = D_8009B234;
        cur = D_800A6418;
        offset = 8;
        for (i=0; i<count-1; ++i, ++cur, offset+=8) {
            t = D_800AB388[i] | D_800AB388[i+1];
            if ((t & 1) || t != 0xc0) goto next_node;
            if (!(UH(owner,0x8e) & 4) && B(owner,0x72) < 0) {
                p = PTR(node,0);
                t = p ? B(p,0x2a) : B(node,0x2a);
                tex = &textures[t];
                t = tex->page;
                frame.texpage1 = (((t & 1) << 4) | (((((t >> 1) << 7) + 0x180) & 0x3ff) >> 6) | 0xa0) << 16;
                frame.texpage0 = (((((t & 3) + ((t << 5) | 0xfc)) << 6) | (((u32)(t << 6 & 0x100) + 0x180) >> 4))) << 16;
            }
            depth = ((cur->z + H((u8 *)D_800A6418+offset,4)) >> 1) - 0x100;
            if (tex) {
                cursor = D_8009F9F0;
                if (cursor < D_8009FA28) D_8009F9F0 = cursor + 0x34;
                __asm__("" : "=r"(prim) : "0"(cursor));
                H(prim,8) = cur->x + D_800A7840[i];
                H(prim,0x14) = cur->x - D_800A7840[i];
                H(prim,0xa) = cur->y + D_800A7898[i];
                H(prim,0x16) = cur->y - D_800A7898[i];
                __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x2c) = following->x - D_800A7840[i+1];
                H(prim,0x20) = following->x + D_800A7840[i+1];
                H(prim,0x2e) = following->y - D_800A7898[i+1];
                H(prim,0x22) = following->y + D_800A7898[i+1];
                if (!((H(prim,8)<320 && H(prim,0xa)<240) ||
                      (H(prim,0x14)<320 && H(prim,0x16)<240) ||
                      (H(prim,0x20)<320 && H(prim,0x22)<240) ||
                      (H(prim,0x2c)<320 && H(prim,0x2e)<240))) {
                    D_8009F9F0 -= 0x34;
                    goto next_node;
                }
                {
                u32 c0 = W(node,0x14);
                u8 *nn2 = PTR(node,0);
                u32 c1 = W(nn2,0x14);
                __asm__("" : "+r"(c0), "+r"(c1));
                c0 = (c0 >> 8) | 0x3e000000;
                W(prim,0x10) = c0; W(prim,4) = c0;
                W(prim,0x28) = c1 >> 8; W(prim,0x1c) = c1 >> 8;
                }
                if (B(owner,0x72) < 0) {
                    W(prim,0xc) = (u16)(UH(tex,4) | (UH(tex,8)<<8)) | frame.texpage0;
                    {
                    u32 tx = UH(tex,4), tw = UH(tex,12), page;
                    __asm__("lw %0,%1" : "=r"(page) : "m"(frame.texpage1), "r"(tx), "r"(tw));
                    W(prim,0x18) = (u16)((tx+tw-1) | (UH(tex,8)<<8)) | page;
                    }
                    UH(prim,0x30) = (UH(tex,4)+UH(tex,12)-1) | ((UH(tex,8)+UH(tex,16)-1)<<8);
                    UH(prim,0x24) = UH(tex,4) | ((UH(tex,8)+UH(tex,16)-1)<<8);
                } else {
                    W(prim,0xc) = frame.uv0;
                    W(prim,0x18) = frame.uv1;
                    UH(prim,0x30) = frame.uv2;
                    UH(prim,0x24) = frame.uv3;
                }
                W(prim,0) = (s16)depth | 0x0c000000;
            } else {
                register u32 cmdhi asm("$4");
                s32 room;
                cursor = D_8009F9F0;
                if (cursor >= D_8009FA28) {
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    __asm__("lui %0,0xe100" : "=r"(cmdhi) : "r"(cursor));
                } else {
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    D_8009F9F0 = prim + 0x34;
                    __asm__("lui %0,0xe100" : "=r"(cmdhi) : "r"(prim));
                }
                {
                s32 tagdepth;
                __asm__("" : "=r"(tagdepth) : "0"(depth), "r"(cmdhi));
                W(prim,0) = (s16)tagdepth | 0x01000000;
                }
                cursor = D_8009F9F0;
                room = cursor < D_8009FA28;
                W(prim,4) = cmdhi | 0x6a0;
                if (!room) {
                    __asm__("" : "+r"(cursor));
                    prim = cursor;
                } else {
                    __asm__("" : "+r"(cursor));
                    prim = cursor;
                    D_8009F9F0 = prim + 0x34;
                }
                H(prim,8) = cur->x + D_800A7840[i];
                H(prim,0xa) = cur->y + D_800A7898[i];
                H(prim,0x10) = cur->x;
                H(prim,0x12) = cur->y;
                __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x18) = following->x + D_800A7840[i+1];
                H(prim,0x1a) = following->y + D_800A7898[i+1];
                H(prim,0x20) = following->x;
                H(prim,0x22) = following->y;
                if ((H(prim,8)<320 && H(prim,0xa)<240) ||
                    (H(prim,0x10)<320 && H(prim,0x12)<240) ||
                    (H(prim,0x18)<320 && H(prim,0x1a)<240) ||
                    (H(prim,0x20)<320 && H(prim,0x22)<240)) {
                    {
                    u32 c0 = W(node,0x14);
                    u8 *nn = PTR(node,0);
                    u32 c1 = W(nn,0x14);
                    __asm__("" : "+r"(c0), "+r"(c1));
                    W(prim,4) = 0x3a000000;
                    W(prim,0xc) = (c0 >> 8) | 0x3a000000;
                    W(prim,0x14) = 0;
                    W(prim,0) = (s16)depth | 0x08000000;
                    W(prim,0x1c) = c1 >> 8;
                    }
                    cursor = D_8009F9F0;
                    if (cursor < D_8009FA28) D_8009F9F0 = cursor + 0x34;
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    H(prim,8) = cur->x;
                    H(prim,0xa) = cur->y;
                    H(prim,0x10) = cur->x - D_800A7840[i];
                    H(prim,0x12) = cur->y - D_800A7898[i];
                    __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x18) = following->x;
                    H(prim,0x1a) = following->y;
                    H(prim,0x20) = following->x - D_800A7840[i+1];
                    H(prim,0x22) = following->y - D_800A7898[i+1];
                    if ((H(prim,8)<320 && H(prim,0xa)<240) ||
                        (H(prim,0x10)<320 && H(prim,0x12)<240) ||
                        (H(prim,0x18)<320 && H(prim,0x1a)<240) ||
                        (H(prim,0x20)<320 && H(prim,0x22)<240)) {
                        W(prim,0xc) = 0;
                        W(prim,0x1c) = 0;
                        W(prim,4) = ((u32)W(node,0x14) >> 8) | 0x3a000000;
                        W(prim,0) = (s16)depth | 0x08000000;
                        W(prim,0x14) = (u32)W(PTR(node,0),0x14) >> 8;
                    } else D_8009F9F0 -= 0x34;
                } else D_8009F9F0 -= 0x34;
            }
next_node:
            node = PTR(node,0);
        }
advance:
        obj = PTR(obj,0);
    }
}
