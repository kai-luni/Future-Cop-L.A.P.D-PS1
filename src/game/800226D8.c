// Categories: Platform > Streaming Audio | Platform > Sound > Voices
#include "game/types.h"

extern void func_80022A64(void);
extern void func_80022C68(void);

/**
 * @brief Stops active audio stream playback and resets all stream channel fields.
 *
 * Calls func_80022A64 to tear down SPU IRQ/transfer state, drain CD streaming, and release active voices in D_800A0968, then calls func_80022C68 to zero D_800A0968.array[i].field across all active stream channels.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; both callees operate on D_800A0968 (StreamState).
 *
 * @par Control flow
 * Sequential call to func_80022A64 followed by func_80022C68, then return.
 *
 * @par Calls
 * func_80022A64 - stops stream playback, disables SPU IRQ hook, and frees voice handles.
 * func_80022C68 - clears per-channel state field across D_800A0968.count slots.
 *
 * @par Side effects
 * Halts audio streaming, removes SPU IRQ callback, frees voice handles, and clears D_800A0968 channel fields.
 *
 * @par Callers
 * Confirmed: func_80022654 (0x80022684), func_800244A4 (0x800244D0 in asm/us/main.s), func_80024630 (0x80024668 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_800226D8(void)
{
    func_80022A64();
    func_80022C68();
}
