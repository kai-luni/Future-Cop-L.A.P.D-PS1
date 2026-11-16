// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice {
    u8 unk00[0x1C];
    struct SoundVoice *next;
};

extern struct SoundVoice *D_8009F73C;
extern s32 D_8009F740;

/**
 *  @brief Removes and returns the head of the available voice list.
 *
 *  Uses the shared count to decide whether a voice is available.
 *  When nonzero, it decrements the count and advances the list head.
 *  @return The removed voice, or null if the count is zero.
 *  @par Globals
 *  D_8009F740 is the count; D_8009F73C is the list head.
 *  @par Control flow
 *  Branches on the count; no traversal is performed.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  A nonzero count is decremented and the head is replaced by its next pointer.
 *  @par Callers
 *  Confirmed: func_80046B04.
 */
struct SoundVoice *func_8004665C(void)
{
    struct SoundVoice *voice;

    if (D_8009F740 != 0) {
        D_8009F740--;
        voice = D_8009F73C;
        D_8009F73C = voice->next;
    } else {
        voice = 0;
    }
    return voice;
}
