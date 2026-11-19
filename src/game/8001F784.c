// Categories: Game Engine > State Management
#include "game/types.h"

struct Context {
    u8 pad000[0x8f4];
    u32 state;
    s32 field8f8;
    s32 field8fc;
    s32 field900;
    s32 field904;
    u8 pad908[0x9a4 - 0x908];
    s32 field9a4;
    s32 field9a8;
    u8 pad9ac[0x1bb0 - 0x9ac];
    s32 field1bb0;
};
extern struct Context *gOperationState __asm__("D_8009AB08");
extern void func_8008D640(s32);
s32 gMcAbortCallback(void) __asm__("func_8001F784");


/**
 *  @brief Resets context state and passes a context field to a helper.
 *
 *  Clears common fields in the global context and performs additional resets
 *  when its previous state is 2, 3, or 6.
 *  @return Always zero.
 *  @par Globals
 *  D_8009AB08 points to the context whose fields are read and written.
 *  @par Control flow
 *  States 2, 3, and 6 also clear field904 and set field9a4; the default
 *  path resets state and field8f8 without those additional writes.
 *  @par Calls
 *  func_8008D640 receives field8fc in either branch.
 *  @par Side effects
 *  Clears field1bb0, state, and field8f8; sets field9a8 and may update
 *  field904 and field9a4, then invokes the helper.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Aborts or resets the memory-card operation state in gOperationState
 * (D_8009AB08) by clearing field1bb0, setting field9a8 to 1, clearing state
 * and field8f8 (plus clearing field904 and setting field9a4 to 1 when state
 * is 2, 3, or 6), and calling func_8008D640(field8fc).
 */
s32 gMcAbortCallback(void)
{
    volatile struct Context *p = gOperationState;
    p->field1bb0 = 0;
    p->field9a8 = 1;
    switch (p->state) {
    case 2:
    case 3:
    case 6:
        p->state = 0;
        p->field8f8 = 0;
        p->field904 = 0;
        p->field9a4 = 1;
        func_8008D640(p->field8fc);
        break;
    default:
        ((volatile struct Context *)gOperationState)->field8f8 = 0;
        ((volatile struct Context *)gOperationState)->state = 0;
        func_8008D640(((volatile struct Context *)gOperationState)->field8fc);
        break;
    }
    return 0;
}
