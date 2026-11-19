// Categories: Graphics > GTE > Initialisation | Graphics > Renderer Setup
#include "game/types.h"

struct Matrix {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
};

extern void *gLevelVertexBlock __asm__("D_8009FA1C");
extern void *gPartVertexBuffer __asm__("D_8009F578");
extern void *gClipFlagBuffer __asm__("D_8009F57C");
extern s16 gModelUvOffset __asm__("D_8009F580");

extern void func_800959D0(s32 red, s32 green, s32 blue);
extern void func_800959A0(struct Matrix *matrix);
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

void gInitModelRendererBuffers(void) __asm__("func_80013A14");

/**
 * @brief Initializes GTE background and color state and allocates three model
 * renderer buffers.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Sets the color triplet to 0x40 and builds a 0x1000-diagonal matrix for the
 * GTE color-matrix helper. Calls the allocator with sizes 0x400, 0x800, and
 * 0x100, each with tag 2. Clears the UV offset halfword.
 *
 * @par Globals
 * Writes gLevelVertexBlock, gPartVertexBuffer, gClipFlagBuffer, and
 * gModelUvOffset.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80024A24.
 *
 * @par Role and path
 * Engine graphics startup prepares GTE state and storage shared by the model
 * rendering path.
 *
 * @par Unknown
 * The allocator tags have meaning outside this routine.
 */
void gInitModelRendererBuffers(void)
{
    struct Matrix color;

    func_800959D0(0x40, 0x40, 0x40);
    color.m[0][0] = color.m[1][0] = color.m[2][0] = 0x1000;
    color.m[0][1] = color.m[0][2] = color.m[1][1] = color.m[1][2] = color.m[2][1] =
        color.m[2][2] = 0;
    func_800959A0(&color);
    gLevelVertexBlock = gAllocMemory(0x400, 2);
    gPartVertexBuffer = gAllocMemory(0x800, 2);
    gClipFlagBuffer = gAllocMemory(0x100, 2);
    gModelUvOffset = 0;
}
