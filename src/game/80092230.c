// Categories: Graphics > GPU > Draw Environment | Graphics > Textures
#include "game/types.h"

/**
 * @brief Packs the texture-page parameter word from coordinates and mode
 *        bits (the tpage input of the E1h command).
 *
 * @param type Two mode bits placed at bits 7..8.
 * @param blend Two blend bits placed at bits 5..6.
 * @param x Texture-window x; bits 6..9 become the page-x field.
 * @param y Texture-window y; bit 8 goes to bit 4, bit 9 to bit 11.
 * @return The packed 16-bit word.
 *
 * @par Confirmed
 * ((type & 3) << 7) | ((blend & 3) << 5) | ((y & 0x100) >> 4) |
 * ((x & 0x3FF) >> 6) | ((y & 0x200) << 2). The layout matches the 0x9FF
 * mask of func_80093F58: 64-pixel pages in x, 256-pixel steps in y.
 *
 * @par Callers
 * func_80091F90.
 */
u16 func_80092230(s32 type, s32 blend, s32 x, s32 y)
{
    return ((type & 3) << 7)
        | ((blend & 3) << 5)
        | ((y & 0x100) >> 4)
        | ((x & 0x3FF) >> 6)
        | ((y & 0x200) << 2);
}
