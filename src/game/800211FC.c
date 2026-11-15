// Categories: Graphics > Sprite/Asset Subsystem | Platform > Interrupts
#include "game/types.h"

extern s32 D_8009F9AC;
extern s32 D_8009FA14;
extern s32 D_8009FA80;
extern s32 D_8009F998;
extern s32 D_8009FA34;
extern s32 D_8009FAD0;
extern s32 D_8009F9C8;

extern void D_8002139C(void);
extern s32 func_80098FB8(void (*callback)(void));
extern s32 func_80098F78(void);
extern void func_80098F48(s32 value);

/**
 * @brief Initializes the subsystem's interrupt side: installs the
 *        callback, queries the mode and clears the state words.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F9AC = func_80098FB8(D_8002139C) - the callee stores the callback
 * address in D_8009EEDC; D_8009FA14 = func_80098F78() (mode query over
 * D_8009EEC0); func_80098F48(0) (disable form). D_8009FA80, D_8009F998,
 * D_8009FA34, D_8009FAD0 are zeroed, D_8009F9C8 = 1.
 *
 * @par Likely role
 * D_8002139C is the per-frame hook the interrupt path dispatches; the
 * state words gate the asset/sprite processing.
 *
 * @par Callers
 * func_8002333C.
 */
void func_800211FC(void)
{
    D_8009F9AC = func_80098FB8(D_8002139C);
    D_8009FA14 = func_80098F78();
    func_80098F48(0);
    D_8009FA80 = 0;
    D_8009F998 = 0;
    D_8009FA34 = 0;
    D_8009FAD0 = 0;
    D_8009F9C8 = 1;
}
