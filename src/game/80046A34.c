// Categories: Platform > Sound > Voices
#include "game/types.h"

extern void func_80046698(s32 handle);

/**
 * @brief Stops a sound voice through its handle, ignoring the null handle.
 *
 * Callers clear their copy of a voice handle after use; this wrapper turns the zero handle (nothing playing) into a silent no-op instead of a fault, and hands everything else to the voice release.
 *
 * @param handle Voice handle as returned by the effect starter; 0 means idle.
 * @return Nothing.
 *
 * @par Globals
 * None directly; func_80046698 rewrites the voice lists kept behind D_8009F73C through D_8009F758.
 *
 * @par Control flow
 * One branch: the call is skipped when the handle is zero.
 *
 * @par Calls
 * func_80046698 - marks the voice free, unhooks it from its priority list and returns it to its bank list.
 *
 * @par Side effects
 * The voice stops and the engine's free lists change; the caller's handle variable is not cleared here.
 *
 * @par Callers
 * Confirmed: func_800260B8 (0x80026438, 0x800266F0), func_80027510 (0x8002760C), func_8007CA34 (0x8007CA80), func_8007CACC (0x8007CC98, 0x8007CD3C, 0x8007CDDC), func_80025E18 (0x80025E80), func_80022A64 (0x80022AE8), func_800474F0 (0x8004752C) and the data block D_8007D150 (indirect call at 0x8007D170).
 * Unknown: one call at 0x80027254 from a routine that is not in the matching set.
 */
void func_80046A34(s32 handle)
{
    if (handle != 0) {
        func_80046698(handle);
    }
}
