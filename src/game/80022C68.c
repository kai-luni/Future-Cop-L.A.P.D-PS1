// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    u8 pad00[0x14];
    s32 count;
    u8 pad18[0x2C];
    struct {
        s32 field;
        u8 pad[0x20];
    } array[1];
} Unk800A0968;

extern Unk800A0968 gMixerState __asm__("D_800A0968");

s32 gClearAudioStreamSpuBuffers(void) __asm__("func_80022C68");

/**
 * @brief Clears the stored SPU buffer address of each configured stream channel.
 *
 * @param void No parameters.
 * @return Always 0.
 *
 * @par Confirmed mechanics
 * Iterates while the signed channel index is below the count, zeroing the word at mixer +0x44
 * and advancing by 0x24 each time.
 *
 * @par Globals
 * Updates channel buffer words in gMixerState.
 *
 * @par Callers
 * Direct calls from gStopAudioStreamAndClearChannels (func_800226D8).
 *
 * @par Role and path
 * The stop-and-clear wrapper runs this after playback has been stopped.
 *
 * @par Unknown
 * The stores clear recorded addresses. Ownership of the underlying allocation is handled
 * elsewhere.
 */
s32 gClearAudioStreamSpuBuffers(void)
{
    s32 i;
    s32 ret;

    i = 0;
    ret = i;

    for (; i < gMixerState.count; i++) {
        gMixerState.array[i].field = 0;
    }

    return ret;
}
