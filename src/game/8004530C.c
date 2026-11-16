// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice {
    u8 unk00[5];
    s8 note;
    s8 program;
    s8 velocity;
    u8 unk08[0x14];
    struct SoundVoice *next;
    u8 unk20[0x0C];
};

extern void func_80047800(struct SoundVoice *voice, s32 program, s32 note, s32 velocity);

/**
 *  @brief Dispatches each linked sound voice with a valid note.
 *
 *  Traverses the voice list and passes each eligible voice's stored program,
 *  note, and velocity to the voice operation.
 *  @param voice Head of the voice list, or null for an empty list.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Follows next pointers and skips voices whose note is -1.
 *  @par Calls
 *  func_80047800 processes each eligible voice and its stored values.
 *  @par Side effects
 *  The called function may modify voice or other state.
 *  @par Callers
 *  Confirmed: func_800452D4.
 */
void func_8004530C(struct SoundVoice *voice)
{
    while (voice != 0) {
        if (voice->note != -1) {
            func_80047800(voice, voice->program, voice->note, voice->velocity);
        }
        voice = voice->next;
    }
}
