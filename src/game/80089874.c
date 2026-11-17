// Categories: Game Engine > State Management
#include "game/types.h"

typedef struct {
    u8 pad0[0x420];
    s32 unk420;
    u8 pad424[0x150];
    void *unk574;
    void *unk578;
} Unk8009F970;

extern Unk8009F970 *gQueueBlock __asm__("D_8009F970");
extern void func_8008E5D8(void *);
extern void func_80091240(void *);
extern void func_800895E4(s32);

/**
 *  @brief Runs three cleanup or update calls when the state flag is set.
 *
 *  The routine checks a field in the global state object and, when nonzero,
 *  passes two stored pointers to separate functions before issuing a zero argument.
 *  The precise purpose of these operations is not established by this source.
 *  @par Globals
 *  D_8009F970 supplies the flag and both pointer arguments.
 *  @par Control flow
 *  A single conditional either skips all calls or executes them in order.
 *  @par Calls
 *  func_8008E5D8 receives unk574; func_80091240 receives unk578.
 *  func_800895E4 is then called with zero.
 *  @par Side effects
 *  Any state changes are performed by the called functions.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80089874(void)
{
    if (gQueueBlock->unk420 != 0) {
        func_8008E5D8(gQueueBlock->unk574);
        func_80091240(gQueueBlock->unk578);
        func_800895E4(0);
    }
}
