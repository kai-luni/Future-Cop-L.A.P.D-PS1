// Categories: Graphics > Fade Layers | Platform > GPU
#include "game/types.h"

extern u8 gLayerIndex __asm__("D_8009F225");

extern void func_80091F60(s32 arg0);
extern void func_8001BE68(void);

/**
 * @brief Initialises the fade/dimmer layer system with exactly two layers.
 *
 * func_80091F60(0) resets the layer hardware state, D_8009F225 = 2 is the layer
 * count every other function of the group loops over, and func_8001BE68()
 * sets the hardware up for that count.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F225 (layer count = 2; read by func_8001C040/8001C088/8001BFE4's
 * sibling code), D_800A06E0 (the layer array).
 *
 * @par Likely role
 * The two layers are what the game fades with: func_8001C088() enables them and
 * selects one, func_8001C040() sets its colour, func_800932F0() uploads the
 * result. Boot sets both to black before the first frame.
 *
 * @par Callers
 * func_80076EB0 - its return value is checked there, but this unit returns
 * nothing, so the caller only ever sees the leftover register value 0.
 */
void func_8001BF78(void)
{
    func_80091F60(0);
    gLayerIndex = 2;
    func_8001BE68();
}
