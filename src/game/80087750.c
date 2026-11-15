// Categories: Platform > Sound > Shutdown | Platform > SPU
#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

typedef struct {
    u8 pad[0x7B10];
    void *unk7B10;
    SpuIrqCallback unk7B14;
    s32 unk7B18;
    volatile s32 unk7B1C;
    volatile s32 unk7B20;
    volatile s32 unk7B24;
    u8 pad2[0x20];
    volatile s32 unk7B48;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;

extern void func_8008C9C0(void);
extern void func_800876EC(void);
extern void func_80098A78(s32 arg);
extern SpuIrqCallback func_80098BF8(SpuIrqCallback callback);
extern void func_8008C9E0(void);
extern void func_80098FB8(s32 value);
extern void func_80091510(void *argument);
extern void func_80087688(void);

/**
 * @brief Shuts the sound subsystem down: stops the DMA, waits for the
 *        pending flag and restores the saved callbacks.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Under func_8008C9C0: func_800876EC, func_80098A78(0), then
 * func_80098BF8(D_8009F4D8->unk7B14) restores the IRQ callback;
 * func_8008C9E0 unmask. A busy loop waits for unk7B48 == 0. Under mask
 * again: func_80098FB8(unk7B18) restores the SPU DMA latch, unk7B20 = 0,
 * unk7B1C != 0 installs func_80091510(unk7B10), unk7B24 = 0; unmask and
 * func_80087688 finish.
 *
 * @par Likely role
 * The teardown counterpart of func_80087824 (both called by
 * func_80088120): the busy wait drains the in-flight transfer before the
 * latch is restored, so no IRQ fires into a freed state.
 *
 * @par Unknown
 * What unk7B48 counts (likely the pending DMA words).
 *
 * @par Callers
 * func_80088120 (verified in the retail listing; the call-graph edge is
 * missing).
 */
void func_80087750(void)
{
    func_8008C9C0();
    func_800876EC();
    func_80098A78(0);

    func_80098BF8(D_8009F4D8->unk7B14);
    func_8008C9E0();

    while (D_8009F4D8->unk7B48 != 0) {
    }

    func_8008C9C0();
    func_80098FB8(D_8009F4D8->unk7B18);
    D_8009F4D8->unk7B20 = 0;
    if (D_8009F4D8->unk7B1C != 0) {
        func_80091510(D_8009F4D8->unk7B10);
    }
    D_8009F4D8->unk7B24 = 0;
    func_8008C9E0();
    func_80087688();
}
