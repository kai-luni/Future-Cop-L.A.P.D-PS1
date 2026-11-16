// Categories: Platform > Sound > Voices
#include "game/types.h"

void func_8002238C(s32 arg0);

/**
 * @brief Releases the hardware sound channel bound to a voice descriptor.
 *
 * Reads the signed channel index at arg0 + 0x0D and passes it to func_8002238C, which keys off/stops channel mask (1 << channel) via func_80021A8C.
 *
 * @param arg0 Voice descriptor pointer containing the signed channel byte at +0x0D.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line signed byte load from arg0 + 0x0D and tail call to func_8002238C.
 *
 * @par Calls
 * func_8002238C - stops SPU/driver channel (1 << channel) via func_80021A8C.
 *
 * @par Side effects
 * Stops the hardware sound channel associated with the voice record.
 *
 * @par Callers
 * Confirmed: func_80046698 (0x800466B8), func_80047088 (0x80047124), func_800471C4 (0x80047228).
 */
void func_800243F8(s8 *arg0) {
    func_8002238C(*(arg0 + 0xD));
}
