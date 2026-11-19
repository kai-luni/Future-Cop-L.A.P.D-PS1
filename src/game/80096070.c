// Categories: Graphics > Image Handling | Utilities > Memory
#include "game/types.h"

extern void func_800962D0(s32 destination, u32 size);
void gStartMdecOutDma(s32 destination, u32 size) __asm__("func_80096070");


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
 *
 * @par Environment path
 * Video decode support: starts the next decode-out DMA into the destination
 * buffer with the given size, so a strip can be decoded before func_80085074
 * uploads it to VRAM. Callers are func_80085074, func_80085228, func_80085630
 * and func_800861E4.
 */
void gStartMdecOutDma(s32 destination, u32 size)
{
    func_800962D0(destination, size);
}
