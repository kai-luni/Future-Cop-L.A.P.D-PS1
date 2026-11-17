// Categories: Graphics > Map > Tile Renderer | Gameplay > Radar
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Table { s8 pad[4]; s32 entries[100]; };
struct Grid { s32 unused; s32 width; s32 pad[7]; u32 cells[1]; };
struct Actor { u8 pad[0xc1]; s8 flag; u8 pad2[0x23e]; void *obj; };
struct Owner { u8 pad[0xac]; struct Actor *actor; };
extern s32 gPlayerCount[] __asm__("D_8009FA0C");
extern struct Table gTileDefTable __asm__("D_800A6F38");
extern struct Grid *gTileMapTable __asm__("D_8009F80C");
extern u8 *gTileBlockTable __asm__("D_8009F808");
extern s16 gTileColourWord __asm__("D_8009F83C");
extern s32 D_800A2960[], D_800A2964[];
extern s32 func_800665E8(s32 *, s32);
extern void func_800654F8(s32);
extern s32 func_80065744(s32);
extern void func_80065CA8(u8 *, s32, s32, s32);
extern void func_8004ED18(s32 *, s32 *, s32);
extern void func_800694C8(s32 *, s32 *, s32, s32);
extern void func_8001C214(s32, s32, s32, s32, s32, s32);
extern void func_80066684(void *, s32 *, s32, s32);

/**
 * @brief Draws the 16x16 tile block of the minimap around a position.
 *
 * @param owner Owner: actor pointer at +0xAC (whose obj and flag byte are used).
 * @param pos Position (two 32-bit values used as the block origin).
 * @return Nothing.
 *
 * @par Confirmed
 * func_800665E8(pos, scale) maps the position to the grid; the loops run
 * `row = x0 >> 4 .. (x0 + 24) >> 4` and `col = z0 >> 4 .. (z0 + size) >> 4`
 * (size 16 when D_8009FA0C[0] >= 2), read the cell values from
 * D_8009F80C->cells, clamp them to 0..53, look up the tile in the 100-entry
 * table D_800A6F38 and draw each with func_80065CA8(tile, index, x, y). The block
 * is then finished with func_8004ED18, func_800694C8, func_8001C214(0x7FFF, ...)
 * and func_80066684.
 *
 * @par Likely role
 * The minimap/radar renderer: a 54x54 cell grid (the clamp value), 16 pixels per
 * cell, drawn as a textured block and framed by the blended rectangle.
 *
 * @par Unknown
 * The meaning of the table field D_800A6F38.pad[1] (compared against the actor's
 * id).
 */
void func_8006680C(struct Owner *owner, s32 *pos)
{
    s32 x;
    s32 start[2];
    s32 end[2];
    s32 sx;
    s32 sy;
    s32 z0v[1];
    s32 x0v[1];
    s32 zfrac;
    s32 size;
    s32 colEnd;
    s32 rowEnd;
    register s32 half __asm__("$17");
    register s32 scale __asm__("$16");
    register s32 col __asm__("$21");
    register s32 row __asm__("$23");
    register u8 *tile __asm__("$20");
    s32 i, j, yy, xx, id, hi;
    s32 w, h, val, shift;
    s32 cb;
    s32 sc2;
    s32 r;
    s32 r2;
    s32 ax;

    size = 32;
    if (gPlayerCount[0] >= 2) size = 16;
    half = (u32)size >> 1;
    scale = ((half * 5) << 17) / 80;
    func_800665E8(pos, scale);
    zfrac = pos[2];
    z0v[0] = zfrac >> 13;
    scale = zfrac - scale;
    zfrac &= 0x1fff;
    zfrac *= 80;
    z0v[0] -= half;
    start[0] = scale;
    x = pos[0];
    gTileDefTable.pad[1] = -1;
    x0v[0] = x >> 13;
    start[1] = x - 0x18000;
    x &= 0x1fff;
    x *= 80;
    x0v[0] -= 12;
    for (row = x0v[0] >> 4; row <= (x0v[0] + 24) >> 4; row++) {
        for (col = z0v[0] >> 4; col <= (z0v[0] + size) >> 4; col++) {
            tile = *(u8 **)((gTileMapTable->cells[col + gTileMapTable->width * row] & 0xfffc) + (s32)gTileBlockTable);
            id = *(s8 *)(tile + 0x36f);
            if (id != -1 && id != gTileDefTable.pad[1]) {
                gTileDefTable.pad[1] = id;
                func_800654F8(gTileDefTable.entries[id]);
            }
            i = 0;
            cb = col << 4;
            hi = func_80065744(*(s8 *)(tile + 0x3c9));
            gTileColourWord = (hi << 8) | func_80065744(*(s8 *)(tile + 0x3c8));
            for (; i < 16; i++) {
                yy = (row << 4) + i - x0v[0];
                if (yy < 0) continue;
                if (yy >= 26) break;
                for (j = 0; j < 16; j++) {
                    xx = cb + j - z0v[0];
                    if (xx < 0) continue;
                    if (xx > size) break;
                    func_80065CA8(tile, (i << 4) + j, ((xx * 5) << 17) - zfrac, ((yy * 5) << 17) - x);
                }
            }
        }
    }
    sc2 = ((size * 5) << 17) / 80;
    end[0] = start[0] + sc2;
    end[1] = start[1] + 0x30000;
    func_8004ED18(start, end, owner->actor->flag);
    func_800694C8(&sx, &sy, start[0], start[1]);
    w = sc2 * (D_800A2960[0] >> 8);
    w >>= 24;
    val = sx + w;
    if (val >= 0) { if (val < 54) r = val; else r = 53; } else r = 0;
    sx = r - w;
    h = ((D_800A2964[0] >> 8) * 3) << 16;
    h >>= 24;
    val = sy + h;
    if (val >= 0) { if (val < 54) r2 = val; else r2 = 53; } else r2 = 0;
    sy = r2 - h;
    shift = owner->actor->flag ? 80 : 0;
    ax = sx + 13;
    func_8001C214(0x7fff, (shift + ax) * 2, (sy + 17) * 2, w * 2, h * 2, 1);
    func_80066684(owner->actor->obj, start, ((size * 5) << 17) / 80, 0x30000);
}
