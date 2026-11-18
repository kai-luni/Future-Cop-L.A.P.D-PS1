// Categories: Graphics > 2D Drawing > Lines | Utilities > Mathematics > Trigonometry
#include "game/types.h"

struct LineF2 {
    u32 tag;
    u32 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
};

extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");

extern void gSinCos(s32 id, s32 *first, s32 *second) __asm__("func_8001846C");

/**
 * @brief Emits a line primitive from an origin along an angle.
 *
 * @param origin Packed origin (x/y).
 * @param id Angle in the engine's 4096-step convention.
 * @return Nothing; the 0x1C-byte primitive comes from D_8009FA90 (D_8009F9C0 is
 *         the pool end, checked before the write).
 *
 * @par Confirmed
 * func_8001846C(id, &first, &second) supplies sine and cosine from the table,
 * the primitive gets `code`, a tag built from 0x03000000/0x02C00000 and the
 * colour 0x40808080, and the two endpoints in x0/x1 and y0/y1.
 *
 * @par Likely role
 * The free-hand/aim line of the HUD: one call per angle, 28 bytes each.
 */
void func_8005A740(s32 origin, s32 id)
{
    struct LineF2 *line;
    s32 first;
    s32 second;

    gSinCos(id, &first, &second);
    line = (struct LineF2 *)gPacketCursor;
    if (gPacketCursor < gPacketLimit) {
        gPacketCursor += 0x1C;
    }
    line->tag = 0x3000000;
    line->code = 0x40808080;
    line->x0 = (origin + second * 24) >> 16;
    line->y0 = (first * 24 + 0x2C0000) >> 16;
    line->x1 = (origin + second * 30) >> 16;
    line->y1 = (first * 30 + 0x2C0000) >> 16;
}
