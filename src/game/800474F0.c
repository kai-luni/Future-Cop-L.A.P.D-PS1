// Categories: Platform > Audio
#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

extern void func_80046A34(struct Unk4749CResource *resource);

/**
 * @brief Releases a sound resource handle if it is still valid.
 *
 * A null handle, a null resource or a serial mismatch (the resource was already
 * recycled) ends the call without freeing. Otherwise the resource is released
 * through func_80046A34, the handle is cleared and 1 is returned, which makes
 * the function safe to call twice.
 * @param handle Handle with resource pointer and serial.
 * @return 1 when the resource was released, 0 otherwise.
 * @par Globals
 * None.
 * @par Control flow
 * Three guard exits, one release path.
 * @par Calls
 * func_80046A34 - releases the resource.
 * @par Side effects
 * Frees the resource and clears the handle.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
s32 func_800474F0(struct Unk4749CHandle *handle)
{
    s32 result;

    result = 0;
    if (handle == 0) {
        goto set_result;
    }
    if (handle->resource == 0) {
        goto done;
    }
    if (handle->resource->serial != handle->serial) {
        goto done;
    }
    func_80046A34(handle->resource);
    handle->resource = 0;
    result = 1;
set_result:
    return result;
done:
    return result;
}
