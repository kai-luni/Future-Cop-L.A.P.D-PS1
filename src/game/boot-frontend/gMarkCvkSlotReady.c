// Categories: Game Engine > State
#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gSpuBufferPtr __asm__("D_8009F278");
extern s32 gVolatileBase __asm__("D_8009F27C");
extern u8 gFlushSoundChannelsIfCvkActive[] __asm__("D_80023314");
extern Unk800A0A00 gObjectSlots[8] __asm__("D_800A0A00");

extern void gClaimSpuEntry(s32 arg0) __asm__("func_80098708");
extern s32 gClearScreenModeHook(s32 callback) __asm__("func_8001F12C");
extern void func_80021250(void);
extern void gReleaseCvkSoundBanksAndVoices(void) __asm__("func_80023BF8");
extern void gShutdownSoundSystem(void) __asm__("func_80021C80");
extern void gDisableSoundProcessing(void) __asm__("func_80044FEC");

void gMarkCvkSlotReady(u32 arg0) __asm__("func_80023500");

/**
 * @brief Marks a CVK slot ready for processing after its bank load.
 *
 * @param arg0 Unsigned index of the entry to update.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Checks the unsigned index against eight, multiplies it by 16 and stores byte 2 at slot +0x02.
 *
 * @par Globals
 * Writes the state byte in gObjectSlots.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Passed by gRegisterPendingCvkPair to the sound-bank loader as its completion callback, via
 * retail alias gMarkCvkSlotReady.
 *
 * @par Unknown
 * How the loader maps completion arguments to this slot index is outside this callback.
 */
void gMarkCvkSlotReady(u32 arg0)
{
    if (arg0 < 8) {
        gObjectSlots[arg0].unk02 = 2;
    }
}
