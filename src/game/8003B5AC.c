#include "game/types.h"

struct Item {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
};

struct Entry {
    u8 pad00[4];
    struct Item items[1];
};

extern struct Entry *D_800A0C70[];

u8 func_8003B5AC(u8 index, u8 slot, s32 mode)
{
    struct Entry *entry = D_800A0C70[index];

    if (entry == 0) {
        return 0;
    }
    if (mode == 1) {
        goto tail;
    }
    if (mode == 0) {
        goto c0;
    }
    if (mode == 2) {
        goto c2;
    }
    if (mode == 3) {
        return 0;
    }
    goto tail;
c0:
    return entry->items[slot].unk3;
c2:
    return entry->items[slot].unk1;
tail:
    return D_800A0C70[index]->items[slot].unk2;
}
