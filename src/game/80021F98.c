// Categories: Game Engine > Audio
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

/**
 *  @brief Resolves a bank entry and initializes a sound channel slot.
 *
 *  The selected record supplies a default value when args requests one.
 *  Successful initialization marks the slot through a bitmask call.
 *  @param a0 Bank index checked against the global bank count.
 *  @param a1 Index passed to the bank lookup.
 *  @param a2 Record index within the resolved data.
 *  @param a3 Unused argument.
 *  @param slot Channel slot to initialize and mark.
 *  @param args Channel value and inputs for deriving two additional values.
 *  @return 1 on successful resolution and initialization, otherwise 0.
 *  @par Globals
 *  D_800A07A8 supplies the bank table, count and channel storage.
 *  @par Control flow
 *  Checks the bank index and pointers before initializing the slot.
 *  @par Calls
 *  func_80021800 resolves data; func_800223B0 derives values; func_80021A70 marks the slot.
 *  @par Side effects
 *  Writes the selected channel entry and invokes the slot-marking operation.
 *  @par Callers
 *  Confirmed: func_80024358.
 */
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
