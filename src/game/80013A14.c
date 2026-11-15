// Categories: Graphics > GTE > Initialisation | Graphics > Renderer Setup
#include "game/types.h"

struct Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
};

extern void *D_8009FA1C;
extern void *D_8009F578;
extern void *D_8009F57C;
extern s16 D_8009F580;

extern void func_800959D0(s32 red, s32 green, s32 blue);
extern void func_800959A0(struct Matrix *matrix);
extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Sets the render background colour and allocates the three render
 *        buffers.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_800959D0(0x40, 0x40, 0x40) sets the colour, the matrix handed to
 * func_800959A0 has 0x1000 in the first column of every row (m[0][0], m[1][0],
 * m[2][0]) and zero elsewhere, then func_80017EBC allocates 0x400, 0x800 and
 * 0x100 bytes with tag 2 into D_8009FA1C, D_8009F578 and D_8009F57C, and
 * D_8009F580 (the depth/tag word) is reset to 0.
 *
 * @par Likely role
 * The one-time render set-up of a level: a 1 KB vertex block, a 2 KB packet
 * block and a 256-byte side buffer.
 */
void func_80013A14(void)
{
    struct Matrix color;

    func_800959D0(0x40, 0x40, 0x40);
    color.m[0][0] = color.m[1][0] = color.m[2][0] = 0x1000;
    color.m[0][1] = color.m[0][2] = color.m[1][1] = color.m[1][2] = color.m[2][1] =
        color.m[2][2] = 0;
    func_800959A0(&color);
    D_8009FA1C = func_80017EBC(0x400, 2);
    D_8009F578 = func_80017EBC(0x800, 2);
    D_8009F57C = func_80017EBC(0x100, 2);
    D_8009F580 = 0;
}
