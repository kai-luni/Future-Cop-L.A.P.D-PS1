// Categories: Input > Controller Init | Platform > Device Init
#include "game/types.h"

extern s32 D_8009FAD8;
extern u8 D_8009FA20;

extern void func_80091480(void);
extern void func_80095878(void);
extern void func_800200A8(void);
extern void func_8001E87C(void);
extern void func_8001E588(u32 port);

/**
 * @brief Initialises the controller/device layer for both ports.
 *
 * @return 0, always - the caller's error check never triggers.
 *
 * @par Confirmed flow
 * func_80091480(), func_80095878() and func_800200A8() reset the three
 * sub-devices, D_8009FAD8 = 0, func_8001E87C() installs the device hooks,
 * D_8009FA20 = 0 marks the layer as "not running yet", and func_8001E588(port)
 * is called for port 0 and port 1 (the two controller ports).
 *
 * @par Globals
 * D_8009FAD8 (device state), D_8009FA20 (per-frame-callback enabled flag, set
 * later by func_8001F0B8).
 *
 * @par Likely role
 * The pad/device bring-up of the boot sequence; func_8001E540() is its
 * teardown counterpart.
 *
 * @par Callers
 * func_80076EB0, once, and only after func_8008DEA0() succeeded.
 */
s32 func_8001E4E4(void)
{
    u32 port;

    func_80091480();
    func_80095878();
    func_800200A8();
    D_8009FAD8 = 0;
    func_8001E87C();
    D_8009FA20 = 0;
    for (port = 0; port < 2; port++) {
        func_8001E588(port);
    }
    return 0;
}
