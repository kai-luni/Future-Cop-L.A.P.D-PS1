// Categories: Game Engine > Table Processing
typedef unsigned char u8;
typedef int s32;

struct Entry {
    u8 data[16];
    u8 count;
    u8 rest[11];
};

extern struct Entry gSoundEntryTable[] __asm__("D_800A7938");
extern void func_800242B4(s32, s32);

void gClearSoundProgramSamples(void) __asm__("func_80023A24");

/**
 * @brief Drains the sample lists of all 110 sound programs.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Walks 0x1C-byte records. The inner loop captures the count at +0x10 and repeatedly loads the
 * first byte of that same record before calling func_800242B4.
 *
 * @par Globals
 * Reads gSoundEntryTable. The removal helper changes its selected record.
 *
 * @par Callers
 * Direct calls from gReleaseCvkSoundBanksAndVoices (func_80023BF8).
 *
 * @par Role and path
 * Used during CVK sound-bank teardown after bank-specific removals.
 *
 * @par Unknown
 * The helper decides how each removal compacts the record.
 */
void gClearSoundProgramSamples(void)
{
    register s32 i __asm__("$18");
    register s32 j __asm__("$16");
    register s32 count __asm__("$20");
    struct Entry *entry;
    register u8 *data __asm__("$19");
    i = 0;
    entry = gSoundEntryTable;
    do {
        count = entry->count;
        j = 0;
        if (count != 0) {
            data = entry->data;
            __asm__("" : "+r" (data));
            do {
                j++;
                func_800242B4(i, *data);
            } while (j < count);
        }
        i++;
        entry++;
    } while (i < 110);
}
