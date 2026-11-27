// Categories: Resources > Lookup
#include "game/types.h"

typedef struct {
    u16 id;
    u16 unk2;
    u16 unk4;
} Unk23528Entry;

typedef struct {
    u16 unk0;
    u16 count;
    Unk23528Entry entries[1];
} Unk23528Header;

extern Unk23528Header **gCshdNodeSlots[2] __asm__("D_8009F640");

Unk23528Header *gFindCshdSoundGroup(s32 id, s32 *outIndex) __asm__("func_80023528");

/**
 * @brief Finds the first CSHD header containing the requested sound group ID.
 *
 * @param id Entry ID to find.
 * @param outIndex Optional destination for the matching entry index.
 * @return The matching header, or null if no entry matches.
 *
 * @par Confirmed mechanics
 * Walks two container pointers, dereferences the first word of each nonnull container and
 * searches the halfword IDs in six-byte records. Returns the header and optionally writes the
 * matching index.
 *
 * @par Globals
 * Reads gCshdNodeSlots.
 *
 * @par Callers
 * Direct calls from gInstallCvkGroupSamples (func_800235D4), gRegisterPendingCvkPair
 * (func_800236FC).
 *
 * @par Role and path
 * CVK bank registration checks that the matching sound group exists. The sample insertion pass
 * also uses the returned index.
 *
 * @par Unknown
 * The lookup trusts the header's entry count.
 */
Unk23528Header *gFindCshdSoundGroup(s32 id, s32 *outIndex)
{
    s32 j = 0;
    Unk23528Header *header = 0;
    s32 i = 0;
    register Unk23528Header ***pp __asm__("$10");
    Unk23528Header **container;

    pp = gCshdNodeSlots;
    for (; i < 2; i++, pp++) {
        container = *pp;
        if (container != 0) {
            header = *container;
            for (j = 0; j < header->count; j++) {
                if (id == header->entries[j].id) {
                    break;
                }
            }
            if (j < header->count) {
                break;
            }
        }
    }

    if (i < 2) {
        if (outIndex != 0) {
            *outIndex = j;
        }
    } else {
        header = 0;
    }
    return header;
}
