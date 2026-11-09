#include "game/types.h"
typedef struct {
    s16 vx;
    s16 vy;
} DVECTOR;


typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

struct Node {
    u8 pad00[0x10];
    Vec3 v[2];
    u8 pad28[0x18];
    u32 field40;
    u16 field44;
};

struct Camera {
    u8 pad00[0x7C];
    s32 x;
    s32 y;
    s32 z;
};

extern struct Camera *D_8009F9E4;
extern u8 *D_8009F9F0;
extern u8 *D_8009FA28;
extern u32 D_8009F388;

extern s32 func_8001A100(s32 *vector);
extern void func_80010820(u8 *matrix);
extern void func_80010704(s16 *src, s16 *dst);
extern void func_8006F21C(DVECTOR a0, DVECTOR a1, DVECTOR a2, DVECTOR a3, s32 s3, s32 s4, u32 s5, u32 s6, s32 depth);

void func_8006F310(struct Node *node, Vec3 *pos)
{
    s32 diff[3];
    DVECTOR proj[2][4];
    struct Camera *cam;
    register struct Node *nptr __asm__("$17");
    s32 i;
    s32 s1;
    s32 s3;
    s32 s4;
    s32 abs_s3;
    s32 abs_s4;
    s32 a;
    s32 b;
    s32 sum;
    s32 dist;
    s16 depth;
    u8 *prim;
    u32 s6;
    u32 w;
    u32 mask;
    u16 z1;

    diff[0] = node->v[0].x - pos->x;
    diff[1] = node->v[0].y - pos->y;
    diff[2] = node->v[0].z - pos->z;
    if (func_8001A100(diff) > 0x1FFFF) {
        diff[0] = node->v[1].x - pos->x;
        diff[1] = node->v[1].y - pos->y;
        diff[2] = node->v[1].z - pos->z;
        if (func_8001A100(diff) > 0x1FFFF) {
            return;
        }
    }

    i = 0;
    nptr = node;
    for (; i < 2; i++, nptr = (struct Node *)((u8 *)nptr + 12)) {
        __asm__ volatile("" : : "r"(nptr));
        cam = D_8009F9E4;
        diff[0] = nptr->v[0].x - cam->x;
        diff[1] = nptr->v[0].y - cam->y;
        diff[2] = nptr->v[0].z - cam->z;
        func_80010820((u8 *)cam + 0x28);
        proj[1][2].vx = diff[0] >> 4;
        proj[1][2].vy = diff[1] >> 4;
        proj[1][3].vx = diff[2] >> 4;
        func_80010704((s16 *)&proj[1][2], (s16 *)&proj[i][0]);
        if (proj[i][1].vx < 0x40) {
            return;
        }
        if (proj[i][0].vx < 0) {
            return;
        }
        if (proj[i][0].vx >= 0x141) {
            return;
        }
        if (proj[i][0].vy < 0) {
            return;
        }
        if (proj[i][0].vy >= 0xF1) {
            return;
        }
    }

    proj[0][2].vx = (u16)proj[0][0].vx + ((proj[1][0].vx - proj[0][0].vx) >> 1);
    proj[0][2].vy = (u16)proj[0][0].vy + ((proj[1][0].vy - proj[0][0].vy) >> 1);

    s1 = (s16)node->field44 >> 1;
    s1 = (s1 << 9) / proj[0][1].vx;
    z1 = proj[1][1].vx;
    if (s1 < 3) {
        s1 = 3;
    }
    depth = ((proj[0][1].vx + (s16)z1) >> 1) - 200;

    s3 = proj[1][0].vy - proj[0][0].vy;
    s4 = proj[0][0].vx - proj[1][0].vx;

    a = s3;
    if (a < 0) {
        a = -a;
    }
    b = s4;
    if (b < 0) {
        b = -b;
    }
    if (a < b) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    sum = a + b;
    if ((a >> 1) < b) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + a + (b << 5);
        b = a - b;
        a = sum >> 7;
    }
    dist = a + (b >> 2) - (b >> 6);

    if (dist > 0) {
        s1 = (s1 * 1024) / dist;
        s3 = (s3 * s1) >> 10;
        s4 = (s4 * s1) >> 10;
    } else {
        s4 = 0;
        s3 = s4;
    }

    s6 = node->field40;
    w = D_8009F388;
    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    prim = D_8009F9F0;
    goto L_done;
L_alloc:
    prim = D_8009F9F0;
    D_8009F9F0 = prim + 0x34;
L_done:

    *(u32 *)(prim + 4) = 0xE10006A0;
    mask = 0xFFFFFF;
    w = (w >> 8) | 0x3A000000;
    s6 = s6 >> 8;
    prim[3] = 1;
    *(u32 *)prim = (*(u32 *)prim & 0xFF000000) | (depth & mask);

    func_8006F21C(proj[0][0], proj[0][1], proj[0][2], proj[0][3], s3, s4, w, s6, depth);
    __asm__ volatile("" : "+r"(s3));
    i = -s3;
    s1 = -s4;
    func_8006F21C(proj[0][0], proj[0][1], proj[0][2], proj[0][3], i, s1, w, s6, depth);
    func_8006F21C(proj[1][0], proj[1][1], proj[0][2], proj[0][3], s3, s4, w, s6, depth);
    func_8006F21C(proj[1][0], proj[1][1], proj[0][2], proj[0][3], i, s1, w, s6, depth);
}
