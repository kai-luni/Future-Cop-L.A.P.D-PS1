// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern void gResetCdCallbacks(void) __asm__("func_8008F89C");
extern s32 gInitCdSubsystem(void) __asm__("func_8008F8E8");
extern s32 gInitCdAudioMix(void) __asm__("func_8008F7AC");
s32 gInitCdMode(s32 mode) __asm__("func_8008E04C");


/**
 * @brief Initializes the CD-ROM subsystem and optionally configures CD/SPU audio mixing.
 *
 * Confirmed: when mode == 2, runs callback-only reset func_8008F89C and returns 1; otherwise calls func_8008F8E8 (CD_init, returning 0 on non-zero), and when mode == 1 also calls func_8008F7AC (CD/SPU audio mix, returning 0 on non-zero). Likely: PsyQ CdInit wrapper (sibling to func_8008DF30).
 *
 * @param mode Initialization mode (2 = reset callbacks/IRQ only; 1 = init drive and CD audio; 0/other = init drive only).
 * @return 1 on success, or 0 if func_8008F8E8 or func_8008F7AC fails.
 *
 * @par Globals
 * None directly (callees access CD-ROM/SPU ports and CD driver state D_8009C8A8..D_8009C8C5, D_8009CB80..D_8009CB88).
 *
 * @par Control flow
 * Early return on mode == 2 via func_8008F89C, else early 0 exits on non-zero returns from func_8008F8E8 and (if mode == 1) func_8008F7AC.
 *
 * @par Calls
 * func_8008F89C - clears CD callbacks and re-installs IRQ 2; func_8008F8E8 - resets CD hardware/drive; func_8008F7AC - sets CD/SPU XA mixing.
 *
 * @par Side effects
 * Configures CD-ROM hardware, IRQ 2 handler, and CD/SPU audio registers through callees.
 *
 * @par Callers
 * Confirmed: none in edges.json or asm/us/main.s / Likely: unreferenced PsyQ CdInit helper / Unknown: indirect or external callers.
 *
 * @par Role and path
 * Initializes the CD-ROM subsystem according to mode: mode 2 calls
 * func_8008F89C to reset callbacks and IRQ 2, while other modes call
 * func_8008F8E8 and, when mode == 1, func_8008F7AC. Returns 1 on success or 0
 * if either hardware helper returns non-zero. Accesses no globals directly
 * and has no direct callers in src/.
 */
s32 gInitCdMode(s32 mode)
{
    if (mode == 2) {
        gResetCdCallbacks();
        return 1;
    }
    if (gInitCdSubsystem() != 0) {
        return 0;
    }
    if (mode == 1) {
        if (gInitCdAudioMix() != 0) {
            return 0;
        }
    }
    return 1;
}
