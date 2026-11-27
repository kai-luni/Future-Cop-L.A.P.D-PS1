// Categories: Graphics > Map > Tile Renderer | Gameplay > Radar | Graphics > Effects > Gouraud Disc
#include "game/types.h"

typedef struct { s16 x, y, z; } Vec;
extern u8 gFrameTime __asm__("D_8009FAA4");
extern s32 gMarkerColorTable[] __asm__("D_800A6F60");
extern void gEmitGouraudDisc(Vec *, s32, s32, s32, s32, s32, s32) __asm__("func_8001484C");
void gDrawMinimapMarkers(u8 *obj, s32 *pos, s32 width, s32 height) __asm__("func_80066684");


/**
 * @brief Draws the point markers of one minimap object: up to eight
 *        slots, each a colored disc at the scaled map position.
 *
 * @param obj Marker object; flags at +0xA4, per-slot color indices at
 *            +0xA8, s16 coordinates at +0xB0/+0xB2 (4-byte stride).
 * @param pos Map origin (pos[0], pos[1]) subtracted from the coordinates.
 * @param width Visible width before the border offset.
 * @param height Visible height before the border offset.
 * @return Nothing.
 *
 * @par Confirmed
 * Returns when obj is null or !(flags & 0x492492). scale = D_8009FAA4 * 2,
 * offset = scale * 103 added to width and height. Per slot (3-bit stride,
 * active bit 1): x = (coord << 9) - pos[0]; drawn only when
 * x + offset >= 0 && x < width, with v.x = ((u32)(x * 5)) >> 12 (logical
 * right shift); y likewise.
 * v.z = 0x1000. func_8001484C(&v, D_800A6F60[color & 31] << 8, 0, 32,
 * scale, 0xE00, 0) draws the disc.
 *
 * @par Likely role
 * The *5 >> 12 factor maps world units to minimap cells; the offset is a
 * border margin around the visible map window.
 *
 * @par Callers
 * func_8006680C (minimap tile renderer).
 *
 * @par Environment path
 * Draws up to eight point markers of one map object as gouraud discs: it
 * returns at once for a null object or when no slot of the flag word +0xA4 is
 * active (mask 0x492492); for every 3-bit slot group with bit 1 set the s16
 * pair at +0xB0/+0xB2 (stride 4) is shifted << 9, offset by the map origin
 * pos[0]/pos[1], clipped against the window widened by 103 * scale, mapped to
 * screen with *5 >> 12 and drawn by gEmitGouraudDisc (func_8001484C) in
 * colour gMarkerColorTable[+0xA8[i] & 31]. The disc size scale is twice the
 * low byte of gFrameTime, so the markers pulse with the clock. Its only
 * caller is gDrawMapView (func_8006680C), the map-screen/radar renderer that
 * runs each frame the map is shown.
 */
void gDrawMinimapMarkers(u8 *obj, s32 *pos, s32 width, s32 height)
{
    u32 flags;
    s32 i, x, y, scale, offset;
    register u32 tmp __asm__("$2");
    register u32 q __asm__("$3");
    register u8 *p __asm__("$16");
    Vec v;
    if (!obj) return;
    flags = *(u32 *)(obj + 0xa4);
    if (!(flags & 0x492492)) return;
    i = 0;
    p = obj;
    __asm__("" : "+r"(p));
    v.z = 0x1000;
    scale = gFrameTime * 2;
    offset = scale * 103;
    width += offset;
    height += offset;
    for (; i < 8; flags >>= 3, i++, p += 4) {
        if (flags & 2) {
            x = (*(s16 *)(p + 0xb0) << 9) - pos[0];
            if (x + offset >= 0 && x < width) {
                tmp = x * 5;
                __asm__("" : "+r"(tmp));
                q = tmp >> 12;
                __asm__("" : "+r"(q));
                v.x = q;
                y = (*(s16 *)(p + 0xb2) << 9) - pos[1];
                if (y + offset >= 0 && y < height) {
                    tmp = y * 5;
                    __asm__("" : "+r"(tmp));
                    q = tmp >> 12;
                    __asm__("" : "+r"(q));
                    v.y = q;
                    gEmitGouraudDisc(&v, gMarkerColorTable[(obj + i)[0xa8] & 31] << 8, 0, 32, scale, 0xe00, 0);
                }
            }
        }
    }
}
