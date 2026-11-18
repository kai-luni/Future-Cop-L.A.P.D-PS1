// Categories: Graphics > Video Layer
#include "game/types.h"

extern u8 gLayerOpenResult __asm__("D_8009F504");

/**
 * @brief Reports whether the video/image layer is open.
 *
 * @return D_8009F504: 1 after gOpenVideoLayer() opened the layer, 0 after
 *         gCloseVideoLayer() closed it.
 *
 * @par Callers
 * gDrawImageResource uses it to decide whether it has to open the layer itself
 * (and therefore close it again) or can draw into an already open one.
 */
u8 gIsVideoLayerOpen(void) __asm__("func_80087D3C");

u8 gIsVideoLayerOpen(void)
{
    return gLayerOpenResult;
}
