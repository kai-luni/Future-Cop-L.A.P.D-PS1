// Categories: Graphics > Effects > World Quads | Graphics > Model Renderer > Primitives | Graphics > 3D > Projection
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

/**
 * @brief Draws one world-space quad band between two points as four textured
 *        packets, after a cheap distance reject and per-endpoint screen tests.
 *
 * Confirmed role
 * --------------
 * func_8006F844 walks the list head D_800A353C and calls this function once per
 * node with the same second argument, so the function owns the per-node draw
 * side of that list. It emits nothing but the four calls to the quad emitter
 * func_8006F21C, whose own documentation names this caller as the model draw
 * entry.
 *
 * @param node List node: two points at +0x10 and +0x1C (three words each), a
 *        colour word at +0x40 and a 16-bit width parameter at +0x44.
 * @param pos  Reference point for the early distance reject (Likely the camera
 *        or view position passed through from the caller).
 *
 * @return Nothing. Three early returns exist (distance reject after both
 *         endpoints, and the screen tests of either endpoint) and each of them
 *         means "no packet at all".
 *
 * @par Confirmed range reject
 * diff = v[0] - pos. When func_8001A100(diff) exceeds 0x1FFFF the test is
 * repeated with v[1]; two failures end the call before any packet is taken from
 * the pool.
 *
 * @par Confirmed projection and screen tests
 * The loop runs twice, advancing the node pointer by 12 bytes, so the second
 * pass uses v[1] as its base point. The camera block D_8009F9E4 supplies the
 * position at +0x7C/+0x80/+0x84; the camera matrix at +0x28 is loaded with
 * func_80010820 (the variant that zeroes the translation), the difference is
 * shifted right by 4 and func_80010704 transforms it into the pair
 * proj[i][0]/proj[i][1], where proj[i][1].vx carries the depth. Each endpoint
 * is rejected when the depth is below 0x40, x is negative or >= 0x141, or y is
 * negative or >= 0xF1 - a 320 x 240 screen with a 64-unit near plane.
 *
 * @par Confirmed band geometry
 * The midpoint of the two screen points becomes proj[0][2]. The half width is
 * ((field44 >> 1) << 9) / depth, floored at 3. The depth written into the packet
 * tag is ((depth0 + depth1) >> 1) - 200, an ordering-table bucket offset. The
 * direction of the band is (dy, -dx); it is normalized with the octagonal
 * approximation used elsewhere in this engine and scaled by the half width, so
 * s3/s4 are the perpendicular offset in screen units and are zero when the
 * direction length is not positive.
 *
 * @par Confirmed packet handling
 * One packet of 0x34 bytes: when the cursor D_8009F9F0 is below the limit
 * D_8009FA28 it is advanced by 0x34, otherwise the last slot is reused - the
 * same "always emit something" policy as func_8006F21C. The tag word keeps its
 * top byte and takes the low 24 bits from the depth value. The draw-mode word is
 * (D_8009F388 >> 8) | 0x3A000000 and the colour is field40 >> 8; both are handed
 * to func_8006F21C, which is called four times with the offsets (+s3, +s4) and
 * (-s3, -s4) and with the first and the second endpoint pair, so the band is a
 * 2 x 2 block of polygons.
 *
 * @par Likely
 * With a 0x3A000000 draw-mode word, a width parameter and four coplanar quads,
 * the node is a translucent trail or beam segment: the four calls duplicate the
 * same band with opposite perpendicular offsets, which only makes sense as the
 * two halves of a symmetric ribbon. That the second argument of func_8006F844 is
 * the camera position follows from its use here and from the camera block read
 * a few lines later, not from a symbol name.
 *
 * @par Confirmed dead stores
 * The word at packet+4 is written as 0xE10006A0 and the length byte as 1 before
 * func_8006F21C runs, but that function writes packet+4 from its own argument
 * and sets the length byte to 8, so both values are overwritten and cannot reach
 * the GPU.
 *
 * @par Unknown
 * The 200 subtracted from the average depth, the +0x40 colour layout (three
 * bytes and one flag) and the meaning of the width field at +0x44 (world or
 * screen units before the division by the depth) are not decidable from this
 * file alone.
 *
 * @par Globals
 * D_8009F9E4 at 0x8009F9E4 - camera/module block; position +0x7C/+0x80/+0x84,
 * matrix +0x28.
 * D_8009F9F0 at 0x8009F9F0 - packet cursor, advanced by 0x34 per call.
 * D_8009FA28 at 0x8009FA28 - packet limit, the "pool full" test.
 * D_8009F388 at 0x8009F388 - draw-mode word, shifted right by 8 into the packet.
 *
 * @par Node offsets used (Confirmed)
 * +0x10 and +0x1C the two points, +0x40 colour word, +0x44 width.
 *
 * @par Control flow
 * 1. Distance reject against both points.
 * 2. Per endpoint: camera-relative vector, matrix load, projection, five screen
 *    tests.
 * 3. Midpoint, half width, depth, perpendicular offset.
 * 4. Packet allocation and header.
 * 5. Four quad emissions.
 *
 * @par Calls
 * func_8001A100 (length, two sites), func_80010820 (camera matrix to GTE, two
 * sites), func_80010704 (projection, two sites), func_8006F21C (quad emitter,
 * four sites).
 *
 * @par Side effects
 * Advances or reuses the packet cursor, writes 0x34 bytes of GPU packet data,
 * replaces the GTE rotation matrix with the camera matrix and writes nothing
 * else. D_8009F388 and the camera block are only read.
 *
 * @par Callers
 * func_8006F844, which walks D_800A353C (analysis/edges.json, kind "direct").
 */
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
