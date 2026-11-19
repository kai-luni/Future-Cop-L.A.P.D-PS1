// Categories: Resources > Slot Table | Game Engine > Frame
#include "game/types.h"

typedef struct {
    s8 unk0;
    s8 id;
    s8 state;
    s8 pad3;
    s32 unk4;
    void *block;
    s32 time;
} Slot;

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern Slot gObjectSlots[] __asm__("D_800A0A00");

extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void gInstallCvkGroupSamples(s32 id) __asm__("func_800235D4");
extern void gShutdownCvkSystem(void) __asm__("func_80023478");

s32 gPollCvkBankSlots(void) __asm__("func_8002385C");

/**
 * @brief Processes ready CVK banks and shuts down the CVK subsystem on an expired pending bank.
 *
 * @param void No parameters.
 * @return Always 1.
 *
 * @par Confirmed mechanics
 * When enabled, scans eight 16-byte records. State 2 frees the +0x08 block, clears the state
 * byte and processes the group. State 1 with an expired deadline calls gShutdownCvkSystem and
 * exits immediately.
 *
 * @par Globals
 * Reads gCvkSystemActive and gFrameTickCounter and updates gObjectSlots.
 *
 * @par Callers
 * Direct calls from gDrawScreenFadeQuad (func_80025690), gPrepareAndSubmitFrame
 * (func_800259D8).
 *
 * @par Role and path
 * Frame preparation and fade drawing poll asynchronous CVK bank loads.
 *
 * @par Unknown
 * The retail paths return 1, including after an expired pending slot.
 */
s32 gPollCvkBankSlots(void)
{
    s32 ret;
    u32 i;
    Slot *slot;

    ret = 1;
    if (gCvkSystemActive != 0) {
        i = 0;
        do {
            slot = &gObjectSlots[i];
            if (slot->state == 2) {
                if (slot->block != 0) {
                    gFreeBlock(slot->block);
                    slot->block = 0;
                }
                slot->state = 0;
                gInstallCvkGroupSamples(slot->id);
            } else if (slot->state == 1) {
                ret = 0;
                if (slot->time < gFrameTickCounter) {
                    ret = 1;
                    gShutdownCvkSystem();
                    break;
                }
            }
            i++;
        } while (i < 8);
    }
    return ret;
}
