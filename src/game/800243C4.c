// Categories: Game Engine > Audio | Platform > Sound > Voices
#include "../../include/game/types.h"

struct TaggedObject {
    u8 pad[0xD];
    s8 tag;
};

extern void gSetSoundChannelMix(s32 tag, s32 *values) __asm__("func_800222CC");

void gUpdateVoiceChannelMix(struct TaggedObject *object, s32 third, s32 second) __asm__("func_800243C4");

/**
 * @brief Applies the supplied level and pan to a voice's channel without changing pitch.
 *
 * @param object Voice descriptor containing the signed channel byte at +0x0D.
 * @param third Pan value stored in element 2 of the mix array.
 * @param second Level value stored in element 1 of the mix array.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the signed channel byte at voice +0x0D and passes a stack array containing -1, the
 * third register argument and the second register argument to gSetSoundChannelMix.
 *
 * @par Globals
 * None directly. The callee reads gChannelTable and submits the SPU mix.
 *
 * @par Callers
 * Direct calls from func_80047800.
 *
 * @par Role and path
 * The voice mix path forwards an updated volume and pan with the pitch field disabled.
 *
 * @par Unknown
 * Channel bounds are handled by the caller.
 */
void gUpdateVoiceChannelMix(struct TaggedObject *object, s32 third, s32 second)
{
    s32 values[3];

    values[1] = second;
    values[2] = third;
    values[0] = -1;
    gSetSoundChannelMix(object->tag, values);
}
