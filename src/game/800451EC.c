// Categories: Platform > Sound
#include "game/types.h"

extern s32 D_8009F734;

/**
 * @brief Sets the global audio listener/spatialization mode D_8009F734.
 *
 * Confirmed: stores value directly into D_8009F734. Cross-referencing readers func_800458E8 and func_800460D4 shows D_8009F734 selects sound attenuation and stereo panning behavior: 0 forces full D_8009F330 volume with center pan 64, 1 attenuates/pans relative to D_800A1920[0], and 2 mixes/pans across two listener slots D_800A1920[0..1] (and extra->data->mode). Category Platform > Sound follows D_8009F734 usage in the sound mixer/panner and sound init caller func_80044F8C.
 * Likely: value corresponds to active local listener count (0=UI/menu, 1=single-player, 2=two-player split screen).
 * Unknown: behavior of values other than 0, 1, or 2 beyond falling into the default unattenuated branch in func_800458E8.
 *
 * @param value Listener/spatialization mode stored into D_8009F734 (0, 1, or 2 at known call sites).
 *
 * @par Globals
 * D_8009F734 - audio listener/spatialization mode (written).
 *
 * @par Control flow
 * Single assignment and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F734.
 *
 * @par Callers
 * Confirmed: func_80027EE4 (0x80027F3C, passes 0), func_80044F8C (0x80044F9C, passes 0), and func_80028618 (0x80028748 in asm/us/main.s, passing active D_8009FA58 count; edges.json lists 0x80028620) / Likely: none / Unknown: none.
 */
void func_800451EC(s32 value)
{
    D_8009F734 = value;
}
