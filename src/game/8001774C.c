// Categories: Graphics > Scene Graph | Game Engine > Init
#include "game/types.h"

extern void *gCamera[] __asm__("D_8009F9E4");
extern void *gCameraList __asm__("D_8009F588");
extern s32 gNextCameraId __asm__("D_8009F58C");

void gResetCameraState(void) __asm__("func_8001774C");

/**
 * @brief Clears the active camera and resets transform-record bookkeeping.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores zero to gCamera via its absolute address and to two consecutive
 * GP-relative words.
 *
 * @par Globals
 * Clears gCamera, gCameraList, and gNextCameraId.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80024A24.
 *
 * @par Role and path
 * Mission startup runs this before the camera registration and update
 * functions.
 *
 * @par Unknown
 * The camera selected after reset depends on later calls.
 */
void gResetCameraState(void)
{
    gCamera[0] = 0;
    gCameraList = 0;
    gNextCameraId = 0;
}
