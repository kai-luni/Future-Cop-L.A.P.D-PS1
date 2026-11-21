// Categories: Resources > Handles
#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};
s32 gValidateSoundVoiceHandle(struct Unk4749CHandle *handle) __asm__("func_800474C4");


/**
 *  @brief Checks whether a resource handle has a matching serial.
 *
 *  Reads the resource pointer and compares its serial with the handle's serial.
 *  A null resource pointer is treated as an invalid handle.
 *  @param handle Handle containing the resource pointer and expected serial.
 *  @return 1 if the pointer is non-null and the serials match; otherwise 0.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Branches on the resource pointer before comparing serials.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None; the handle and resource are only read.
 *  @par Callers
 *  Confirmed: func_80023D78, func_80024188.
 */
s32 gValidateSoundVoiceHandle(struct Unk4749CHandle *handle)
{
    register struct Unk4749CResource *resource __asm__("$3");
    register s32 result __asm__("$2");

    resource = handle->resource;
    result = 0;
    if (resource != 0) {
        result = resource->serial == handle->serial;
    }
    return result;
}
