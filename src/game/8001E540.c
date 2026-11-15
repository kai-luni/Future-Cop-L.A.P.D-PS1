// Categories: Platform > Device Shutdown | Input > Controller
#include "game/types.h"

extern u8 D_8009FA20;

extern void func_800200F0(void);
extern void func_8001E910(void);
extern void func_8001F158(void);
extern void func_80091574(void);

/**
 * @brief Shuts the controller/device layer down again.
 *
 * @return 0, always.
 *
 * @par Confirmed flow
 * func_800200F0() and func_8001E910() release the two sub-devices, then - only
 * if D_8009FA20 says the per-frame driver is running - func_8001F158() stops
 * it, and func_80091574() removes the interrupt hook. The flag is not cleared
 * here.
 *
 * @par Globals
 * D_8009FA20 (running flag, read only).
 *
 * @par Callers
 * func_80076FE0 - the counterpart of func_8001E4E4.
 */
s32 func_8001E540(void)
{
    func_800200F0();
    func_8001E910();
    if (D_8009FA20 != 0) {
        func_8001F158();
    }
    func_80091574();
    return 0;
}
