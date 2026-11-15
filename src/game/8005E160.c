// Categories: Graphics > Model Renderer > Effects | Gameplay > Effects
#include "game/types.h"

extern void *D_800A22F0[];

extern void func_8005DFF0(void *effect);

/**
 * @brief Processes the four global effect slots.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * for i in 0..3: when D_800A22F0[i] != 0,
 * func_8005DFF0(D_800A22F0[i]) runs the model-renderer effect pass.
 *
 * @par Callers
 * func_8003F6E4 (scene traversal driver).
 */
void func_8005E160(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (D_800A22F0[i] != 0) {
            func_8005DFF0(D_800A22F0[i]);
        }
    }
}
