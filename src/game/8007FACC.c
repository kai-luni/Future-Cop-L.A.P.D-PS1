// Categories: Resources > Save Data | Graphics > Display State
#include "game/types.h"

extern u8 gPendingFlag __asm__("D_8009F496");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");

extern void func_8002078C(void);

/**
 * @brief Commits a pending profile transfer.
 *
 * If D_8009F496 is set, the two UI flags D_8009F8BD and D_8009F8BE are raised,
 * func_8002078C() releases the card operation (BIOS card events closed, lock
 * dropped) and the pending flag is cleared. A second call does nothing, which
 * makes this the idempotent "apply" half of the func_8007FA88 / func_8007FACC
 * pair.
 *
 * @return Nothing.
 *
 * @par Callers
 * func_80076A2C, func_80077AD4, func_8008125C and the unit at 0x80078530.
 */
void func_8007FACC(void)
{
    if (gPendingFlag) {
        gUiHoldFlag = 1;
        gUiInteractionEnabled = 1;
        func_8002078C();
        gPendingFlag = 0;
    }
}
