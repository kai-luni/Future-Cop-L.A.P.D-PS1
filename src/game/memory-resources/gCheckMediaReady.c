// Categories: Resources > Asset Loader | Platform > CD-ROM
#include "game/types.h"

extern u8 gMediaBypassFlag __asm__("D_8009F234");

extern s32 gInitCdFileSystem(s32 arg0, s32 arg1, s32 arg2) __asm__("func_8001D248");
s32 gCheckMediaReady(void) __asm__("func_8001CE10");


/**
 * @brief Readiness gate before an asset operation: returns -2 when the
 *        disc subsystem is not ready.
 *
 * @return 0 when ready or bypassed (D_8009F234 == 1), else -2.
 *
 * @par Confirmed
 * With D_8009F234 != 1 the probe func_8001D248(0, 0, 0) runs; a negative
 * result yields -2.
 *
 * @par Likely role
 * The pre-flight check of the asset lookup/load entries (func_8001D424,
 * func_8001D5E4): the bypass flag likely marks the installed/hdd case
 * where no disc wait is needed.
 *
 * @par Unknown
 * What func_8001D248 probes (likely the CD drive status).
 *
 * @par Callers
 * func_8001D424, func_8001D5E4.
 *
 * @par Role and path
 * Checks whether gMediaBypassFlag (D_8009F234) is 1, and if not calls
 * func_8001D248(0, 0, 0), returning -2 on failure or 0 when ready. Called by
 * func_8001D424 and func_8001D5E4 before opening file slots.
 */
s32 gCheckMediaReady(void)
{
    if (gMediaBypassFlag != 1 && gInitCdFileSystem(0, 0, 0) < 0) {
        return -2;
    }
    return 0;
}
