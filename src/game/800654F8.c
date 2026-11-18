// Categories: Graphics > HUD > Minimap | Graphics > Textures
#include "game/types.h"

extern u8 gRampVariantTable[] __asm__("D_800A2630");
extern u8 D_800A2730[];
extern s32 gDiscShadeByte __asm__("D_8009FAA4");

extern s32 func_80018534(u32);
void gBuildTilePattern(u32 input) __asm__("func_800654F8");


/**
 * @brief Generates the 256-byte minimap background tile: a sine interference
 *        pattern (command 0) or a blended copy of the source tile (command 1).
 *
 * @param input Command word; the top nibble selects the mode, the remaining
 *        bits carry the parameters.
 * @return Nothing; the result lands in D_800A2630.
 *
 * @par Confirmed
 * Mode 0: n = bits 16..19, step = 0x100 << n, the tile is walked as a
 * (step * 15 + 1) square grid; mode = bits 24..27 (0 flattens the row term),
 * shift = bits 20..23 + 8, phase = bits 8..15, base = D_8009FAA4 * bits 0..7,
 * mask = (0x1000 << n) - 1. Each cell samples func_80018534((base + x + row)
 * & mask) >> shift when the masked angle is below 0x1000, adds phase and
 * clamps to 0..255. Mode 1: base = bits 8..15 << (bits 0..7); 0x40 iterations
 * blend four bytes at a time from D_800A2730 with the plane at +0x100:
 * v = p < v ? 2 * p - v : v, then dst = (base + v) >> sh.
 *
 * @par Likely role
 * The animated minimap background: mode 0 renders a moving sine moire whose
 * scale follows the map zoom D_8009FAA4, mode 1 fades the static tile in.
 *
 * @par Unknown
 * The exact visual intent of the mask/shift parameters and who consumes
 * D_800A2630.
 *
 * @par Callers
 * func_8006531C, func_8006680C.
 *
 * @par Environment path
 * Builds the 256-byte pattern uploaded for a tileset; the tile grid calls it
 * whenever the tileset id of a region changes.
 */
void gBuildTilePattern(u32 input)
{
    register u32 arg __asm__("$4") = input;

    switch (arg >> 28) {
    case 0: {
        register u32 tmp __asm__("$2");
        register u32 n __asm__("$5");
        s32 step;
        u8 *out;
        s32 y;
        s32 mode;
        s32 shift;
        u32 p_tmp;
        s32 base;
        s32 phase;
        s32 mask;
        s32 x;

        tmp = arg >> 16;
        n = tmp & 15;
        out = gRampVariantTable;
        step = 0x100 << n;
        y = step * 15;
        mode = (arg >> 24) & 15;
        shift = ((arg >> 20) & 15) + 8;
        p_tmp = arg >> 8;
        base = gDiscShadeByte * (arg & 255);
        phase = p_tmp & 255;
        mask = (0x1000 << n) - 1;

        while (y >= 0) {
            s32 row = y;
            if (!mode) {
                row = 0;
            }
            for (x = step * 15; x >= 0; x -= step) {
                register s32 sum __asm__("$2") = x + row;
                register s32 a __asm__("$5") = base + sum;
                s32 value = 0;

                a &= mask;
                if (a < 0x1000) {
                    value = func_80018534(a) >> shift;
                }
                value += phase;
                value = value < 0 ? 0 : (value > 255 ? 255 : value);
                *out++ = value;
            }
            y -= step;
        }
        break;
    }
    case 1: {
        s32 base = (arg >> 8) & 0xFF;
        s32 sh = arg & 0xFF;
        u8 *dst;
        u8 *src;
        u8 *q;
        s32 cnt;

        base <<= sh;
        dst = gRampVariantTable;
        src = D_800A2730;
        q = src + 0x100;
        cnt = 0x40;

        while (cnt > 0) {
            s32 v0 = src[0];
            s32 v1 = src[1];
            s32 v2 = src[2];
            s32 p0 = q[0];
            s32 v3 = src[3];
            s32 p1 = q[1];
            s32 p2 = q[2];
            s32 p3 = q[3];

            if (p0 < v0) {
                v0 = p0 * 2 - v0;
            }
            if (p1 < v1) {
                v1 = p1 * 2 - v1;
            }
            if (p2 < v2) {
                v2 = p2 * 2 - v2;
            }
            if (p3 < v3) {
                v3 = p3 * 2 - v3;
            }

            dst[0] = (base + v0) >> sh;
            dst[1] = (base + v1) >> sh;
            dst[2] = (base + v2) >> sh;
            dst[3] = (base + v3) >> sh;

            src += 4;
            q += 4;
            dst += 4;
            cnt--;
        }
        break;
    }
    }
}
