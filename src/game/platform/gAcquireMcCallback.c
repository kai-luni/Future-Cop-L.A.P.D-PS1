// Categories: Platform > Memory Card | Platform > Interrupts
#include "game/types.h"

struct CardContext {
    u8 unk000[0x8C4];
    s32 lockCount;
    u8 unk8C8[4];
    s32 savedCallback;
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");
extern u8 gMemoryCardWatchdogVSyncCallback[] __asm__("D_80020024");

extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern s32 gSetVSyncCallback(void *argument) __asm__("func_80091510");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");
void gAcquireMcCallback(void) __asm__("func_80020530");


/**
 * @brief Acquires the memory-card callback lock, installing the card
 *        interrupt handler on the first acquisition.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * When D_8009AB08->lockCount == 0: func_8008C9C0 masks interrupts,
 * savedCallback = func_80091510(D_80020024) installs the handler (saving
 * the previous one), func_8008C9E0 unmask. lockCount then increments
 * unconditionally.
 *
 * @par Likely role
 * The recursive lock around card operations (func_800206D0): nested
 * callers only bump the count, the handler stays installed until the
 * matching release drops it to 0.
 *
 * @par Callers
 * func_800206D0.
 *
 * @par Role and path
 * Acquires the recursive memory-card frame-callback lock by installing
 * D_80020024 (func_80020024) via func_80091510 inside a critical section when
 * gOperationState->lockCount is 0 and saving the previous callback in
 * savedCallback, then increments lockCount. Reads and writes D_8009AB08;
 * called by func_800206D0.
 */
void gAcquireMcCallback(void)
{
    if (gOperationState->lockCount == 0) {
        gDisableCpuInterrupts();
        gOperationState->savedCallback = gSetVSyncCallback(gMemoryCardWatchdogVSyncCallback);
        gEnableCpuInterrupts();
    }
    gOperationState->lockCount++;
}
