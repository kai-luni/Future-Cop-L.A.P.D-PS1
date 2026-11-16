// Categories: Platform > Audio
#include "game/types.h"

struct Unk21490 {
    s32 channel;
    s32 arg1;
    s32 arg0;
    u8 unk0C[8];
    s32 mode;
};

extern s32 D_8009FA34;

extern void func_80098EE8(s32 channel);
extern void func_80098E88(s32 arg0, s32 arg1);
extern void func_80098E28(s32 arg0, s32 arg1);

/**
 *  @brief Dispatches a channel request using the selected mode.
 *
 *  Marks the request state, selects the channel, and forwards the two
 *  request arguments to one of two mode-dependent operations.
 *  @param request Request containing the channel, arguments, and mode.
 *  @par Globals
 *  D_8009FA34 is set to 1 before dispatch.
 *  @par Control flow
 *  Mode 1 selects func_80098E88; all other modes select func_80098E28.
 *  @par Calls
 *  func_80098EE8 selects the channel; the selected operation receives arg0 and arg1.
 *  @par Side effects
 *  Updates the global flag and invokes the channel operations.
 *  @par Callers
 *  Confirmed: func_800212F8, func_8002139C.
 */
void func_80021490(struct Unk21490 *request)
{
    D_8009FA34 = 1;
    func_80098EE8(request->channel);
    if (request->mode == 1) {
        func_80098E88(request->arg0, request->arg1);
    } else {
        func_80098E28(request->arg0, request->arg1);
    }
}
