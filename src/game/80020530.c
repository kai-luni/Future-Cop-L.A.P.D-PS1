// Categories: Platform > Memory Card | Platform > Interrupts
#include "game/types.h"

struct CardContext {
    u8 unk000[0x8C4];
    s32 lockCount;
    u8 unk8C8[4];
    s32 savedCallback;
};

extern struct CardContext *gOperationState __asm__("D_8009AB08");
extern u8 D_80020024[];

extern void func_8008C9C0(void);
extern s32 func_80091510(void *argument);
extern void func_8008C9E0(void);

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
 */
void func_80020530(void)
{
    if (gOperationState->lockCount == 0) {
        func_8008C9C0();
        gOperationState->savedCallback = func_80091510(D_80020024);
        func_8008C9E0();
    }
    gOperationState->lockCount++;
}
