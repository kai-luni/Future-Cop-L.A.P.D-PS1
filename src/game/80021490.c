// Categories: Platform > Audio
#include "game/types.h"

struct Unk21490 {
    s32 channel;
    s32 arg1;
    s32 arg0;
    u8 unk0C[8];
    s32 mode;
};

extern s32 gSpuTransferActive __asm__("D_8009FA34");

extern void func_80098EE8(s32 channel);
extern void func_80098E88(s32 arg0, s32 arg1);
extern void func_80098E28(s32 arg0, s32 arg1);

void gStartSpuTransfer(struct Unk21490 *request) __asm__("func_80021490");

/**
 * @brief Marks a queued SPU request active and dispatches its transfer
 * operation.
 *
 * @param request Request with SPU address at +0 and buffer and count words at
 *     +4 and +8.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Sets gSpuTransferActive and calls the transfer-address routine with request
 * word +0. Mode 1 at +0x14 calls func_80098E88, and other modes call
 * func_80098E28. Both receive request words +8 and +4 in that order.
 *
 * @par Globals
 * Sets gSpuTransferActive.
 *
 * @par Callers
 * Direct calls in the retail listing are from gEnqueueSpuTransfer,
 * gCompleteSpuTransfer.
 *
 * @par Role and path
 * The SPU request ring uses this for the first request and each request
 * advanced by its completion callback.
 *
 * @par Unknown
 * The dispatch body leaves the mode-specific handling to the transfer callees.
 */
void gStartSpuTransfer(struct Unk21490 *request)
{
    gSpuTransferActive = 1;
    func_80098EE8(request->channel);
    if (request->mode == 1) {
        func_80098E88(request->arg0, request->arg1);
    } else {
        func_80098E28(request->arg0, request->arg1);
    }
}
