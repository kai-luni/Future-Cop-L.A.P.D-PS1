// Categories: Graphics > Display Setup | Graphics > Fade Layers
#include "game/types.h"

extern s8 gSelectedLayer __asm__("D_8009F224");
extern u8 gDisplayEnabled __asm__("D_8009F608");
extern void gSelectFadeLayer(s32, u8) __asm__("func_8001C088");
void gFlipDisplayLayer(void) __asm__("func_8001BFA8");


/**
 * @brief Flips to the other display buffer.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_8001C088((1 - D_8009F224) & 0xFF, D_8009F608): the buffer index
 * D_8009F224 (0/1) is inverted and passed with the byte D_8009F608 to the
 * display/fade setup.
 *
 * @par Callers
 * func_8001EFF8.
 *
 * @par Role and path
 * Flips the active display/fade layer by calling func_8001C088 with (1 -
 * gSelectedLayer) & 0xFF and the current gDisplayEnabled (D_8009F608) flag.
 * Called by func_8001EFF8 during frame display flipping.
 */
void gFlipDisplayLayer(void)
{
    gSelectFadeLayer((1 - gSelectedLayer) & 0xFF, gDisplayEnabled);
}
