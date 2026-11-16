// Categories: Graphics > Palette | Graphics > Text
#include "game/types.h"

extern u16 D_8009AAE0[];

/**
 * @brief Writes one entry of the 20-colour text palette in 15-bit BGR form.
 *
 * D_8009AAE0 is a u16 array of 0x14 entries shared with the text renderer (func_80048F98 reads it while emitting glyphs, func_80048EC4 clears the matching colour rows). The value is packed like a PS1 CLUT entry - blue in bits 14..10, green in 9..5, red in 4..0 - hence the blue, green, red argument order.
 *
 * @param index Palette entry, 0..0x13; higher indices are ignored.
 * @param blue, green, red Colour components, 8 bits each (top 3 bits dropped).
 * @return Nothing.
 *
 * @par Globals
 * D_8009AAE0 - the text palette; exactly one halfword entry is overwritten.
 *
 * @par Control flow
 * Range test, two shifts, two ors, one halfword store; each component is masked to 8 bits, so its top 3 bits are dropped rather than scaled.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * One palette entry changes and stays changed until the next call; it becomes visible when the text pass uploads the palette to the GPU.
 *
 * @par Callers
 * Confirmed: func_8007BCF4 (0x8007BDD8) and func_8007F1F8 (0x8007F388), the list/profile screens that highlight the selected row.
 * Unknown: one call at 0x80079B64 from a routine that is not in the matching set.
 */
void func_800493C0(u32 index, u8 blue, u8 green, u8 red)
{
    if (index < 0x14) {
        D_8009AAE0[index] = (blue << 10) | (green << 5) | red;
    }
}
