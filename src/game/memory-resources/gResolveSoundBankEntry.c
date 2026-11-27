// Categories: Resources > Offset Tables
#include "../../include/game/types.h"

void *gResolveSoundBankEntry(void *base, u32 index) __asm__("func_80021800");

/**
 * @brief Resolves a nonzero indexed offset to an address inside a sound bank.
 *
 * @param base Sound bank containing the relative-offset table.
 * @param index Index of the desired entry.
 * @return Base plus the stored offset, or null for a zero offset.
 *
 * @par Confirmed mechanics
 * Loads a word at base +0x10 plus index times four, and adds it to the
 * original base when nonzero.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021F98.
 *
 * @par Role and path
 * The voice startup path resolves a selected bank entry before configuring a
 * channel.
 *
 * @par Unknown
 * The caller must provide a valid table index.
 */
void *gResolveSoundBankEntry(void *base, u32 index)
{
    u32 offset = *(u32 *)((u8 *)base + 0x10 + index * 4);

    if (offset == 0) {
        return 0;
    }
    return (u8 *)base + offset;
}
