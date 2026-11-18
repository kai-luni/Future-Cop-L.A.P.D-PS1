// Categories: Game Engine > State Access
#include "game/types.h"

struct D_8009F4D8_Obj {
    u8 pad_00[0x7B74];
    volatile u32 field_7B74;
};

extern struct D_8009F4D8_Obj *gMovieAudioState __asm__("D_8009F4D8");

/**
 *  @brief Writes an unsigned value to a global object's field.
 *
 *  Stores the supplied value through a volatile field at offset 0x7B74
 *  in the object referenced by the global pointer.
 *  @param value Unsigned value to store.
 *  @par Globals
 *  gMovieAudioState supplies the object pointer.
 *  @par Control flow
 *  Performs the store unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes the object's field at offset 0x7B74; the pointer must be valid.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gMovieSetAudioMode(u32 value) __asm__("func_80086F08");

void gMovieSetAudioMode(u32 value)
{
    gMovieAudioState->field_7B74 = value;
}
