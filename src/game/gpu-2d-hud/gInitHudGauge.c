// Categories: Graphics > HUD | Gameplay > Player
#include "game/types.h"

struct Unk5A190Gauge {
    s32 unk00;
    s32 value;
    s32 unk08;
    s32 limit;
    u8 state;
    u8 unk11[3];
};
void gInitHudGauge(struct Unk5A190Gauge *gauge, s32 value, s32 divisor) __asm__("func_8005A0C4");


/**
 * @brief Initializes a 0x14-byte HUD/entity gauge block with range, scale factor, 1/3 threshold, and state.
 *
 * Clamps negative `value` to `0`, records `unk00 = divisor`, `value`, `limit = divisor / 3`, and `unk08 = 0x600000 / divisor`, and sets `gauge->state` (`+0x10`) to `2` above `limit`, `1` when `0 < value <= limit`, or `0` at zero. Category Graphics > HUD | Gameplay > Player follows gauge struct Unk5A190Gauge and callers func_800579D4/func_80060A0C.
 *
 * @param gauge Destination `Unk5A190Gauge` struct (`+0x00` max, `+0x04` value, `+0x08` scale, `+0x0C` limit, `+0x10` state).
 * @param value Initial gauge level (clamped to `>= 0`).
 * @param divisor Maximum gauge value used to derive `limit` (`divisor / 3`) and `unk08` (`0x600000 / divisor`).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Negative clamp on `value`, linear field initialization, then three-way branch setting `gauge->state` (`2`, `1`, or `0`).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Populates all 20 bytes (`unk00`, `value`, `unk08`, `limit`, `state`) of `*gauge`.
 *
 * @par Callers
 * Confirmed: func_800331CC (0x800335E4), func_800579D4 (0x80057E10), and func_80060A0C (0x80060B78 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void gInitHudGauge(struct Unk5A190Gauge *gauge, s32 value, s32 divisor)
{
    s32 limit;

    if (value < 0) {
        value = 0;
    }
    gauge->unk00 = divisor;
    gauge->value = value;
    limit = divisor / 3;
    gauge->limit = limit;
    gauge->unk08 = 0x600000 / divisor;
    if (value > limit) {
        gauge->state = 2;
    } else if (value > 0) {
        gauge->state = 1;
    } else {
        gauge->state = 0;
    }
}
