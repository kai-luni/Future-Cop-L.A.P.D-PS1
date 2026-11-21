// Categories: Platform > Sound | Resources > Sound Banks | Game Engine > Game State > Reset
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s8 gObjectSlots[] __asm__("D_800A0A00");
extern void *gCshdNodeSlots[2] __asm__("D_8009F640");
extern volatile s32 gCvkSpuUploadComplete __asm__("D_8009F650");

extern void gUnloadCvkSoundBank(s32 arg0) __asm__("func_80023AB4");
extern void gClearSoundProgramSamples(void) __asm__("func_80023A24");
extern void gFreeBlock(void *block) __asm__("func_800181D4");
extern void gResetSoundBankTables(void) __asm__("func_80044EF8");

void gReleaseCvkSoundBanksAndVoices(void) __asm__("func_80023BF8");

/**
 * @brief Releases CVK sound banks, program samples, CSHD nodes and voice allocations.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Tests the enable byte, passes each of eight slot IDs to gUnloadCvkSoundBank and drains the
 * program list. Frees and clears both nonnull CSHD pointers before calling func_80044EF8.
 *
 * @par Globals
 * Reads gCvkSystemActive and gObjectSlots and clears gCshdNodeSlots. The callees modify the
 * sample and voice tables.
 *
 * @par Callers
 * Direct calls from gShutdownCvkSystem (func_80023478), gUnloadFrontEndSubsystems
 * (func_800249BC), gResetMissionSubsystems (func_80024D14), gFinishScreenModule
 * (func_80025B7C).
 *
 * @par Role and path
 * Used by CVK shutdown and by transitions that reset mission or front-end sound resources.
 *
 * @par Unknown
 * Allocation and hardware teardown details reside in the callees.
 */
void gReleaseCvkSoundBanksAndVoices(void)
{
    register u32 i __asm__("$17");
    s8 *entry;
    s32 value;
    register void **slot __asm__("$16");
    void *block;

    if (gCvkSystemActive != 0) {
        i = 0;
        entry = gObjectSlots;
        do {
            value = entry[1];
            entry += 0x10;
            i++;
            gUnloadCvkSoundBank(value);
        } while (i < 8);

        i = 0;
        gClearSoundProgramSamples();
        slot = gCshdNodeSlots;
        do {
            block = *slot;
            if (block != 0) {
                gFreeBlock(block);
                *slot = 0;
            }
            i++;
            slot++;
        } while (i < 2);

        gResetSoundBankTables();
    }
}

void gMarkCvkSpuUploadComplete(void) __asm__("D_80023C94");

/**
 * @brief Marks the synchronous CVK SPU upload complete.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores 1 to the word at gp +0x43C and returns.
 *
 * @par Globals
 * Writes gCvkSpuUploadComplete.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Installed as the DMA completion callback by gUploadCvkSpuDataAndWait. The submitting routine
 * clears and polls the word.
 *
 * @par Unknown
 * The callback takes no observed request argument.
 */
void gMarkCvkSpuUploadComplete(void)
{
    gCvkSpuUploadComplete = 1;
}
