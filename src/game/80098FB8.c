// Categories: Platform > Sound | Platform > DMA
#include "../../include/game/types.h"

extern s32 gSpuTransferCallback __asm__("D_8009EEDC");

/**
 * @brief Installs the SPU DMA transfer-complete callback.
 *
 * @param value Handler run when the transfer completes; 0 removes it.
 * @return Nothing.
 *
 * @par Confirmed
 * Compare-and-write of gSpuTransferCallback; no other effect. func_800972F8
 * calls the word when it is set and otherwise falls back to
 * func_8008DFF0(0xF0000009, 0x20), and func_80098E28/func_80098E88 test it
 * the same way, so the word is the transfer-complete callback of the SPU DMA
 * event, not a state value. The neighbour D_8009EEE0 is the cached interrupt
 * handler of gSpuInstallIrqCallback.
 *
 * @par Likely role
 * The sound driver installs its own handler here before queueing SPU DMA
 * traffic, and gShutdownMovieAudio/gInitMovieAudio save and restore it.
 *
 * @par Unknown
 * Whether the interrupt side calls the word as well; only the three polling
 * sites show a call.
 *
 * @par Callers
 * func_800211FC, gShutdownMovieAudio, gInitMovieAudio.
 */
void gSpuSetTransferCallback(s32 value) __asm__("func_80098FB8");

void gSpuSetTransferCallback(s32 value)
{
    if (value != gSpuTransferCallback) {
        gSpuTransferCallback = value;
    }
}
