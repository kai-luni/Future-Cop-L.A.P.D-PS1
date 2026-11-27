// Categories: Platform > Sound > DMA | Platform > Interrupts
#include "game/types.h"

typedef void (*SpuDmaCallback)(void);

extern void gSetDmaCallback(s32 channel, SpuDmaCallback callback) __asm__("func_800914E0");

/**
 * @brief Arms the SPU transfer-complete event with a handler (channel 4).
 *
 * Fixed-channel wrapper: only the handler is supplied, the channel is the constant 4 - the SPU DMA channel whose completion event the sound driver uses. The pair goes to func_800914E0, the dispatcher that forwards it to the callback block behind D_8009DD10.
 *
 * @param callback Handler run when the channel-4 transfer completes; 0 removes it.
 * @return Nothing.
 *
 * @par Globals
 * None here; the handler ends up in the callback block D_8009DD10, which this function never touches directly.
 *
 * @par Control flow
 * Constant 4 into the first argument register, then a tail call.
 *
 * @par Calls
 * func_800914E0 - forwards (4, handler) to the driver's callback slot.
 *
 * @par Side effects
 * The SPU DMA completion handler changes; a null handler disarms it. gSpuInitDmaEvent (0x80096E68) registers func_800972F8 here, gSpuShutdownDmaEvent (0x80097A7C) passes 0 when it tears the event down.
 *
 * @par Callers
 * Confirmed: gSpuInitDmaEvent (0x80096E68) at SPU DMA init and gSpuShutdownDmaEvent (0x80097A7C) at its teardown.
 */
void gSpuSetDmaCallback(SpuDmaCallback callback) __asm__("func_800979F8");

void gSpuSetDmaCallback(SpuDmaCallback callback)
{
    gSetDmaCallback(4, callback);
}
