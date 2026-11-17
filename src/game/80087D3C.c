// Categories: Graphics > Video Layer
#include "game/types.h"

extern u8 gLayerOpenResult __asm__("D_8009F504");

/**
 * @brief Reports whether the video/image layer is open.
 *
 * @return D_8009F504: 1 after func_80087EA0() opened the layer, 0 after
 *         func_80087D48() closed it.
 *
 * @par Callers
 * func_80089D44 uses it to decide whether it has to open the layer itself
 * (and therefore close it again) or can draw into an already open one.
 */
u8 func_80087D3C(void)
{
    return gLayerOpenResult;
}
