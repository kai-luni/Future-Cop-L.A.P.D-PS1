// Categories: Platform > DMA | Graphics > Image Transfer
#include "game/types.h"

extern s32 gMdecWaitInSync(void) __asm__("func_8009635C");
extern u32 func_80096484(void);

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
 */
s32 func_80096090(s32 mode)
{
    s32 result;

    if (mode == 0) {
        result = gMdecWaitInSync();
    } else {
        result = (func_80096484() >> 29) & 1;
    }

    return result;
}
