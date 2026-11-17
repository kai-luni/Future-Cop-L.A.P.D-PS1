// Categories: Platform > CD-ROM | Platform > Hardware Init
#include "game/intc_globals.h"

#include "game/dma_globals.h"

#include "game/types.h"

typedef struct {
    u16 active;
    u8 pad02[0x30];
    u16 value;
    u32 data;
    u8 payload[1];
} Context;

extern Context gFrameClockAnchor __asm__("D_8009CC88");
extern u16 *gInterruptMaskPtr __asm__("D_8009DD18");
extern u32 *gDmaControlPtr __asm__("D_8009DD1C");
extern void func_80091B80(void *);
extern void func_8008C9B0(void);

/**
 * @brief Starts one CD device-transfer context and arms the hardware.
 *
 * Context D_8009CC88 is checked for "active"; when idle, func_80091B80 prepares
 * the payload, the active flag is set and the context's value/data words are
 * published to D_8009DD18 and D_8009DD1C, after which func_8008C9B0 runs the
 * hardware start sequence.
 * @return The context pointer, or 0 when it was already busy.
 * @par Globals
 * D_8009CC88 - context (active 0x00, value 0x32, data 0x34);
 * D_8009DD18/D_8009DD1C - published hardware words.
 * @par Control flow
 * Busy guard, then the start sequence.
 * @par Calls
 * func_80091B80 - payload prepare. func_8008C9B0 - hardware start.
 * @par Side effects
 * Marks the context busy and writes hardware words.
 * @par Callers
 * Confirmed: the CD/interrupt block around func_80091270.
 */
Context *func_80091AA4(void)
{
    Context *ctx;
    Context *result;
    register u32 value __asm__("$2");
    register u32 *dest __asm__("$3");
    __asm__("lui %0,%%hi(D_8009CC88)\n\taddiu %0,%0,%%lo(D_8009CC88)" : "=r"(ctx));
    if (ctx->active != 0)
        goto busy;
    func_80091B80(ctx->payload);
    ctx->active = 1;
    *gInterruptMaskPtr = ctx->value;
    dest = gDmaControlPtr;
    __asm__ volatile("" : "+r"(dest));
    value = ctx->data;
    __asm__ volatile("nop" : "+r"(value));
    *dest = value;
    func_8008C9B0();
    result = ctx;
    goto done;
busy:
    result = 0;
done:
    return result;
}
