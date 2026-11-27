// Categories: Graphics > 2D Drawing > Rectangles | Graphics > Blending
#include "game/types.h"

struct HalfRect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
};

extern void gEmitSolidTileRect(u8 red, u8 green, u8 blue, struct HalfRect *rect, u8 blend) __asm__("func_8001C148");
void gEmitTranslucentTileRect(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");


/**
 * @brief Draws a blended rectangle, converting colour and coordinates.
 *
 * @param color Packed colour: red from bits 8-15, green from bits 3-10, blue
 *              from bits 0-4, each widened to 8 bits by masking with 0x7C /
 *              shifting by 2 (5-bit to 8-bit expansion).
 * @param x, y, w, h Rectangle in the caller's (double-width) coordinates; all
 *        four are halved before use.
 * @param blend Blend mode byte, forwarded unchanged.
 * @return Nothing.
 *
 * @par Confirmed
 * The rectangle is built with x >> 1, y >> 1, w >> 1, h >> 1 and handed to
 * func_8001C148(red, green, blue, &rect, blend), which emits the primitive.
 *
 * @par Likely role
 * The 2D overlay path: the caller works in 320x240-ish units while the target
 * rectangle is expressed in half resolution (the engine stores two "screens" of
 * 160 pixels side by side, see func_8002549C).
 *
 * @par Callers
 * func_800260B8, func_800267C8, func_80027510, func_80047BE8, func_80047F30,
 * func_8005AB8C, func_8006680C and two inlined sites.
 *
 * @par Environment path
 * Converts 15-bit color and screen coordinates (x, y, w, h) and delegates to
 * func_8001C148 to emit a solid or semi-transparent GP0 0x62 TILE rectangle
 * paired with a GP0 0xE1 DR_TPAGE packet.
 */
void gEmitTranslucentTileRect(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend)
{
    struct HalfRect rect;

    rect.x = x >> 1;
    rect.y = y >> 1;
    rect.w = w >> 1;
    rect.h = h >> 1;
    gEmitSolidTileRect((color >> 8) & 0x7C, (color >> 3) & 0x7C, (color & 0x1F) << 2, &rect, blend);
}
