// Categories: Platform > Audio
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gActiveStreamId __asm__("D_8009F280");
extern s32 gStreamCommandState __asm__("D_8009F648");

extern void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");
extern void gMarkAudioStreamStopPending(void) __asm__("func_80022700");
extern void func_8002A31C(s32 arg0, s32 arg1);
extern void func_8002A544(void);

/**
 * @brief Shuts the audio stream path down when streaming is active.
 *
 * The whole body is gated by D_8009F26C (streaming enabled). A pending mode
 * switch (D_8009F648 == 1) is resolved first: arg0 selects the "begin/stop"
 * variant gMarkAudioStreamStopPending or gStopAudioStreamAndClearChannels, and the pending marker is cleared.
 * An active stream id D_8009F280 is then handed to func_8002A31C with volume 0
 * and forgotten, and func_8002A544 runs the final teardown of the channel.
 *
 * @param arg0 Selects the stop variant: non-zero uses the "begin" path.
 *
 * @par Globals
 * D_8009F26C - streaming enabled; D_8009F648 - pending mode switch;
 * D_8009F280 - active stream id.
 *
 * @par Control flow
 * Two gates, then two teardown calls.
 *
 * @par Calls
 * gMarkAudioStreamStopPending / gStopAudioStreamAndClearChannels - mode switch. func_8002A31C - mute/stop.
 * func_8002A544 - channel teardown.
 *
 * @par Side effects
 * Stops the stream and clears the pending markers.
 *
 * @par Callers
 * Confirmed: the audio shutdown path (func_800243C4 neighbourhood).
 */
void func_80024630(s32 arg0)
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
