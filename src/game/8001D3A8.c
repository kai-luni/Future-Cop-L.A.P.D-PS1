// Categories: Resources > File Access | Platform > CD-ROM
#include "game/types.h"

typedef struct {
    volatile u32 field0;
    u8 unk04[0x14];
} Unk800A6D78;

extern Unk800A6D78 D_800A6D78[];

/**
 * @brief Closes a file slot in D_800A6D78 unless an asynchronous read (state 2 or 3) is active.
 *
 * Reads the low byte of `D_800A6D78[index].field0` when `index < 4` and non-zero (otherwise `0xF7`), rejects active states `2` and `3` with `-0xF` without modifying the slot, and otherwise zeroes `D_800A6D78[index].field0` and returns the prior state byte (or `0xF7`). Category Resources > File Access | Platform > CD-ROM follows the four-slot D_800A6D78 file table and CD/stream callers.
 *
 * @param index File slot index `0..3` in D_800A6D78 (note: `index >= 4` still zeroes `D_800A6D78[index].field0` before returning `0xF7`).
 * @return `-0xF` if slot state is `2` or `3`, `0xF7` if `index >= 4` or `field0 == 0`, else `field0 & 0xFF`.
 *
 * @par Globals
 * D_800A6D78 - four-slot array of `0x18`-byte file/stream records (`field0` status word at `+0x00`).
 *
 * @par Control flow
 * Bounds/non-zero check selecting `value` (`field0 & 0xFF` or `0xF7`), busy check `(u32)(value - 2) < 2`, then store `0` and return `value`.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Clears `D_800A6D78[index].field0` to `0` when the slot is not in state `2` or `3`.
 *
 * @par Callers
 * Confirmed: func_8001E004 (0x8001E0E4), func_80029EB8 (0x80029F2C), and func_80088120 (0x800884A4 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_8001D3A8(u32 index)
{
    s32 value;

    if (index < 4) {
        if (D_800A6D78[index].field0 != 0) {
            value = D_800A6D78[index].field0 & 0xFF;
        } else {
            value = 0xF7;
        }
    } else {
        value = 0xF7;
    }

    if (value < 0) {
        goto store;
    }
    if ((u32)(value - 2) < 2) {
        return -0xF;
    }
store:
    D_800A6D78[index].field0 = 0;
    return value;
}
