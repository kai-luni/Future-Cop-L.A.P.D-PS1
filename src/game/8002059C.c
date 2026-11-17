// Categories: Platform > Memory Card | Platform > Interrupts
#include "game/types.h"

struct CardContext {
    u8 unk000[0x8C4];
    s32 lockCount;
    u8 unk8C8[4];
    void (*savedCallback)(void);
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");

extern void func_8008C9C0(void);
extern s32 func_80091510(void (*callback)(void));
extern void func_8008C9E0(void);

/**
 * @brief Releases the memory-card callback lock, restoring the previous
 *        interrupt handler on the last release.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * --lockCount; when it reaches 0: func_8008C9C0 masks interrupts,
 * func_80091510(savedCallback) restores the handler, savedCallback = 0,
 * func_8008C9E0 unmask. The acquire counterpart is func_80020530.
 *
 * @par Likely role
 * The recursive unlock around card operations (func_8002078C): nested
 * acquisitions only bump the count, the original handler returns when the
 * outermost section ends.
 *
 * @par Callers
 * func_8002078C.
 */
void func_8002059C(void)
{
    if (--gOperationState->lockCount == 0) {
        func_8008C9C0();
        func_80091510(gOperationState->savedCallback);
        gOperationState->savedCallback = 0;
        func_8008C9E0();
    }
}
