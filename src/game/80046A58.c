// Categories: Platform > Sound | Resources > Object Pool
#include "game/types.h"

extern u8 gSoundBankTableLow[] __asm__("D_800A1830");

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern s32 func_80046B04(s32 arg0, s32 arg1, s32 arg2, s32 slot);

/**
 * @brief Resolves a sound id to an active D_800A7938 bank slot and allocates a playback voice handle.
 *
 * Maps `id` through func_80045494 against primary table D_800A1830 (limit `0x47`; ids >= `0x46` switch to D_800A18D0 inside func_80045494); if `slot != 0xFF`, allocates or steals a `SoundVoice` node via func_80046B04. Category Platform > Sound | Resources > Object Pool follows sound-bank table D_800A1830 and voice-pool allocation.
 *
 * @param arg0 Initial volume forwarded to func_80046B04 (`voice->volume` and priority base).
 * @param arg1, arg2 Priority multiplier (`arg1 << 4`) and voice class/pan selector (`1` adds `+0x64` priority boost in func_80046B04).
 * @param id Sound effect or track id resolved via func_80045494.
 * @return Allocated `SoundVoice` handle from func_80046B04, or `0` when `slot == 0xFF` or voice allocation fails.
 *
 * @par Globals
 * D_800A1830 - primary sound-id-to-bank lookup table passed to func_80045494 with bound `0x47`.
 *
 * @par Control flow
 * Resolves `slot = func_80045494(id, D_800A1830, 0x47)`; single guard `slot != 0xFF` around func_80046B04.
 *
 * @par Calls
 * func_80045494 - resolves `id` to an active D_800A7938 bank index or `0xFF`; func_80046B04 - allocates or steals a `SoundVoice` slot for `(arg0, arg1, arg2, slot)`.
 *
 * @par Side effects
 * Allocates/steals a `SoundVoice` and links it into active sound lists via func_80046B04.
 *
 * @par Callers
 * Confirmed: func_80025E18 (0x80025ED4) and func_8007CACC (0x8007CCC8, 0x8007CD6C in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_80046A58(s32 arg0, s32 arg1, s32 arg2, s32 id)
{
    s32 handle;
    s32 slot;

    handle = 0;
    slot = func_80045494(id, gSoundBankTableLow, 0x47);
    if (slot != 0xFF) {
        handle = func_80046B04(arg0, arg1, arg2, slot);
    }
    return handle;
}
