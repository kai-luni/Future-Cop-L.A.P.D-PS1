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
void gBindSoundVoiceHandle(struct Unk4749CHandle *handle, struct Unk4749CResource *resource) __asm__("func_8004749C");


/**
 *  @brief Associates a handle with a resource and copies its serial.
 *
 *  When both pointers are nonnull, stores the resource pointer in the handle.
 *  It also snapshots the resource's serial into the handle.
 *  @param handle Handle to update, or null to do nothing.
 *  @param resource Resource to associate, or null to do nothing.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Both assignments are guarded by nonnull checks on the arguments.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May modify the handle's resource and serial fields.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gBindSoundVoiceHandle(struct Unk4749CHandle *handle, struct Unk4749CResource *resource)
{
    if (resource != 0 && handle != 0) {
        handle->resource = resource;
        handle->serial = resource->serial;
    }
}
