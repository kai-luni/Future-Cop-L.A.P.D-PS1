// Categories: Graphics > Culling > Object Grid | Graphics > HUD > Minimap
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef struct Obj { char pad0[8]; s32 active; char padc[0x18]; s32 flags; char pad28[0x34]; s32 z; s32 y; s32 x; s32 radius; } Obj;
typedef struct { s32 count, pad; Obj *items[1]; } List;
typedef struct Node { Obj *obj; struct Node *next; } Node;
typedef struct { s32 x,z,w,h; } Bounds;
typedef struct { s32 x,z; } Pair;
typedef struct { u32 bits; s32 pad[3]; } Cell;
extern List gCommandListHead __asm__("D_800A8F90");
extern Pair gViewGroundAnchors[] __asm__("D_800A3190");
extern Bounds gCellClipRect __asm__("D_800A2970");
extern Node gCellNodeArray[] __asm__("D_800A2990");
extern Cell gCoverageGrid[] __asm__("D_800AB3B8");
extern s32 gViewCount __asm__("D_8009F814");
extern s32 gCellEntityCount __asm__("D_8009F828");
extern Obj **gCellEntityList __asm__("D_8009F818");
extern u8 *gCellMap __asm__("D_8009F820");
extern u8 gGridOriginY __asm__("D_8009F835");
extern u8 gGridOriginX __asm__("D_8009F834");
void gBuildSingleViewObjectGrid(void) __asm__("func_80062C3C");


/**
 * @brief Builds the single-view object list and the per-cell object grid of the
 *        region, testing every entity against the first view anchor only.
 *
 * @param (none) The function takes no arguments; all its inputs are the globals
 *        D_800A8F90, D_800A3190[0], D_800AB3B8, D_8009F834, D_8009F835,
 *        D_800A2970, D_8009F820 and D_8009F828.
 * @return Nothing. The results land in D_8009F828/D_8009F818, the grid heads in
 *         D_8009F820 and the node counter in D_8009F814.
 *
 * @par Confirmed
 * 1. The entity table D_800A8F90 is walked by count from the head of
 *    D_800A8F90.items; entries whose word at +0x08 is not positive are skipped.
 *    Per entity the z, x and radius words come from +0x5C, +0x64 and +0x68 and
 *    the flags word from +0x24.
 * 2. Clamp to the anchor D_800A3190[0]: per axis, when the distance to the
 *    anchor does not exceed the radius the anchor coordinate is used, otherwise
 *    the point moves one radius towards the anchor, so the result is the point of
 *    the disc nearest the anchor.
 * 3. Coverage test: cell column = (z >> 17) - D_8009F835 and row = (x >> 17) -
 *    D_8009F834, both rejected outside 0..4; cell = 16 bytes at D_800AB3B8 +
 *    (row * 5 + column) * 16, whose first word is the bit mask; the bit index is
 *    15 - (((z >> 15) & 3) * 4 + ((x >> 15) & 3)).
 * 4. On a hit the entity is appended to D_8009F818 (bounded by 256, counter from
 *    D_8009F828) and its hit flag is set.
 * 5. Entities with bit 4 of the flags word never reach the grid step. Entities
 *    without a hit reach it only when bit 0x100 is set.
 * 6. Grid step: column = (x >> 15) - D_800A2970.x and row = (z >> 15) -
 *    D_800A2970.z, both bounded by D_800A2970.w and .h. The byte
 *    D_8009F820[row * width + column] is read as the previous head, replaced by
 *    n, and node n of D_800A2990 receives the entity and the previous head; n
 *    starts at 1 and ends in D_8009F814.
 * 7. The counter is read from memory and written back, so the caller has to zero
 *    it for a fresh list, which func_80062FB4 does.
 *
 * @par Likely role
 * The single-view variant of the region gather used when only one small region
 * is active: it keeps the same coverage test and grid as func_80062874 but drops
 * the second anchor, block and list. The list it fills is the one the region
 * draw pass reads through D_8009F828 and D_8009F818.
 *
 * @par Unknown
 * What the 16 bits of a cell encode and in which space the anchor and the grid
 * live; the meaning of entity flag bits 4 and 0x100.
 *
 * @par Callers
 * func_80062FB4, the region setup, when D_8009FA0C == 1 (the small-view case).
 */
void gBuildSingleViewObjectGrid(void) {
    register s32 count __asm__("$12") = gCellEntityCount;
    Obj **scan = gCommandListHead.items;
    register s32 remaining __asm__("$13");
    register s32 cx __asm__("$16") = gViewGroundAnchors[0].x;
    register s32 cz __asm__("$25") = gViewGroundAnchors[0].z;
    Cell *cells = gCoverageGrid;
    register s32 ox __asm__("$23") = gGridOriginX;
    register s32 oz __asm__("$22") = gGridOriginY;
    register s32 width __asm__("$24") = gCellClipRect.w;
    register s32 height __asm__("$21") = gCellClipRect.h;
    register s32 bx __asm__("$20") = gCellClipRect.x;
    register s32 bz __asm__("$19") = gCellClipRect.z;
    Obj **output = gCellEntityList;
    register s32 n __asm__("$11") = 1;
    register Obj *o __asm__("$8");
    register s32 x __asm__("$9");
    register s32 z __asm__("$15");
    register s32 r __asm__("$4");
    s32 dx,dz,px,pz,col,row;
    register s32 hit __asm__("$10");
    u32 bits;
    register s32 sign __asm__("$3");
    Node *node;
    for (remaining = gCommandListHead.count; remaining > 0; remaining--) {
        o = *scan++;
        if (o->active <= 0) continue;
        z = o->z; x = o->x; r = o->radius;
        dx = x - cx;
        pz = z;
        sign = dx >> 31;
        if (((sign + dx) ^ sign) > r) goto xoutside;
        px = cx;
        goto xdone;
    xoutside:
        if (dx > 0) px = x-r;
        else px = x+r;
    xdone:
        dz = pz - cz;
        sign = dz >> 31;
        if (((sign + dz) ^ sign) <= r) pz = cz;
        else if (dz > 0) pz -= r;
        else pz += r;
        hit = 0;
        col = (pz >> 17) - oz;
        if ((u32)col < 5) {
            col *= 5;
            dz = (px >> 17) - ox;
            if ((u32)dz < 5) {
                col += dz;
                bits = cells[col].bits;
                if (bits == 0) goto after_hit;
                col = (pz >> 13) & 12;
                col += (px >> 15) & 3;
                if (bits & (1 << (15 - col))) {
                    if (count >= 256) continue;
                    hit = 1;
                    { Obj **q = output + count; *q = o; }
                    count++;
                }
            }
        }
    after_hit:
        if (o->flags & 4) continue;
        if (hit) {
            col = (x >> 15) - bx;
        } else if (o->flags & 0x100) {
            col = (x >> 15) - bx;
        } else {
            continue;
        }
        if (col < 0 || col >= width) continue;
        row = (z >> 15) - bz;
        if (row < 0 || row >= height) continue;
        {
            u8 *p;
            u8 old;
            col += row * width;
            p = &gCellMap[col];
            old = *p;
            *p = n;
            node = &gCellNodeArray[n++];
            node->obj = o;
            if (old == 0) {
                node->next = 0;
            } else {
                node->next = &gCellNodeArray[old];
            }
        }
        __asm__("" : : "r"(x), "r"(z));
    }
    gViewCount = n;
    gCellEntityCount = count;
}
