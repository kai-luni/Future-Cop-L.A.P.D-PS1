// Categories: Game Engine > Screen Dispatch | Resources | Graphics
#include "game/types.h"

extern s32 gUploadInProgress __asm__("D_8009F9C4");
extern s32 gPlayerCount __asm__("D_8009FA0C");
extern u8 gFrameBarrier __asm__("D_8009FA3C");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern u8 D_8009FAA8;

extern u8 gStartModuleByName(s32 arg0) __asm__("func_80024E3C");
extern void gPrepareAndSubmitFrame(void) __asm__("func_800259D8");
extern void func_80029E64(void);
extern void gSetGeomOffsetAndH(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80010030");
extern s32 func_80048E94(void);
extern s32 func_80048E28(s32 arg0);
extern void gFinishLevelLoad(void) __asm__("func_8003FAD4");
extern void func_80027EE4(void);
extern void gRunGameFrameLoop(s32 arg0) __asm__("func_80028618");
extern void gFinishScreenModule(s32 arg0) __asm__("func_80025B7C");

/**
 * @brief Starts (or re-enters) one front-end screen module by name and runs
 *        its per-frame work.
 *
 * @param arg0 Module/file name, e.g. "FE.mis" on the first boot pass or the
 *             name collected in D_8009B300 afterwards.
 * @param arg1 1 = UI/render mode, 0 = normal frame. Stored in D_8009F9C4,
 *             which selects which of the two module loaders is used and which
 *             of the two update paths runs.
 *
 * @return 0 on success, 0 as well when the module name could not be resolved
 *         (func_80024E3C returned 0); the caller therefore cannot distinguish
 *         the two cases and simply keeps looping.
 *
 * @par Confirmed flow
 * 1. D_8009FA0C, D_8009F9C4 (from arg1), D_8009FAA4, D_8009FAA8 are reset.
 * 2. gStartModuleByName(arg0) resolves the name to "<prefix><name>" and dispatches
 *    to func_80024904 (when D_8009F9C4 != 0) or func_80024A24; a zero result
 *    aborts the whole function.
 * 3. D_8009FA3C = 0, gPrepareAndSubmitFrame(), func_80029E64() prepare the frame.
 * 4. func_80010030(D_8009FA0C == 1 ? 0xA0 : 0x50, 0x78, 0x200) - a GTE/projection
 *    set-up with an x offset that depends on D_8009FA0C (set by the module).
 * 5. func_80048E28(func_80048E94()) and func_8003FAD4() run the per-frame
 *    engine step.
 * 6. Either func_80027EE4() (when D_8009F9C4 != 0, i.e. arg1 != 0) or
 *    func_80028618(arg0) produce the screen contents.
 * 7. gFinishScreenModule(arg0) finishes the frame.
 *
 * @par Globals
 * D_8009FA0C (set by the module, read for the projection offset),
 * D_8009F9C4 (UI mode), D_8009FAA4/D_8009FAA8 (per-frame work counters),
 * D_8009FA3C (cleared). The screen rectangle globals D_8009F850..D_8009F869
 * are read by the callees, not here.
 *
 * @par Likely role
 * The single entry point the main loop uses for "run the current screen".
 * Passing arg1 = 1 on the very first call means "initialise and draw once".
 *
 * @par Callers
 * func_80076A2C only - once with "FE.mis" and mode 1, then once per frame with
 * the module name and mode 0.
 */
s32 gRunScreenModule(s32 arg0, u8 arg1) __asm__("func_800287F4");

s32 gRunScreenModule(s32 arg0, u8 arg1)
{
    gPlayerCount = 0;
    gUploadInProgress = arg1;
    gFrameTime = 0;
    D_8009FAA8 = 0;

    if (gStartModuleByName(arg0) == 0) {
        return 0;
    }

    gFrameBarrier = 0;
    gPrepareAndSubmitFrame();
    func_80029E64();

    gSetGeomOffsetAndH((gPlayerCount == 1) ? 0xA0 : 0x50, 0x78, 0x200);
    func_80048E28(func_80048E94());
    gFinishLevelLoad();

    if (gUploadInProgress != 0) {
        func_80027EE4();
    } else {
        gRunGameFrameLoop(arg0);
    }

    gFinishScreenModule(arg0);
    return 0;
}
