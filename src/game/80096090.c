// Categories: Platform > DMA | Graphics > Image Transfer
#include "game/types.h"

extern s32 gMdecWaitInSync(void) __asm__("func_8009635C");
extern u32 gIsMdecBusy(void) __asm__("func_80096484");
s32 gMdecInSync(s32 mode) __asm__("func_80096090");


/**
 * @brief Queries a transfer: wait for completion (mode 0) or read a status bit.
 *
 * @param mode 0 = blocking wait, anything else = read the flag.
 * @return 0 when the busy flag cleared (mode 0), -1 on timeout (mode 0), and
 *         bit 29 of the status word for other modes.
 *
 * @par Confirmed
 * mode 0 runs func_8009635C, which spins on `*D_8009E9F4 & 0x20000000` with a
 * 0x100000-iteration timeout and prints the message at D_80012BA8 on timeout.
 * Otherwise `(func_80096484() >> 29) & 1` is returned.
 *
 * @par Environment path
 * Video decode support, not a draw routine: blocks until the decode-input
 * side is idle (mode 0, timeout inside func_8009635C) or returns bit 29 of
 * the decoder status. func_800861E4 and the playback resets func_80085A24 and
 * func_80085AC4 call it with 0.
 */
s32 gMdecInSync(s32 mode)
{
    s32 result;

    if (mode == 0) {
        result = gMdecWaitInSync();
    } else {
        result = (gIsMdecBusy() >> 29) & 1;
    }

    return result;
}
