// Categories: Platform > Audio | Resources > Buffer Management
#include "game/types.h"

extern s32 D_8009F280;

extern void func_8002A31C(s32 arg0, s32 arg1);
extern s32 func_8002A398(void);
extern void func_80022B28(s32 start, s32 length, s32 channel, u8 looping);

/**
 *  @brief Replaces a stored resource and configures two channel buffers.
 *
 *  Releases the previous resource if present, obtains a replacement, and
 *  uses two consecutive 0x2FC0-byte regions after its header when available.
 *  @param arg0 Unused argument.
 *  @return The newly stored resource value, or zero if none was obtained.
 *  @par Globals
 *  D_8009F280 holds the previous resource and is replaced with the new value.
 *  @par Control flow
 *  Conditionally releases the old resource and configures channels only for a nonzero result.
 *  @par Calls
 *  func_8002A31C releases the old value; func_8002A398 obtains a replacement.
 *  func_80022B28 configures channel 1 and then channel 0 without looping.
 *  @par Side effects
 *  Updates D_8009F280, performs a volatile header read, and invokes resource and channel operations.
 *  @par Callers
 *  Confirmed: func_80024470.
 */
s32 func_800246A8(s32 arg0)
{
    s32 *v1;
    u8 *s0;

    s0 = 0;
    if (D_8009F280 != 0) {
        func_8002A31C(D_8009F280, 1);
        D_8009F280 = 0;
    }
    v1 = (s32 *)func_8002A398();
    D_8009F280 = (s32)v1;
    if (v1 != 0) {
        (void)*(volatile s32 *)(v1 + 1);
        s0 = (u8 *)v1 + 0x1C;
    }
    if (s0 != 0) {
        func_80022B28((s32)(s0 + 0x2FC0), 0x2FC0, 1, 0);
        func_80022B28((s32)s0, 0x2FC0, 0, 0);
    }
    return D_8009F280;
}
