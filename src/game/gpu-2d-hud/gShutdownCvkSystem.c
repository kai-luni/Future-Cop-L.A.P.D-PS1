// Categories: Graphics > Sprite/Asset Subsystem | Resources > Teardown
#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gSpuBufferPtr __asm__("D_8009F278");
extern s32 gVolatileBase __asm__("D_8009F27C");
extern u8 gFlushSoundChannelsIfCvkActive[] __asm__("D_80023314");
extern Unk800A0A00 gObjectSlots[8] __asm__("D_800A0A00");

extern void gClaimSpuEntry(s32 arg0) __asm__("func_80098708");
extern s32 gClearScreenModeHook(s32 callback) __asm__("func_8001F12C");
extern void func_80021250(void);
extern void func_80023BF8(void);
extern void func_80021C80(void);
extern void gDisableSoundProcessing(void) __asm__("func_80044FEC");
void gShutdownCvkSystem(void) __asm__("func_80023478");


/**
 * @brief Tears the sprite/asset subsystem down when it is running.
 *
 * The whole body is guarded by D_8009F26C ("running", set by func_8002333C):
 * the two work buffers D_8009F27C and D_8009F278 are released with
 * func_80098708() and cleared, then the callback and the resource groups are
 * removed (func_8001F12C(D_80023314), func_80021250(), func_80023BF8(),
 * func_80021C80(), func_80044FEC()) and D_8009F26C = 0.
 *
 * @return Nothing. Calling it when the subsystem is not running does nothing,
 *         which is why func_8002333C can call it unconditionally at start-up.
 *
 * @par Globals
 * D_8009F26C (running flag), D_8009F278/D_8009F27C (buffers),
 * D_800A0A00 (eight object slots, not cleared here), D_80023314 (callback
 * pointer).
 *
 * @par Callers
 * func_8002333C and func_80076FE0, plus the unit at 0x80023860.
 *
 * @par Environment path
 * Not on the draw path: it is subsystem teardown. When gCvkSystemActive is
 * set it frees the two buffers gVolatileBase (D_8009F27C) and gSpuBufferPtr
 * (D_8009F278) with func_80098708 and nulls them, removes the callback at
 * 0x80023314 with func_8001F12C, calls func_80021250, func_80023BF8,
 * func_80021C80 and func_80044FEC, and clears the flag. Called by func_8002333C
 * (before set-up), func_8002385C and func_80076FE0.
 */
void gShutdownCvkSystem(void)
{
    if (gCvkSystemActive != 0) {
        if (gVolatileBase != 0) {
            gClaimSpuEntry(gVolatileBase);
            gVolatileBase = 0;
        }
        if (gSpuBufferPtr != 0) {
            gClaimSpuEntry(gSpuBufferPtr);
            gSpuBufferPtr = 0;
        }
        gClearScreenModeHook((s32)gFlushSoundChannelsIfCvkActive);
        func_80021250();
        func_80023BF8();
        func_80021C80();
        gDisableSoundProcessing();
        gCvkSystemActive = 0;
    }
}
