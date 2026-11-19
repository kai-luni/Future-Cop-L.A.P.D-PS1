// Categories: Platform > Sound
#include "game/types.h"

extern void func_80021508(void);
extern void func_80022CA8(void);
extern s32 func_80021554(s32 *arguments);
extern void gShutdownSoundSystem(void) __asm__("func_80021C80");

s32 gConfigureSoundSystem(s32 first, ...) __asm__("func_80021C20");

/**
 * @brief Resets sound and stream state before forwarding a variadic sound configuration.
 *
 * @param first First word of the sound configuration list.
 * @param ... Remaining configuration words in the caller's argument area.
 * @return The parser's status word.
 *
 * @par Confirmed mechanics
 * Spills a0 through a3, calls func_80021508 and func_80022CA8, then passes the address of first
 * to func_80021554. It preserves the result across a call to gShutdownSoundSystem on nonzero
 * status.
 *
 * @par Globals
 * None directly. The reset and configuration callees update gSoundSystem and gMixerState.
 *
 * @par Callers
 * Direct calls from gInitCvkStreamSubsystem (func_8002333C).
 *
 * @par Role and path
 * The sound-bank setup path feeds its key/value configuration through this wrapper. The retail
 * parser clears the result before returning, so the conditional teardown is not reached through
 * that parser.
 *
 * @par Unknown
 * The wrapper alone does not establish the full configuration record format.
 */
s32 gConfigureSoundSystem(s32 first, ...)
{
    s32 result;

    func_80021508();
    func_80022CA8();
    result = func_80021554(&first);
    if (result != 0) {
        gShutdownSoundSystem();
    }
    return result;
}
