// Categories: Platform > Interrupts | Graphics > Display
#include "game/types.h"

extern void func_80091480(void);
extern void func_80096150(s32 option);

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
void func_80095E90(s32 option)
{
    if (option == 0) {
        func_80091480();
    }

    func_80096150(option);
}
