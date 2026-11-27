// Categories: Game Engine > Object Processing
#include "game/types.h"

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

struct Unk6A898 {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
};

extern void gScaleDefinitionSubrecordFields(struct Unk607B8 *value) __asm__("func_800607B8");
void gScaleRegistryObjectFields(struct Unk6A898 *object) __asm__("func_8006A898");


/**
 *  @brief Processes an embedded structure and rescales two object fields.
 *
 *  After processing the embedded substructure, the function scales the
 *  two signed fields by shifting left 12 bits and dividing by 218.
 *  @param object Object containing the substructure and fields to rescale.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Calls the substructure operation once, then updates both fields in order.
 *  @par Calls
 *  func_800607B8 processes the embedded substructure.
 *  @par Side effects
 *  Replaces object fields unk0A and unk08 and may modify the substructure through the call.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gScaleRegistryObjectFields(struct Unk6A898 *object)
{
    gScaleDefinitionSubrecordFields(&object->sub);
    object->unk0A = (object->unk0A << 12) / 218;
    object->unk08 = (object->unk08 << 12) / 218;
}
