// Categories: Platform > Sound
#include "game/types.h"

extern void func_80021508(void);
extern void func_80022CA8(void);
extern s32 func_80021554(s32 *arguments);
extern void func_80021C80(void);

/**
 * @brief Variadic sound-command wrapper: prepare, execute against the
 *        sound system state, commit on success.
 *
 * @param first First command word; the remaining varargs are passed by
 *              address to the executor.
 * @return The executor's result.
 *
 * @par Confirmed
 * func_80021508(); func_80022CA8(); result = func_80021554(&first);
 * when result != 0, func_80021C80(); return result. func_80021554 works
 * on the SoundSystem block D_800A07A8 (bank and channel tables).
 *
 * @par Likely role
 * The two surrounding calls lock/prepare and commit the sound state; the
 * variadic form carries command-specific parameters.
 *
 * @par Callers
 * func_8002333C.
 */
s32 func_80021C20(s32 first, ...)
{
    s32 result;

    func_80021508();
    func_80022CA8();
    result = func_80021554(&first);
    if (result != 0) {
        func_80021C80();
    }
    return result;
}
