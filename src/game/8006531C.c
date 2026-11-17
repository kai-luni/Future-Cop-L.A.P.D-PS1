// Categories: Graphics > Map > Tile Renderer | Gameplay > Radar
#include "game/types.h"

extern s32 gTileShapeTable[] __asm__("D_8009B1A4");
extern s32 *gCamera __asm__("D_8009F9E4");
struct Table { s8 pad[4]; s32 entries[100]; };
extern struct Table gTileDefTable __asm__("D_800A6F38");
extern s32 gTableBuffer __asm__("D_8009FA08");
extern s16 gTileColourWord __asm__("D_8009F83C");
extern void func_800654F8(s32);
extern s32 func_80065744(s32);
extern void func_800646BC(void *, s32, void *, s32);
extern void func_80065134(void *, s32, s32, s32, s32);

/**
 * @brief Draws a 5x5 grid of map regions: copies the tile header into the
 *        scratchpad, selects tilesets and palettes per region and renders
 *        the masked sub-cells.
 *
 * @param x Grid origin x (world units, << 8 internally).
 * @param y Grid origin y.
 * @param region Array of 4-word entries: mask, object pointer, two
 *               parameters forwarded to the tile pass.
 * @return Nothing.
 *
 * @par Confirmed
 * Copies 0x70 bytes from D_8009B1A4 to the scratchpad at 0x1F800000.
 * dx = (D_8009F9E4[2] >> 9) - (x << 8), dy likewise from [0]. Per region:
 * obj[0x36F] is the tileset id; on change, func_800654F8 selects
 * D_800A6F38.entries[id] (100-entry table). D_8009F83C =
 * (func_80065744(obj[0x3C9]) << 8) | func_80065744(obj[0x3C8]).
 * func_800646BC(obj, base, region + 2, mask << 16) draws the textured
 * surface, then a 4x4 sub-cell loop (a = 0xC0..0 step 0x40, b = 0xC..0
 * step 4) calls func_80065134(obj, base, dx, dy, a + b) for each set mask
 * bit. dx advances by -0x100 per column (+0x500 row wrap), dy by -0x100
 * per row; base = D_8009FA08.
 *
 * @par Callers
 * func_80066E98 (region drawing).
 */
void func_8006531C(s32 x, s32 y, s32 *region)
{
    s32 *src = gTileShapeTable;
    s32 *dst = (s32 *)0x1F800000;
    s32 base = gTableBuffer;
    s32 dx = (gCamera[2] >> 9) - (x << 8);
    s32 dy = (gCamera[0] >> 9) - (y << 8);
    s32 rows, cols, a, b, hi;
    s32 id;
    register s32 value __asm__("$2");
    register s32 mask __asm__("$18");
    register s8 *obj __asm__("$19");
    gTileDefTable.pad[1] = -1;
    do { *dst++ = *src++; } while ((s32)dst <= 0x1F80006F);
    for (rows = 5; rows > 0; rows--) {
        for (cols = 5; cols > 0; cols--) {
            value = region[0];
            __asm__ volatile ("" : "+r"(value));
            if (value != 0) {
                obj = (s8 *)region[1];
                mask = value;
                id = obj[0x36F];
                if (id != -1 && id != gTileDefTable.pad[1]) {
                    gTileDefTable.pad[1] = id;
                    func_800654F8(gTileDefTable.entries[id]);
                }
                hi = func_80065744(obj[0x3C9]);
                gTileColourWord = (hi << 8) | func_80065744(obj[0x3C8]);
                func_800646BC(obj, base, region + 2, mask << 16);
                for (a = 0xC0; a >= 0; a -= 0x40) {
                    for (b = 0xC; b >= 0; b -= 4) {
                        if (mask & 1) func_80065134(obj, base, dx, dy, a + b);
                        mask >>= 1;
                    }
                }
            }
            dx -= 0x100;
            region += 4;
        }
        dx += 0x500;
        dy -= 0x100;
    }
}
