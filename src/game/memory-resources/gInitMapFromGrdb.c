// Categories: Resources > Asset Loader | Gameplay > Map
#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec63DC8;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    Vec63DC8 unk0C;
    s32 unk18;
} Res63DC8;

typedef struct {
    u8 *unk00;
    u8 pad04[0x1C];
    u32 unk20;
} Unk63DC8;

extern Res63DC8 * volatile gMapGridHeader __asm__("D_8009F80C");
extern Unk63DC8 *gMapChunkNode __asm__("D_8009F810");
extern void *gCellMap __asm__("D_8009F820");

extern s32 gMapGridBounds[] __asm__("D_800A2950");
extern s32 gCellClipRect[] __asm__("D_800A2970");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gAllocLightGrid(s32 width, s32 height) __asm__("func_800496D0");
extern void gAllocCellBucketGrid(s32 width, s32 height) __asm__("func_800834C4");

#define FIXMUL(dst, x)                                   \
    dst = 0x360000;                                      \
    __asm__ volatile("multu\t%0, %1\n\t"                   \
                     "mflo\t$8\n\t"                        \
                     "mfhi\t$9\n\t"                        \
                     "sll\t$9, $9, 16\n\t"                 \
                     "srl\t$8, $8, 16\n\t"                 \
                     "or\t%0, $9, $8"                       \
                     : "=r"(dst)                           \
                     : "r"(x), "0"(dst)                    \
                     : "$2", "$3", "$4", "$8", "$9")
void gInitMapFromGrdb(Unk63DC8 *arg0) __asm__("func_80063DC8");


/**
 * @brief Parses the map header chunk ("GRDB") and initializes world bounds and cell grids.
 *
 * Scans arg0->unk00..+arg0->unk20 for the last record tagged 0x47524442 ("GRDB"), publishes it to D_8009F80C, rotates and negates its origin vector unk0C, populates map world bounds and fixed-point 0x360000 inverse spans in D_800A2950[0..5] and cell counts in D_800A2970[4..5], and allocates D_8009F820 and two auxiliary cell grids.
 *
 * @param arg0 Loaded map resource descriptor (+0x00 data base, +0x20 byte length).
 *
 * @par Globals
 * D_8009F810/D_8009F80C - active map descriptor and "GRDB" payload; D_800A2950/D_800A2970 - world bounds and cell dimension tables; D_8009F820 - allocated 16-byte-per-cell grid buffer.
 *
 * @par Control flow
 * Single do-while scan over chunk headers (advancing by *(u32 *)(ptr + 4)), followed by straight-line coordinate setup and grid allocations.
 *
 * @par Calls
 * func_80017EBC - allocates D_8009F820 (tag 2); func_800496D0 - allocates/clears D_8009F780 pointer grid; func_800834C4 - allocates/clears D_8009F938 8-byte cell grid.
 *
 * @par Side effects
 * Writes D_8009F810, D_8009F80C, D_800A2950, D_800A2970, D_8009F820, mutates unk0C in place, and initializes sub-grids.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (0x80025210, level asset loader) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Handler for the 'Cptc' (0x43707463) chunk in gLoadQueuedAssets
 * (80024EC8.c:151-152): it stores the chunk in D_8009F810, scans its records
 * for the last 'GRDB' one and publishes that header (+0xC) in D_8009F80C,
 * replaces the origin vector (x, y, z) at +0xC by -(y, 0, x), then writes the
 * world bounds D_800A2950[0..3] (origin plus border << 17 and the far edges,
 * 0x20000 units per cell) and two scale factors [4],[5] (0x360000 times the
 * inverse half-spans) and the cell limits D_800A2970[4] = (w-1)*4, [5] =
 * (h-1)*4. It allocates the 16-byte-per-cell grid D_8009F820 for (w-1) x
 * (h-1) cells and sizes the light bucket grid (func_800496D0, D_8009F780) and
 * the scene-node bucket grid (func_800834C4, D_8009F938) to the same cell
 * counts. Everything later that clamps to the level (func_800665E8,
 * func_8006943C), samples its height (func_800695A8) or buckets scene nodes
 * (gSpawnSceneNode func_80083A10) depends on what this sets up once per level
 * load.
 */
void gInitMapFromGrdb(Unk63DC8 *arg0)
{
    u32 magic;
    u8 *ptr;
    Vec63DC8 local;
    Res63DC8 *transform;
    Res63DC8 *call_resource;
    Res63DC8 *last_resource;
    s32 old_x;
    s32 old_y;
    s32 right;
    void *result;
    register Res63DC8 *found __asm__("$3");
    register Res63DC8 *resource __asm__("$6");
    register u32 work_x __asm__("$5");
    register s32 work_y __asm__("$4");
    register s32 z_bottom __asm__("$7");
    register u32 numerator_q2 __asm__("$11");
    s32 product;

    magic = 0x47524442;
    ptr = arg0->unk00;
    gMapChunkNode = arg0;
    gMapGridHeader = 0;

    do {
        if (*(u32 *)ptr == magic) {
            found = (Res63DC8 *)(ptr + 0xC);
            gMapGridHeader = found;
        }
        ptr += *(u32 *)(ptr + 4);
    } while ((u32)(ptr - arg0->unk00) < arg0->unk20);

    transform = gMapGridHeader;
    resource = gMapGridHeader;

    old_x = transform->unk0C.x;
    old_y = transform->unk0C.y;
    numerator_q2 = 0x80000000u;
    transform->unk0C.y = 0;
    transform->unk0C.z = old_x;
    transform->unk0C.x = old_y;

    local = transform->unk0C;

    resource->unk0C.x = -resource->unk0C.x;
    resource->unk0C.y = -resource->unk0C.y;
    resource->unk0C.z = -resource->unk0C.z;

    z_bottom = local.z;
    work_x = (u32)(z_bottom + (resource->unk18 << 17));
    gMapGridBounds[0] = (s32)work_x;

    right = z_bottom + (resource->unk04 << 17)
        - ((resource->unk18 + 1) << 17);
    work_x = (u32)((right - (s32)work_x) >> 1);
    work_x = numerator_q2 / work_x;
    gMapGridBounds[1] = right;

    work_y = local.x;
    work_y += resource->unk18 << 17;
    gMapGridBounds[2] = work_y;

    z_bottom = z_bottom + (resource->unk08 << 17)
        - ((resource->unk18 + 1) << 17);
    work_y = (z_bottom - work_y) >> 1;
    numerator_q2 = numerator_q2 / (u32)work_y;

    FIXMUL(product, work_x);
    gMapGridBounds[4] = product;
    FIXMUL(product, numerator_q2);
    gMapGridBounds[5] = product;
    gMapGridBounds[3] = z_bottom;

    gCellClipRect[4] = (resource->unk04 - 1) << 2;
    gCellClipRect[5] = (resource->unk08 - 1) << 2;

    product = (resource->unk04 - 1) * (resource->unk08 - 1);
    result = gAllocMemory(product << 4, 2);
    call_resource = gMapGridHeader;
    gCellMap = result;

    gAllocLightGrid(call_resource->unk04 - 1,
                  call_resource->unk08 - 1);

    last_resource = gMapGridHeader;
    gAllocCellBucketGrid(last_resource->unk04 - 1,
                  last_resource->unk08 - 1);
}
