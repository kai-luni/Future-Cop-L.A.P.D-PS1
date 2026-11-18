// Categories: Game Engine > Screen Loading | Resources > File Access
#include "game/types.h"

extern s32 gScreenState __asm__("D_8009FA00");
extern s32 D_8009F658;
extern s32 gUploadInProgress __asm__("D_8009F9C4");
extern u8 *gGameSession __asm__("D_8009FA94");
extern u8 gFormatNamePair[] __asm__("D_8009F2E4");

extern void func_800247A8(void);
extern s32 func_800833A8(s32 arg0);
extern void func_8008CD30(void *dst, void *src, s32 len, s32 arg3);
extern void gLoadFrontEndModule(s32 arg) __asm__("func_80024904");
extern void func_80024A24(s32 arg);

/**
 * @brief Resolves a front-end module name to "<prefix><name>" and hands it to
 *        one of the two module loaders.
 *
 * @param arg0 Module name as it appears in the screen table, e.g. "FE.mis".
 *
 * @return Always 1. The caller (func_800287F4) uses the return value as a
 *         success flag, so a failed load is not reported here - the loader
 *         functions set their own state.
 *
 * @par Confirmed flow
 * 1. func_800247A8() prepares the load (module bookkeeping).
 * 2. D_8009FA00 = 1 (screen state "loading"), D_8009F658 = 0, and the state
 *    block's unk10 is set to 4.
 * 3. prefix = func_800833A8(arg0) returns "CW\\" when the name matches one of
 *    the five specialist modules in D_8009C1CC, "PA\\" otherwise.
 * 4. sprintf(buf, "%s%s", prefix, arg0) - D_8009F2E4 is "%s%s".
 * 5. Dispatch on D_8009F9C4: non-zero runs the "PA" loader gLoadFrontEndModule(buf),
 *    zero runs func_80024A24(buf).
 *
 * @par Globals
 * D_8009FA00 (screen state), D_8009F658, D_8009FA94 (state block, unk10),
 * D_8009F9C4 (loader selector), D_8009C1CC (five names that route to "CW\\"),
 * D_8009F2E4 ("%s%s"), D_8009F3D4/D_8009F3DC ("CW\\"/"PA\\").
 *
 * @par Callers
 * func_800287F4 only - i.e. every module start goes through here.
 *
 * @par Unknown
 * What distinguishes the five D_8009C1CC names (the "CW" group) from the rest.
 */
s32 gStartModuleByName(s32 arg0) __asm__("func_80024E3C");

s32 gStartModuleByName(s32 arg0)
{
    u8 buf[0x28];
    s32 r;

    func_800247A8();
    gScreenState = 1;
    D_8009F658 = 0;
    *(s32 *)(gGameSession + 0x10) = 4;

    r = func_800833A8(arg0);
    func_8008CD30(buf, gFormatNamePair, r, arg0);

    if (gUploadInProgress != 0) {
        gLoadFrontEndModule((s32)buf);
    } else {
        func_80024A24((s32)buf);
    }

    return 1;
}
