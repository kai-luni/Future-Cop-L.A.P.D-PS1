#include "game/types.h"

struct Unk6C488Stats {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Unk6C488 {
    u8 unk00[0xAC];
    struct Unk6C488Stats *stats;
};

struct UnkHandle {
    u8 unk00[0x10];
    s32 field10;
};

struct UnkA10E0 {
    s32 status;
    s32 unk4;
    s32 bank;
    struct UnkHandle *handle;
    s32 field10;
};

extern u8 D_800A1830[];
extern struct UnkA10E0 D_800A10E0[];

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern s32 func_80046F9C(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4);

void func_80046CF0(struct Unk6C488 *object, s32 state)
{
    s32 s1;
    s32 s2;
    u32 i;
    s32 firstFree;
    struct UnkA10E0 *entry;
    s32 handle;

    s1 = object->stats->unkC1;
    if (s1 < 2) {
        s2 = func_80045494(state, D_800A1830, 0x47);
        firstFree = -1;
        if (s2 != 0xFF) {
            entry = &D_800A10E0[s1 * 3];
            for (i = 0; i < 3; i++, entry++) {
                if (entry->status != 0) {
                    if (entry->bank == s2) {
                        if (entry->field10 == entry->handle->field10) {
                            entry->status = 0x1E;
                            entry->unk4 = 1;
                            goto afterLoop;
                        }
                        entry->status = 0;
                    }
                }
                if (entry->status == 0 && firstFree == -1) {
                    firstFree = i;
                }
            }
        afterLoop:
            if (i != 3) {
                return;
            }
            if (firstFree == -1) {
                return;
            }
            entry = &D_800A10E0[s1 * 3] + firstFree;
            handle = func_80046F9C(0, 0, s2, (s32)object, 0);
            if (handle != 0) {
                entry->status = 0x1E;
                entry->unk4 = 1;
                entry->bank = s2;
                entry->field10 = ((struct UnkHandle *)handle)->field10;
                entry->handle = (struct UnkHandle *)handle;
            } else {
                entry->status = 0;
            }
        }
    }
}
