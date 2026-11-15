// Categories: Graphics > Image Handling | Utilities > Memory
#include "game/types.h"

extern void func_800962D0(s32 destination, u32 size);

/**
 * @brief Copies image data with an explicit size (thin wrapper).
 *
 * @param destination Destination address.
 * @param size Byte count.
 * @return Nothing; identical signature and forwarding to func_800962D0.
 *
 * @par Confirmed
 * No own logic: the call is the whole body, so the wrapper exists to keep the
 * public name/signature of the API.
 */
void func_80096070(s32 destination, u32 size)
{
    func_800962D0(destination, size);
}
