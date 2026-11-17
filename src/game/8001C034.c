// Categories: Graphics > Display State
#include "game/types.h"

extern u8 gSelectedLayer __asm__("D_8009F224");

/**
 * @brief Returns the current screen-offset selector.
 *
 * @return The byte D_8009F224, which func_8001C088() writes: it is the index of
 *         the fade layer / screen offset that is currently selected (0 or 1 in
 *         the boot path). Callers map it to the low-resolution x offset: 0 when
 *         it is 0, 0x100 when it is non-zero.
 *
 * @par Callers
 * func_8001B2C8 and func_8001B4A8 (frame buffer size/offset), and the unit at
 * 0x80088124 (image drawing).
 */
u8 func_8001C034(void)
{
    return gSelectedLayer;
}
