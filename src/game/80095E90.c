// Categories: Platform > Interrupts | Graphics > Display
#include "game/types.h"

extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void gResetMdecHardware(s32 option) __asm__("func_80096150");
void gDecDctReset(s32 option) __asm__("func_80095E90");


/**
 * @brief Runs the display option: dispatches the callback only for option 0.
 *
 * @param option Option code; 0 additionally runs func_80091480.
 * @return Nothing.
 *
 * @par Confirmed
 * if (option == 0) func_80091480(); then func_80096150(option) in every case.
 * The second call is the option-agnostic handler, the first only the special
 * case 0.
 */
void gDecDctReset(s32 option)
{
    if (option == 0) {
        gResetIrqDispatch();
    }

    gResetMdecHardware(option);
}
