// Categories: Platform > Sound | Platform > DMA
#include "game/types.h"

struct Struct800A8EA8 {
    u8 pad0[0xC];
    s32 unk0C;
    u8 pad10[0x8];
};

extern s32 gInitDoneFlag __asm__("D_8009F9C8");
extern s32 D_8009FA80;
extern s32 D_8009F998;
extern s32 D_8009FA34;
extern struct Struct800A8EA8 D_800A8EA8[];

/**
 * @brief Cancels queued SPU DMA requests and waits for any active transfer to finish.
 *
 * Confirmed: when D_8009F9C8 is non-zero, collapses enqueue index D_8009F998 to dequeue index D_8009FA80, clears the active request's callback pointer at D_800A8EA8[D_8009FA80].unk0C (+0x0C), and polls D_8009FA34 up to 0x10000 checks.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F9C8 - queue init flag (read); D_8009FA80 - dequeue index (read); D_8009F998 - enqueue index (written); D_800A8EA8 - 9-entry Request queue (+0x0C cleared); D_8009FA34 - active transfer flag (polled).
 *
 * @par Control flow
 * Guarded by D_8009F9C8 != 0; executes a bounded busy-wait loop (D_8009FA34 != 0 && timeout-- != 0, initialized to 0xFFFF).
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Discards pending entries in D_800A8EA8 and nulls the active entry's completion callback at +0x0C.
 *
 * @par Callers
 * Confirmed: func_80022A64 (0x80022AAC in asm/us/main.s), func_80021250 (0x80021260 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_8002129C(void)
{
    s32 timeout = 0xFFFF;

    if (gInitDoneFlag != 0) {
        D_8009F998 = D_8009FA80;
        D_800A8EA8[D_8009FA80].unk0C = 0;
        while (D_8009FA34 != 0 && timeout-- != 0) {
        }
    }
}
