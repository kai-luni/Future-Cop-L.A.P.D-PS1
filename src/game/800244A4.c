// Categories: Game Engine > Subsystem Initialization
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gActiveStreamId __asm__("D_8009F280");
extern s32 gStreamCommandState __asm__("D_8009F648");
extern s32 D_8009F64C;

extern u8 func_8002443C[];

extern s32 gConfigureAudioStreaming(s32 arg0, ...) __asm__("func_80022484");
extern u32 gIsAudioStreamConfigured(void) __asm__("func_800226C8");
extern void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");
extern s32 gPrimeAudioStream(void) __asm__("func_80022710");
extern void func_800227BC(s32 arg0, s32 arg1, s32 arg2);
extern void func_800228F4(void);
extern void func_80024630(s32 arg0);
extern s32 func_800246A8(s32 arg0);
extern void func_8002A31C(s32 arg0, s32 arg1);
extern s32 func_800451E0(void);
extern s32 func_80045288(void);

/**
 *  @brief Conditionally initializes and configures a subsystem.
 *
 *  When enabled, the routine handles an existing operation, sets global state,
 *  submits a fixed configuration, and applies two channel settings on success.
 *  @return Zero when disabled or on the explicit failure paths; otherwise the
 *  value returned by func_800246A8.
 *  @par Globals
 *  D_8009F26C enables the routine; D_8009F280 holds an operation to clear.
 *  D_8009F648 and D_8009F64C receive fixed values; func_8002443C supplies an address.
 *  @par Control flow
 *  Initialization failure invokes cleanup; a later readiness failure cleans up
 *  and returns zero. A mode query selects asymmetric or equal channel values.
 *  @par Calls
 *  gIsAudioStreamConfigured, gStopAudioStreamAndClearChannels, and func_8002A31C handle an existing operation.
 *  gConfigureAudioStreaming, func_80024630, func_800246A8, and gPrimeAudioStream initialize or clean up.
 *  func_800451E0, func_80045288, func_800227BC, and func_800228F4 configure and finish.
 *  @par Side effects
 *  Changes global state and may stop, initialize, or configure the subsystem.
 *  @par Callers
 *  Confirmed: func_8002902C.
 */
s32 func_800244A4(void)
{
    s32 result;

    result = 0;
    if (gCvkSystemActive != 0) {
        if ((gIsAudioStreamConfigured() & 0xFF) != 0) {
            gStopAudioStreamAndClearChannels();
            if (gActiveStreamId != 0) {
                func_8002A31C(gActiveStreamId, result);
                gActiveStreamId = 0;
            }
        }
        gStreamCommandState = 1;
        D_8009F64C = 0x1C2;
        gActiveStreamId = 0;
        if (gConfigureAudioStreaming(1, 0x37B4, 2, 0x2FC0, 3, 3, 10, 2, 6,
                          (s32)(func_8002443C + 0x34), 0) < 0) {
            func_80024630(0);
        } else {
            result = func_800246A8(4);
            if (result != 0) {
                if (gPrimeAudioStream() == 0) {
                    func_80024630(0);
                    return 0;
                }
                if (func_800451E0() != 0) {
                    func_800227BC(0, func_80045288(), 0);
                    func_800227BC(1, func_80045288(), 0x7F);
                } else {
                    func_800227BC(0, func_80045288(), 0x40);
                    func_800227BC(1, func_80045288(), 0x40);
                }
                func_800228F4();
            }
        }
    }
    return result;
}
