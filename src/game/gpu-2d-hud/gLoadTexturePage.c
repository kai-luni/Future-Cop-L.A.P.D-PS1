// Categories: Graphics > GPU > VRAM Transfer | Graphics > Textures
#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect1F90;

extern void gLoadImage(void *rect, void *data) __asm__("func_80092F38");
extern u16 gGetTPage(s32 type, s32 blend, s32 x, s32 y) __asm__("func_80092230");
u16 gLoadTexturePage(void *data, s32 type, s32 blend, s32 x, s32 y, s32 w, s32 h) __asm__("func_80091F90");


/**
 * @brief Uploads one pixel type of an image block and returns its texture word.
 *
 * @param data Source block.
 * @param type Pixel size class: 0 = quarter width, 1 = half width, 2 = full width.
 * @param blend Blend/mode value forwarded to func_80092230.
 * @param x, y, w, h Source rectangle.
 * @return The texture-page word built by func_80092230(type, blend, x, y).
 *
 * @par Confirmed
 * The rectangle is narrowed according to @c type (w/4, w/2 or w) and sent with
 * func_80092F38, then the texture word is returned - the caller stores it in the
 * primitive it is about to emit.
 *
 * @par Likely role
 * The upload half of the texture loader: the same source block is sent once per
 * pixel type, and the returned word tells the renderer which page it landed in.
 *
 * @par Callers
 * None in the symbol table (inlined into the sprite/texture paths).
 *
 * @par Environment path
 * VRAM upload helper of the texture path: builds a rectangle at x,y with
 * height h and a width narrowed for the pixel type (4-bit, 8-bit or 15-bit),
 * sends it with func_80092F38 (the LoadImage-tagged transfer), and returns
 * func_80092230, which packs (type and 3) into bits 7-8, (blend and 3) into
 * bits 5-6, x shifted right 6 into the page X bits, y bit 8 into bit 4 and y
 * bit 9 into bit 11. No caller reaches it in the sources; only func_80092230
 * mentions it.
 */
u16 gLoadTexturePage(void *data, s32 type, s32 blend, s32 x, s32 y, s32 w, s32 h)
{
    Rect1F90 rect;

    rect.x = x;
    rect.y = y;
    rect.h = h;
    switch (type) {
    case 0:
        rect.w = w / 4;
        break;
    case 1:
        rect.w = w / 2;
        break;
    case 2:
        rect.w = w;
        break;
    }
    gLoadImage(&rect, data);
    return gGetTPage(type, blend, x, y);
}
