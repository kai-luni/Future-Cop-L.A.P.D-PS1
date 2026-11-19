// Categories: Graphics > Model Renderer > Flat Polygons | Graphics > Geometry
#include "game/types.h"

typedef struct V3_72F7C {
    s32 x;
    s32 y;
    s32 z;
} V3_72F7C;

typedef struct S3X_72F7C {
    u16 x;
    u16 y;
    s16 z;
    u16 pad;
} S3X_72F7C;

struct Entry72F7C {
    V3_72F7C *verts;
    void *shorts;
    s16 count;
    u16 padA;
    u8 r;
    u8 g;
    u8 b;
    u8 padF;
};

struct Node72F7C {
    void *next;
    void *a;
    void *b;
    s16 count;
    u16 flags;
    struct Entry72F7C *entries;
};

struct Camera72F7C {
    u8 unk00[0x28];
    u8 matrix[0x54];
    V3_72F7C eye;
};

struct Prim72F7C {
    union {
        u32 word;
        u8 bytes[4];
        struct { u32 addr:24; u32 len:8; } fields;
    } hdr;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x0;
    s16 y0;
    u16 uv;
    u16 clut;
    u8 pad[0x34 - 0x10];
};

extern struct Camera72F7C *const gCamera __asm__("D_8009F9E4");
extern struct Prim72F7C *gPrimPoolNext __asm__("D_8009F9F0");
extern struct Prim72F7C *gPrimPoolEnd __asm__("D_8009FA28");

extern s32 gVectorLength3D(V3_72F7C *vector) __asm__("func_8001A100");
extern void gGteLoadRotation(u8 *matrix) __asm__("func_80010820");
extern void gProjectVertexBatch(S3X_72F7C *in, S3X_72F7C *out, u8 *flags, s32 count) __asm__("func_8001047C");

/**
 * @brief Draws a model node as flat-shaded polygons (the cheap renderer path).
 *
 * @param node Node: vertex pointer, a short-coordinate block, a count and a base
 *             colour, plus the per-node matrix.
 * @return Nothing.
 *
 * @par Confirmed
 * The node's vertices are transformed with func_8001047C (projection + clip
 * byte), the matrix is loaded with func_80010820 and the geometry is passed
 * through func_8001A100 for the radius/range test. The colours are the node's
 * single RGB triple (no per-vertex shading), and the packet pool is D_8009F9F0.
 *
 * @par Compare
 * func_80071FA4 is the full path (per-vertex shading, textures, environment
 * mapping); this one is used for nodes without those attributes - func_80073700
 * selects between them by checking whether the node's two geometry pointers are
 * set.
 *
 * @par Callers
 * func_80073700.
 */
void func_80072F7C(struct Node72F7C *node, V3_72F7C *arg)
{
    S3X_72F7C shorts[100];
    u8 buf[0x68];
    V3_72F7C vec;
    struct Prim72F7C *p;
    register struct Camera72F7C *camera __asm__("$7");
    register s32 depth __asm__("$4");
    register u32 tag __asm__("$2");
    register s32 entry_offset __asm__("$6");
    register S3X_72F7C *screen_arg __asm__("$4");
    register S3X_72F7C *screen_out __asm__("$5");
    register struct Entry72F7C *call_entries __asm__("$2");
    s32 i;
    s32 j;
    s32 k;
    s16 dx;
    s16 dy;

    if (node->flags & 0x4000) {
        return;
    }
    for (i = 0; i < node->count; i++) {
        vec.x = node->entries[i].verts[0].x - arg->x;
        vec.y = node->entries[i].verts[0].y - arg->y;
        vec.z = node->entries[i].verts[0].z - arg->z;
        if (0x1FFFF < gVectorLength3D(&vec)) {
            vec.x = node->entries[i].verts[node->entries[i].count].x - arg->x;
            vec.y = node->entries[i].verts[node->entries[i].count].y - arg->y;
            vec.z = node->entries[i].verts[node->entries[i].count].z - arg->z;
            if (0x1FFFF < gVectorLength3D(&vec)) {
                continue;
            }
        }
        j = 0;
        if (j < node->entries[i].count + 1) {
            entry_offset = i * sizeof(struct Entry72F7C);
            __asm__ volatile("" : : "r"(entry_offset));
            camera = gCamera;
            __asm__ volatile("" : : "r"(camera));
            for (; j < node->entries[i].count + 1; j++) {
            __asm__ volatile("" : : "r"(camera));
            shorts[j].x = (node->entries[i].verts[j].x - camera->eye.x) >> 4;
            shorts[j].y = (node->entries[i].verts[j].y - camera->eye.y) >> 4;
            shorts[j].z = (node->entries[i].verts[j].z - camera->eye.z) >> 4;
            }
        }
        gGteLoadRotation(gCamera->matrix);
        screen_arg = shorts;
        __asm__ volatile("" : : "r"(screen_arg));
        call_entries = node->entries;
        __asm__ volatile("" : : "r"(call_entries));
        screen_out = screen_arg;
        __asm__ volatile("" : : "r"(screen_out));
        gProjectVertexBatch(screen_arg, screen_out, buf, call_entries[i].count + 1);
        for (k = 0; k < node->entries[i].count; k++) {
            if ((buf[k] | buf[k + 1]) & 1) {
                continue;
            }
            dx = (s16)(shorts[k + 1].x - shorts[k].x);
            dy = (s16)(shorts[k + 1].y - shorts[k].y);
            if (dx < 0) {
                if (dy < 0) {
                    if (dy > dx) {
                        shorts[k].x = shorts[k].x - 1;
                    } else {
                        shorts[k].y = shorts[k].y - 1;
                    }
                } else {
                    if (-dx > dy) {
                        shorts[k].x = shorts[k].x - 1;
                    } else {
                        shorts[k].y = shorts[k].y + 1;
                    }
                }
            } else {
                if (dy < 0) {
                    if (dx > -dy) {
                        shorts[k].x = shorts[k].x + 1;
                    } else {
                        shorts[k].y = shorts[k].y - 1;
                    }
                } else {
                    if (dx > dy) {
                        shorts[k].x = shorts[k].x + 1;
                    } else {
                        shorts[k].y = shorts[k].y + 1;
                    }
                }
            }
            p = gPrimPoolNext < gPrimPoolEnd ? gPrimPoolNext++ : gPrimPoolNext;
                p->hdr.bytes[3] = 1;
                *(u32 *)&p->r = 0xE10006A0;
                p->hdr.fields.addr = shorts[k].z;
                p = gPrimPoolNext < gPrimPoolEnd ? gPrimPoolNext++ : gPrimPoolNext;
                depth = shorts[k].z;
                p->code = 0x42;
                p->hdr.fields.addr = depth;
                p->hdr.bytes[3] = 3;
                p->r = node->entries[i].r;
                p->g = node->entries[i].g;
                p->b = node->entries[i].b;
                p->x0 = shorts[k].x;
                p->y0 = shorts[k].y;
                p->uv = shorts[k + 1].x;
                p->clut = shorts[k + 1].y;
        }
    }
}
