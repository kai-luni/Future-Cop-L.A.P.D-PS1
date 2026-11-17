// Categories: Platform > Sound | Resources > Sound Banks | Game Engine > Game State > Reset
#include "game/types.h"

extern u8 gChannelEnableFlag __asm__("D_8009F26C");
extern s8 gObjectSlots[] __asm__("D_800A0A00");
extern void *gStreamStatePair[2] __asm__("D_8009F640");
extern volatile s32 D_8009F650;

extern void func_80023AB4(s32 arg0);
extern void func_80023A24(void);
extern void func_800181D4(void *block);
extern void func_80044EF8(void);

/**
 * @brief Tears down active VAB sound banks, sound program tables, buffers, and voice slots when D_8009F26C is set.
 *
 * Iterates all 8 VAB entries in D_800A0A00 (stride 0x10) to unload active banks, clears 110 sound bank programs, frees non-null buffer pointers in D_8009F640[0..1], and resets voice allocation tables via func_80044EF8. Trailing label D_80023C94 in the same unit sets D_8009F650 = 1.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F26C (subsystem active guard), D_800A0A00 (8x 0x10-byte VAB slot array), D_8009F640 (2-element buffer pointer array), D_8009F650 (set to 1 by trailing D_80023C94).
 *
 * @par Control flow
 * Guarded by D_8009F26C != 0: loops i = 0..7 calling func_80023AB4(entry[1]), calls func_80023A24(), loops i = 0..1 freeing and clearing non-null D_8009F640[i] via func_800181D4, then calls func_80044EF8().
 *
 * @par Calls
 * func_80023AB4 (unload VAB bank entry), func_80023A24 (reset 110 sound bank slots), func_800181D4 (free D_8009F640 buffers), func_80044EF8 (reset voice tables and channels).
 *
 * @par Side effects
 * Unloads VAB sound banks, frees and zeroes D_8009F640[0..1], and resets voice/channel tables; D_80023C94 writes D_8009F650 = 1.
 *
 * @par Callers
 * Confirmed: func_80023478 (0x800234D4), func_800249BC (0x800249F4), func_80024D14 (0x80024DF8), and func_80025B7C (0x80025CB0).
 */
void func_80023BF8(void)
{
    register u32 i __asm__("$17");
    s8 *entry;
    s32 value;
    register void **slot __asm__("$16");
    void *block;

    if (gChannelEnableFlag != 0) {
        i = 0;
        entry = gObjectSlots;
        do {
            value = entry[1];
            entry += 0x10;
            i++;
            func_80023AB4(value);
        } while (i < 8);

        i = 0;
        func_80023A24();
        slot = gStreamStatePair;
        do {
            block = *slot;
            if (block != 0) {
                func_800181D4(block);
                *slot = 0;
            }
            i++;
            slot++;
        } while (i < 2);

        func_80044EF8();
    }
}

void D_80023C94(void)
{
    D_8009F650 = 1;
}
