// Categories: Resources > Table Processing
#include "game/types.h"

typedef struct { u16 id, count, offset; } Entry;
typedef struct { u16 unk, count; Entry entries[1]; } Header;
typedef struct { u8 value; s8 id; u8 pad[14]; } Mapping;
typedef struct { u16 key; u8 value; u8 pad[9]; } Item;
extern Mapping gObjectSlots[] __asm__("D_800A0A00");
extern Header *gFindCshdSoundGroup(s32, s32 *) __asm__("func_80023528");
extern s32 gAppendSoundProgramSample(s32, s32, s32, s32, void *) __asm__("func_800241D0");

s32 gInstallCvkGroupSamples(s32 id) __asm__("func_800235D4");

/**
 * @brief Adds a CVK sound group's samples to the indexed sound program lists.
 *
 * @param id Identifier used for the header and mapping lookups.
 * @return 1 if both lookups succeed, including for an empty entry; else 0.
 *
 * @par Confirmed mechanics
 * Gets a header and entry index, finds the matching ID at byte +1 of an object slot, then uses
 * the entry's count and relative offset to visit 12-byte sample records. For each record it
 * passes its halfword ID and byte value plus the matched group and bank bytes to
 * gAppendSoundProgramSample.
 *
 * @par Globals
 * Reads gObjectSlots and the CSHD header records returned by the lookup.
 *
 * @par Callers
 * Direct calls from gPollCvkBankSlots (func_8002385C).
 *
 * @par Role and path
 * The ready-slot processing path installs sample records after the bank transfer completes.
 *
 * @par Unknown
 * The function reports success for a matched group with no samples.
 */
s32 gInstallCvkGroupSamples(s32 id)
{
    register s32 result __asm__("$16") = 0;
    s32 index;
    Header *header;
    s32 i, j, count;
    s8 value;
    Item *item;

    header = gFindCshdSoundGroup(id, &index);
    value = -1;
    if (header != 0) {
        __asm__("" : "+r"(result));
        for (i = 0; i < 8; i++) {
            if (gObjectSlots[i].id == id) {
                value = gObjectSlots[i].value;
                break;
            }
        }
        if (value != -1) {
            item = (Item *)((u8 *)header + header->entries[index].offset);
            j = 0;
            if (j < header->entries[index].count) {
                do {
                    gAppendSoundProgramSample(item->key, item->value, (s8)id, value, item);
                    count = header->entries[index].count;
                    __asm__("" : "+r"(j), "+r"(item) : "r"(count));
                    j++;
                    item++;
                } while (j < count);
            }
            result = 1;
        }
    }
    return result;
}
