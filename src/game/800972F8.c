// Categories: Platform > Event Handling
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    u8 pad[0x1AA];
    volatile u16 flags;
} Unk9EEA4;

extern Unk9EEA4 *gSpuRegBasePtr __asm__("D_8009EEA4");
extern volatile u32 gSpuTransferCallback __asm__("D_8009EEDC");
extern u32 gSetupGuard __asm__("D_8009EEF4");
extern void gSpuBusyWait(void) __asm__("func_80097990");
extern void func_8008DFF0(u32 arg0, s32 arg1);
void gFinishSpuTransfer(void) __asm__("func_800972F8");


/**
 *  @brief Clears two shared flag bits and dispatches a completion action.
 *
 *  The volatile flags are polled briefly if either cleared bit remains set.
 *  A registered callback is preferred over the fallback event call.
 *  @par Globals
 *  D_8009EEF4 controls preliminary setup; D_8009EEA4 supplies volatile flags.
 *  D_8009EEDC supplies the optional callback address.
 *  @par Control flow
 *  Runs setup when D_8009EEF4 is zero, then polls at most 0xF01 times.
 *  Dispatches either the callback or the fallback after polling.
 *  @par Calls
 *  gSpuBusyWait performs preliminary setup when needed.
 *  func_8008DFF0 receives 0xF0000009 and 0x20 when no callback is registered.
 *  @par Side effects
 *  Clears flag bits 0x30 and invokes setup or a completion action.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gFinishSpuTransfer(void)
{
    u32 i;

    if (gSetupGuard == 0) {
        gSpuBusyWait();
    }

    gSpuRegBasePtr->flags &= 0xFFCF;

    if (gSpuRegBasePtr->flags & 0x30) {
        i = 0;
        do {
            i++;
        } while (i < 0xF01 && (gSpuRegBasePtr->flags & 0x30));
    }

    if (gSpuTransferCallback != 0) {
        ((void (*)(void))gSpuTransferCallback)();
    } else {
        func_8008DFF0(0xF0000009, 0x20);
    }
}
