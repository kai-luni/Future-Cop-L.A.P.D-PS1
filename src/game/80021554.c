// Categories: Game Engine > Sound Initialization
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    s32 *banks;
    s32 channelCount;
    u8 *channels;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");
extern s32 gSoundBankSlots __asm__("D_800A07AC");
extern s32 gSoundChannelCount __asm__("D_800A07B0");
extern s32 gChannelTable __asm__("D_800A07B4");

extern s32 gFindConfigValue(s32 *table, s32 key, s32 *value, s32 *index) __asm__("func_80021740");
extern void gClearSoundStorage(void) __asm__("func_800216D8");

s32 gInitSoundStorage(s32 *arguments) __asm__("func_80021554");

/**
 * @brief Initializes the sound bank and channel tables from a key-value
 * configuration.
 *
 * @param arguments Zero-terminated key-value table for sound configuration.
 * @return Always zero.
 *
 * @par Confirmed mechanics
 * Looks up counts under keys 4 and 5 with local defaults 1 and 24. Reads
 * storage pointers from keys 8 and 9, clears count times four or count times
 * 28 bytes, and writes each channel index to record +0x0C. The cleanup branch
 * tests a result register that stays zero.
 *
 * @par Globals
 * Writes fields of gSoundSystem, gSoundBankSlots, gSoundChannelCount, and
 * gChannelTable, and initializes the pointed-to tables.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021C20.
 *
 * @par Role and path
 * Sound startup connects caller-provided memory to the bank and channel
 * managers.
 *
 * @par Unknown
 * The routine assumes that the selected storage is writable and large enough
 * for the configured counts.
 */
s32 gInitSoundStorage(s32 *arguments)
{
    s32 value;
    s32 bankCount;
    s32 channelCount;
    s32 i;
    s32 count;
    s32 result;
    u8 *p;
    register s32 *valuePtr __asm__("$6");

    result = 0;
    bankCount = 1;

    valuePtr = &value;
    if (gFindConfigValue(arguments, 4, valuePtr, 0) != 0) {
        bankCount = value;
        gSoundSystem.bankCount = bankCount;
    }

    channelCount = 0x18;
    valuePtr = &value;
    if (gFindConfigValue(arguments, 5, valuePtr, 0) != 0) {
        channelCount = value;
        gSoundChannelCount = channelCount;
    }

    value = 0;
    valuePtr = &value;
    if (gFindConfigValue(arguments, 8, valuePtr, 0) != 0) {
        gSoundBankSlots = value;
    }

    p = (u8 *)gSoundBankSlots;
    count = bankCount * 4;
    while (count--) {
        *p++ = 0;
    }

    value = 0;
    valuePtr = &value;
    if (gFindConfigValue(arguments, 9, valuePtr, 0) != 0) {
        gChannelTable = value;
    }

    p = (u8 *)gChannelTable;
    count = channelCount * 28;
    while (count--) {
        *p++ = 0;
    }

    for (i = 0; i < channelCount; i++) {
        *(s32 *)(gSoundSystem.channels + i * 0x1C + 0xC) = i;
    }

    if (result != 0) {
        gClearSoundStorage();
    }
    return result;
}
