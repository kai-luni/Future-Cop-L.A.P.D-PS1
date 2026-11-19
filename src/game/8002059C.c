// Categories: Platform > Memory Card | Platform > Interrupts
#include "game/types.h"

struct CardContext {
    u8 unk000[0x8C4];
    s32 lockCount;
    u8 unk8C8[4];
    void (*savedCallback)(void);
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");

extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern s32 func_80091510(void (*callback)(void));
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
void gReleaseMcCallback(void) __asm__("func_8002059C");


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
 *
 * @par Role and path
 * Releases the recursive memory-card frame-callback lock by decrementing
 * gOperationState->lockCount and, when it reaches 0, restoring savedCallback
 * via func_80091510 and clearing savedCallback inside a critical section.
 * Reads and writes D_8009AB08; called by func_8002078C as the counterpart to
 * func_80020530.
 */
void gReleaseMcCallback(void)
{
    if (--gOperationState->lockCount == 0) {
        gDisableCpuInterrupts();
        func_80091510(gOperationState->savedCallback);
        gOperationState->savedCallback = 0;
        gEnableCpuInterrupts();
    }
}
