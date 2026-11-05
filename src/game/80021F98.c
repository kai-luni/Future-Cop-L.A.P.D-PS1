#include "game/types.h"

struct Unk07A8 {
    s32 unk00;
    s32 *unk04;
    s32 unk08;
    u8 *unk0C;
};

extern struct Unk07A8 D_800A07A8;

struct Args {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

struct Entry {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
};

extern u8 *func_80021800(void *base, s32 index);
extern void func_800223B0(s32 a0, s32 a1, s32 a2, s32 *a3, s32 *a4);
extern void func_80021A70(u32 flags);

s32 func_80021F98(s32 a0, s32 a1, s32 a2, s32 a3, s32 slot, struct Args *args)
{
    s32 out1 = 0;
    s32 out2 = 0;
    s32 result = 0;
    u8 *ptr;
    s32 offset;
    struct Entry *entry;

    if (D_800A07A8.unk00 >= a0 && a0 >= 0) {
        ptr = (u8 *)D_800A07A8.unk04[a0];
        if (ptr != 0) {
            ptr = func_80021800(ptr, a1);
            if (ptr != 0) {
                offset = (a2 * 5) * 4;
                offset += 4;
                result = (s32)ptr + offset;
                func_800223B0(result, args->unk04, args->unk08, &out1, &out2);
                entry = (struct Entry *)(D_800A07A8.unk0C + slot * 28);
                entry->unk00 = result;
                if (args->unk00 == -1) {
                    entry->unk10 = *(u16 *)(result + 0xE);
                } else {
                    entry->unk10 = args->unk00;
                }
                entry->unk14 = out1;
                entry->unk18 = out2;
                func_80021A70(1 << slot);
                result = 1;
            }
        }
    }
    return result;
}
