// Categories: Graphics > Scene > Node Spawn | Graphics > 3D > Culling | Game Engine > Entity Lifecycle
#include "game/types.h"

struct Model3A10 {
    u8 pad00[0x34];
    void *unk34;
};

struct Node3A10 {
    struct Model3A10 *model;
    s32 mat[3][3];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 pad40[8];
    s16 unk48;
    u8 flags4A;
    s8 unk4B;
    u8 flags4C;
    u8 unk4D;
    s16 unk4E;
    union {
        s32 word;
        struct { u16 low; u16 high; } half;
    } unk50;
    u16 unk54;
    s8 unk56;
    u8 unk57;
    struct Node3A10 *next;
};

struct Req3A10 {
    u16 unk00;
    u8 pad02[6];
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s8 unk1C;
    u8 unk1D;
    u8 unk1E;
    s8 unk1F;
    u16 unk20;
    u8 unk22;
    u8 unk23;
    void *unk24;
    void *unk28;
    s32 unk2C;
};

struct Vec3I { s32 x; s32 y; s32 z; };
struct Bucket3A { struct Node3A10 *head; s32 arena; };

extern void *gMissionState __asm__("D_8009F934");
extern struct Bucket3A *gBucketArray __asm__("D_8009F938");
extern s32 gMissionGridWidth __asm__("D_8009F93C");
extern s32 gDiscShadeByte __asm__("D_8009FAA4");
extern void *func_800195A4(void *state);
extern void func_80083834(s32 arg0, s32 arg1);
extern void func_80013BD8(struct Node3A10 *node, struct Model3A10 *model, s32 options);
extern u32 func_80019668(void);
extern s32 func_80067684(void *vec, s32 adjust);
extern void func_800188D0(s32 x, s32 y, s32 z, void *arg3);
extern void func_80010820(void *matrix);
extern void func_8001006C(void *src, void *dst, s32 count);
extern s32 func_80083764(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void func_80010750(void *a);
extern void func_80010234(void *a, void *b, s32 c);

/**
 * @brief Turns one spawn request into a render node: takes a node from the
 *        pool, attaches the model, builds transform and bounding data and links
 *        the node into the grid bucket the renderer walks.
 *
 * Confirmed role
 * --------------
 * The two callers (func_80031690 and func_80084110) hand in a request block
 * and a model and get 1 back when the node exists; the node is then reachable
 * through the bucket array D_8009F938 that the scene renderer traverses. The
 * function is therefore both the allocator and the insertion point of the
 * scene graph.
 *
 * @param req   Spawn request (0x30 bytes): id at +0x0, position at +0x8, three
 *        angles at +0x14/+0x16/+0x18, a roll offset at +0x1A, an alignment
 *        code at +0x1C, flags at +0x1D, an animation rate at +0x1E, three Q6
 *        scale bytes at +0x1F..+0x21, a flag at +0x22, an angle in degrees at
 *        +0x23.
 * @param model Model. Only +0x34 (the part/bound table) is read, after
 *        func_80013BD8 has stored it in the node.
 * @param z2    Time tag: >= 0 stores the value plus D_8009FAA4 in the node's
 *        +0x50 and makes the node "dynamic"; negative sets bit 0x80 of the
 *        flags and fills +0x50 with two 16-bit fields instead.
 * @param arg3  Optional 12-word transform: 9 matrix words followed by the three
 *        position words. When null the transform is built from the request.
 *
 * @return 1 when the node was created, 0 when the pool refused twice in a row.
 *
 * @par Confirmed allocation
 * node = func_800195A4(D_8009F934). If that returns null the function calls
 * func_80083834(D_8009FAA4 - 0x5DC, 1) - the frame stamp minus 1500 - and tries
 * again; a second null ends the call with 0. func_80013BD8(node, model, 0)
 * then installs the model and copies the identity matrix D_8009AA38 into the
 * node's matrix.
 *
 * @par Confirmed field set-up
 * +0x4B = -1, +0x56 = req+0x22, +0x54 = req+0x0, +0x4C = req+0x1D; bit 0 of
 * +0x4C sets bit 8 of +0x4A, bit 1 sets bit 4. The rate at req+0x1E becomes
 * +0x4D: 0 stays 0, 1 becomes 0xFF, anything else 300 / rate.
 *
 * @par Confirmed transform
 * With arg3 the nine matrix words and the three position words are copied and
 * +0x57 takes req+0x16 when bit 0x40 of the flags is set, else 0. Without arg3
 * the position comes from req+0x8, the pitch is read from req+0x1C (skip on
 * -99, otherwise -1 and -2 for the codes 0 and 1 and code - 2 above that, with
 * req+0x1A shifted by 4 added to +0x2C), and the three rotation angles are
 * req+0x14/+0x18/+0x16, each replaced by a random 12-bit angle when negative.
 * func_800188D0 then builds the matrix, and unless req+0x1F and req+0x20 are
 * exactly 0x40 and 0x40 each row is multiplied by the byte scale at
 * +0x1F/+0x20/+0x21 and shifted right by 6 - so those bytes are Q6 factors
 * (0x40 = 1.0). A negative scale is jittered by random % (-2 * scale) + 64.
 *
 * @par Confirmed bounds and insertion
 * The six s16 values at model+0x34 + 8 become a diagonal matrix (positions 0,
 * 4, 8 and the translation words 9..11, all shifted by 4), loaded with
 * func_80010820 and applied to four points with func_8001006C. The fourth point
 * is moved by +0x2C/+0x30, stored at +0x34 (before its x is offset by +0x28 -
 * the stored x is the pre-offset value), and the axis extremes are summed from
 * the absolute values of the matrix words 0, 3, 6 and 2, 5, 8 to feed
 * func_80083764, whose result lands in +0x4E (Likely a packed bounding rectangle).
 * The bucket index is (z >> 17) + D_8009F93C * (x >> 17): dynamic nodes are
 * pushed at the head, static ones (z2 < 0) are appended at the tail so spawn
 * order is kept. The bucket's arena field is stamped with D_8009FAA4.
 *
 * @par Likely
 * A non-zero +0x56 (copied from req+0x22) makes the function zero the matrix
 * words 1, 2, 3 and 7 and set word 4 to 0x10000, which is an axis-aligned "keep
 * upright" transform. +0x57 selects one model part that is re-transformed at
 * the end with the matrix and written to +0x34 - a per-node attachment point.
 *
 * @par Unknown
 * The unit of req+0x23 (divided by 360 and shifted by 12, so degrees into the
 * engine's 4096-step angle) and the meaning of +0x54/+0x56 are only visible as
 * data flow; no name is recoverable from this file.
 *
 * @par Globals
 * D_8009F934 at 0x8009F934 - node pool state passed to func_800195A4.
 * D_8009F938 at 0x8009F938 - bucket array; element 0 is the list head, +0x4
 * the arena stamp.
 * D_8009F93C at 0x8009F93C - row stride in buckets for the grid index.
 * D_8009FAA4 at 0x8009FAA4 - frame stamp, also used as the 1500-tick pool
 * timeout and the dynamic/static decision.
 *
 * @par Node offsets used (Confirmed)
 * +0x0 model, +0x4 matrix (9 words), +0x28/+0x2C/+0x30 position, +0x34 bound
 * centre, +0x48 s16 frame, +0x4A flag byte, +0x4B s8, +0x4C flag byte, +0x4D
 * byte, +0x4E u16 rectangle, +0x50 u32 tag (or two u16 angles), +0x54 u16,
 * +0x56 s8, +0x57 byte, +0x58 next.
 *
 * @par Control flow
 * 1. Allocate (with one retry after the 1500-tick reset request).
 * 2. Attach the model, copy the identity matrix.
 * 3. Copy the request fields, build transform (arg3 or request), scale rows.
 * 4. Optional upright correction.
 * 5. Build the bound matrix, transform four points, derive the rectangle.
 * 6. Insert into the bucket, stamp the arena.
 * 7. Optional part transform, return 1.
 *
 * @par Calls
 * func_800195A4 (allocate, two sites), func_80083834 (reset request),
 * func_80013BD8 (attach model), func_80019668 (random, up to five sites),
 * func_80067684 (ground height), func_800188D0 (matrix from angles),
 * func_80010820 (matrix to GTE, twice), func_8001006C (transform four points),
 * func_80083764 (rectangle), func_80010750 and func_80010234 (final part
 * transform).
 *
 * @par Side effects
 * Consumes a node from the pool, writes the bucket head or tail, overwrites the
 * bucket's arena stamp, replaces the GTE matrix and consumes the random
 * generator. No plain global variable is written, only the pool and the bucket
 * array behind the two pointers.
 *
 * @par Callers
 * func_80031690 and func_80084110 (analysis/edges.json, kind "direct"); the
 * address also carries an "address_taken" edge.
 */
u8 func_80083A10(struct Req3A10 *req, struct Model3A10 *model, s32 z2, s32 *arg3)
{
    struct Node3A10 *node;
    s32 mtx[12];
    struct Vec3I dst[4];
    s32 hi;
    s32 lo;
    s32 hi2;
    s32 lo2;
    s32 v;
    register s32 term __asm__("$2");
    s32 px;
    s32 py;
    s32 pz;
    s32 idx;
    s32 adjust;
    register s32 originX __asm__("$4");
    u8 rate;
    struct Node3A10 *head;
    s16 *blk;
    s32 scale;
    s32 i;
    s32 j;
    s8 *q;
    s32 *sp;

    node = (struct Node3A10 *)func_800195A4(gMissionState);
    if (node == 0) {
        func_80083834(gDiscShadeByte - 0x5DC, 1);
        node = (struct Node3A10 *)func_800195A4(gMissionState);
        if (node == 0) {
            return 0;
        }
    }
    func_80013BD8(node, model, 0);
    node->unk4B = -1;
    node->unk56 = req->unk22;
    node->unk54 = req->unk00;
    node->flags4C = req->unk1D;
    if (node->flags4C & 1) {
        node->flags4A |= 8;
    }
    if (node->flags4C & 2) {
        node->flags4A |= 4;
    }
    rate = req->unk1E;
    if (rate != 0) {
        v = rate;
        if (v == 1) {
            node->unk4D = 0xFF;
        } else {
            node->unk4D = 300 / v;
        }
    } else {
        node->unk4D = 0;
    }
    if (z2 >= 0) {
        z2 = z2 + gDiscShadeByte;
        node->unk50.word = z2;
    } else {
        node->flags4C |= 0x80;
        node->unk50.half.low = (u16)req->unk14;
        node->unk50.half.high = ((s32)req->unk23 << 12) / 360;
    }
    if (arg3 != 0) {
        node->mat[0][0] = arg3[0];
        node->mat[0][1] = arg3[1];
        node->mat[0][2] = arg3[2];
        node->mat[1][0] = arg3[3];
        node->mat[1][1] = arg3[4];
        node->mat[1][2] = arg3[5];
        node->mat[2][0] = arg3[6];
        node->mat[2][1] = arg3[7];
        node->mat[2][2] = arg3[8];
        node->unk28 = arg3[9];
        node->unk2C = arg3[10];
        node->unk30 = arg3[11];
        if (node->flags4C & 0x40) {
            node->unk57 = (u8)req->unk16;
        } else {
            node->unk57 = 0;
        }
    } else {
        *(struct Vec3I *)&node->unk28 = *(struct Vec3I *)&req->unk08;
        v = req->unk1C;
        if (v != -99) {
            adjust = (v == 0) ? -1 : (v == 1) ? -2 : v - 2;
            v = func_80067684(&node->unk28, adjust);
            node->unk2C = v + (req->unk1A << 4);
        }
        {
            register s32 ax __asm__("$4");
            ax = req->unk14;
            px = ax;
            if (ax < 0) {
                px = func_80019668() & 0xFFF;
            }
        }
        {
            register s32 ay __asm__("$5");
            ay = req->unk18;
            py = ay;
            if (ay < 0) {
                py = func_80019668() & 0xFFF;
            }
        }
        pz = 0;
        if (node->flags4C & 0x40) {
            node->unk57 = (u8)req->unk16;
        } else {
            pz = req->unk16;
            if (pz < 0) {
                pz = func_80019668() & 0xFFF;
            }
            node->unk57 = 0;
        }
        func_800188D0(px, py, pz, &node->mat[0][0]);
        i = 0;
        if (req->unk1F != 0x40 || req->unk20 != 0x4040) {
            q = (s8 *)node;
            do {
                scale = ((s8 *)((s32)req + i))[0x1F];
                if (scale < 0) {
                    s32 r;
                    r = func_80019668() % (u32)(-(2 * scale));
                    r += 64;
                    scale += r;
                }
                j = 0;
                sp = (s32 *)(q + 4);
                do {
                    term = *sp * scale;
                    *sp = term >> 6;
                    j = j + 1;
                    sp = sp + 1;
                } while (j < 3);
                i = i + 1;
                q = q + 12;
            } while (i < 3);
        }
    }
    if (node->unk56 != 0) {
        node->mat[1][1] = 0x10000;
        node->mat[2][1] = 0;
        node->mat[1][2] = 0;
        node->mat[1][0] = 0;
        node->mat[0][1] = 0;
    }
    blk = (s16 *)node->model->unk34 + 4;
    mtx[9] = blk[0] << 4;
    mtx[10] = blk[1] << 4;
    mtx[11] = blk[2] << 4;
    mtx[0] = blk[3] << 4;
    mtx[1] = 0;
    mtx[2] = 0;
    mtx[3] = 0;
    mtx[4] = blk[4] << 4;
    mtx[5] = 0;
    mtx[6] = 0;
    mtx[7] = 0;
    mtx[8] = blk[5] << 4;
    func_80010820(&node->mat[0][0]);
    func_8001006C(mtx, dst, 4);
    originX = node->unk28;
    dst[3].y += node->unk2C;
    dst[3].z += node->unk30;
    *(struct Vec3I *)&node->unk34 = dst[3];
    term = dst[3].x;
    hi = mtx[0];
    term += originX;
    lo = hi;
    dst[3].x = term;
    if (hi > 0) {
        lo = -hi;
    } else {
        hi = -hi;
    }
    term = mtx[3];
    if (term > 0) {
        lo -= term;
        hi += term;
    } else {
        lo += term;
        hi -= term;
    }
    term = mtx[6];
    if (term > 0) {
        lo -= term;
        hi += term;
    } else {
        lo += term;
        hi -= term;
    }
    hi2 = mtx[2];
    lo2 = hi2;
    if (hi2 > 0) {
        lo2 = -hi2;
    } else {
        hi2 = -hi2;
    }
    term = mtx[5];
    if (term > 0) {
        lo2 -= term;
        hi2 += term;
    } else {
        lo2 += term;
        hi2 -= term;
    }
    term = mtx[8];
    if (term > 0) {
        lo2 -= term;
        hi2 += term;
    } else {
        lo2 += term;
        hi2 -= term;
    }
    node->unk4E = func_80083764(dst[3].z, dst[3].x,
                               dst[3].z + lo2, dst[3].z + hi2,
                               dst[3].x + lo, dst[3].x + hi);
    idx = (dst[3].z >> 17) + gMissionGridWidth * (dst[3].x >> 17);
    if (z2 >= 0 || gBucketArray[idx].head == 0) {
        node->next = gBucketArray[idx].head;
        gBucketArray[idx].head = node;
    } else {
        head = gBucketArray[idx].head;
        while (head->next != 0) {
            head = head->next;
        }
        head->next = node;
        node->next = 0;
    }
    gBucketArray[idx].arena = gDiscShadeByte;
    if (node->unk57 != 0) {
        register s32 frame __asm__("$4");
        func_80010750(&node->mat[0][0]);
        frame = node->unk48;
        frame <<= 4;
        __asm__("" : "+r"(frame));
        frame += 8;
        func_80010234((u8 *)node->model->unk34 + frame,
                      &node->unk34, 1);
    }
    return 1;
}
