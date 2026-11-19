// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    u8 unk00[4];
    s32 unk04;
    u8 unk08[4];
    s32 unk0C;
    u8 unk10[0x78];
    s32 unk88;
} Unk800A0968;

extern Unk800A0968 gMixerState __asm__("D_800A0968");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");
extern void func_8002A23C(void);

void gPollAudioStreamStop(void) __asm__("func_80022654");

/**
 * @brief Stops the active stream or invokes its deadline handler when the corresponding
 * condition is met.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Checks mixer +0x04, then dispatches gStopAudioStreamAndClearChannels if +0x0C is nonzero.
 * Otherwise it compares +0x88 against the current tick and clears the expired value to -1
 * before calling func_8002A23C.
 *
 * @par Globals
 * Reads gMixerState and gFrameTickCounter and may reset the mixer deadline.
 *
 * @par Callers
 * Direct calls from gServiceAudioStreamStop (func_8002441C).
 *
 * @par Role and path
 * The stream polling wrapper runs this deferred stop and timeout path.
 *
 * @par Unknown
 * The deadline helper has a separate gate.
 */
void gPollAudioStreamStop(void)
{
    if (gMixerState.unk04 != 0) {
        if (gMixerState.unk0C != 0) {
            gStopAudioStreamAndClearChannels();
        } else if (gMixerState.unk88 < gFrameTickCounter) {
            gMixerState.unk88 = -1;
            func_8002A23C();
        }
    }
}
