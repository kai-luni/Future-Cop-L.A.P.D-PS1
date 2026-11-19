// Categories: Platform > Audio
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gActiveStreamId __asm__("D_8009F280");
extern s32 gStreamCommandState __asm__("D_8009F648");

extern void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");
extern void gMarkAudioStreamStopPending(void) __asm__("func_80022700");
extern void func_8002A31C(s32 arg0, s32 arg1);
extern void func_8002A544(void);

void gShutdownVagmAudioStream(s32 arg0) __asm__("func_80024630");

/**
 * @brief Stops VAGM audio immediately or requests a deferred stop, then releases the tracked
 * buffer.
 *
 * @param arg0 Nonzero defers the stream stop. Zero stops playback immediately.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Requires the CVK enable byte. When the command state is 1, a nonzero argument sets the
 * pending stop and zero stops immediately, then the state is cleared. Releases the nonzero
 * buffer through func_8002A31C with argument 0 before calling func_8002A544.
 *
 * @par Globals
 * Reads gCvkSystemActive and reads and clears gStreamCommandState and gActiveStreamId.
 *
 * @par Callers
 * Direct calls from gInitializeVagmAudioStream (func_800244A4), func_8002A23C,
 * gSoundHaltAllVoices (func_80047554), gExitOptionsRestoreSound (func_8007CA34),
 * gApplySavedAudioSettings (func_80080984).
 *
 * @par Role and path
 * Used on timeout, audio shutdown and settings changes.
 *
 * @par Unknown
 * The final helper owns teardown of the remaining transport state.
 */
void gShutdownVagmAudioStream(s32 arg0)
{
    if (gCvkSystemActive != 0) {
        if (gStreamCommandState == 1) {
            if (arg0 != 0) {
                gMarkAudioStreamStopPending();
            } else {
                gStopAudioStreamAndClearChannels();
            }
            gStreamCommandState = 0;
        }
        if (gActiveStreamId != 0) {
            func_8002A31C(gActiveStreamId, 0);
            gActiveStreamId = 0;
        }
        func_8002A544();
    }
}
