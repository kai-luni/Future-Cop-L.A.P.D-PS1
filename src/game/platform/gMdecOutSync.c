// Categories: Platform > DMA | Graphics > Image Transfer
#include "game/types.h"

extern s32 gMdecWaitOutSync(void) __asm__("func_800963F0");
extern u32 gIsMdecBusy(void) __asm__("func_80096484");
s32 gMdecOutSync(s32 mode) __asm__("func_800960CC");


/**
 * @brief Queries a transfer: wait for completion (mode 0) or read a status bit.
 *
 * @param mode 0 = blocking wait, anything else = read the flag.
 * @return 0 when the busy flag cleared (mode 0), -1 on timeout (mode 0), and
 *         bit 24 of the status word for other modes.
 *
 * @par Confirmed
 * mode 0 runs func_800963F0 (spin on `*D_8009E9D4 & 0x01000000`, timeout 0x100000,
 * message D_80012BB8), otherwise `(func_80096484() >> 24) & 1`.
 *
 * @par Compare
 * func_80096090 is the same pair for the other descriptor (bit 0x20000000 / bit
 * 29).
 *
 * @par Environment path
 * Video decode support rather than primitive drawing: blocks until the
 * decode-out transfer finishes (mode 0, with timeout handling inside
 * func_800963F0) or returns its busy bit. The strip uploaders func_80085228
 * and func_80085630 and the playback reset func_80085A24 and func_80085AC4
 * call it with 0 before reusing the output buffer.
 */
s32 gMdecOutSync(s32 mode)
{
    s32 result;

    if (mode == 0) {
        result = gMdecWaitOutSync();
    } else {
        result = (gIsMdecBusy() >> 24) & 1;
    }

    return result;
}
