// Categories: Graphics > Fade Layers | Graphics > Colour
#include "game/types.h"

struct Unk1C040 {
    u8 unk00[0x18];
    u8 enabled;
    u8 red;
    u8 green;
    u8 blue;
    u8 unk1C[0x40];
};

extern u8 D_8009F225;
extern struct Unk1C040 D_800A06E0[];

/**
 * @brief Sets the colour of all fade layers.
 *
 * @param color 24-bit colour, split into the layer's red (bits 16-23), green
 *              (8-15) and blue (0-7) bytes. 0 makes every layer black.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * The loop runs over D_8009F225 layers (2 after func_8001BF78) and writes the
 * three bytes of D_800A06E0[i] at offsets 0x19/0x1A/0x1B; the layer's @c enabled
 * byte at 0x18 is left alone.
 *
 * @par Callers
 * func_80076EB0 only - twice with 0, after selecting each layer, so that the
 * first visible frame is guaranteed to be black.
 */
void func_8001C040(u32 color)
{
    s32 i;

    for (i = 0; i < D_8009F225; i++) {
        D_800A06E0[i].red = color >> 16;
        D_800A06E0[i].green = color >> 8;
        D_800A06E0[i].blue = color;
    }
}
