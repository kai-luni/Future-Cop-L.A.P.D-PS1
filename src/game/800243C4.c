// Categories: Game Engine > Object Dispatch
#include "../../include/game/types.h"

struct TaggedObject {
    u8 pad[0xD];
    s8 tag;
};

extern void gSetSoundChannelMix(s32 tag, s32 *values) __asm__("func_800222CC");

/**
 *  @brief Dispatches an object's tag with a three-value array.
 *
 *  Builds a local array containing -1, second, and third in that order.
 *  The object's signed tag selects the value passed to the dispatch routine.
 *  @param object Object whose tag is passed to the routine.
 *  @param third Value placed in array element 2.
 *  @param second Value placed in array element 1.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Initializes the array and makes one unconditional call.
 *  @par Calls
 *  gSetSoundChannelMix receives the tag and the local array.
 *  @par Side effects
 *  Any external effects are those of gSetSoundChannelMix.
 *  @par Callers
 *  Confirmed: func_80047800.
 */
void func_800243C4(struct TaggedObject *object, s32 third, s32 second)
{
    s32 values[3];

    values[1] = second;
    values[2] = third;
    values[0] = -1;
    gSetSoundChannelMix(object->tag, values);
}
