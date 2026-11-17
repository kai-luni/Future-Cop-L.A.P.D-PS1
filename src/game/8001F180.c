// Categories: Platform > Memory Card | Resources > Save Data
#include "game/types.h"

struct Unk8001F180 {
    u8 pad[0x8F4];
    s32 field_8F4;
    s32 field_8F8;
};

extern struct Unk8001F180 *gOperationState __asm__("D_8009AB08");

/**
 * @brief Checks whether the memory-card manager is busy with an active state or queued command.
 *
 * Reads D_8009AB08->field_8F4 (active operation state) and field_8F8 (queued command code) and
 * returns 1 if either word is non-zero, or 0 when both are zero (idle).
 *
 * @param void No parameters (callers func_8001F1B8/func_8001F24C/func_8001F2E0 pass unused arguments).
 * @return 1 if field_8F4 or field_8F8 is non-zero; 0 if the card manager is idle.
 *
 * @par Globals
 * D_8009AB08 - memory-card manager state struct (reads +0x8F4 state and +0x8F8 command).
 *
 * @par Control flow
 * Two sequential non-zero branches returning 1 if either field_8F4 or field_8F8 is set, else 0.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8001F1B8 (0x8001F1DC), func_8001F24C (0x8001F270), func_8001F2E0 (0x8001F304), and func_80020B64 (0x80020C2C) / Likely: none / Unknown: none.
 */
s32 func_8001F180(void)
{
    struct Unk8001F180 *state = gOperationState;

    if (state->field_8F4 != 0) goto one;
    if (state->field_8F8 != 0) goto one;
zero:
    return 0;
one:
    return 1;
}
