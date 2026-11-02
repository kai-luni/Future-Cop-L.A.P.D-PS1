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

extern Unk23528Header **D_8009F640[2];

Unk23528Header *func_80023528(s32 id, s32 *outIndex)
{
    s32 j = 0;
    Unk23528Header *header = 0;
    s32 i = 0;
    register Unk23528Header ***pp __asm__("$10");
    Unk23528Header **container;

    pp = D_8009F640;
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
