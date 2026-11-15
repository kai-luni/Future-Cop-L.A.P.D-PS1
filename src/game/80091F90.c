// Categories: Graphics > GPU > VRAM Transfer | Graphics > Textures
#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect1F90;

extern void func_80092F38(void *rect, void *data);
extern u16 func_80092230(s32 type, s32 blend, s32 x, s32 y);

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
 */
u16 func_80091F90(void *data, s32 type, s32 blend, s32 x, s32 y, s32 w, s32 h)
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
    func_80092F38(&rect, data);
    return func_80092230(type, blend, x, y);
}
