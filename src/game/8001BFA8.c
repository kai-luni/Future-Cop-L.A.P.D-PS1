// Categories: Graphics > Display Setup | Graphics > Fade Layers
#include "game/types.h"

extern s8 D_8009F224;
extern u8 D_8009F608;
extern void func_8001C088(s32, u8);

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
 */
void func_8001BFA8(void)
{
    func_8001C088((1 - D_8009F224) & 0xFF, D_8009F608);
}
