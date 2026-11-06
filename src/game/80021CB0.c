#include "game/types.h"

typedef struct { s32 unused[2]; s32 value; } Entry;
typedef struct { s32 count; Entry **items; } Table;
extern Table D_800A07A8;
extern s32 func_80021898(Entry *, s32, s32, s32);
extern void func_800219EC(Entry *, s32);

s32 func_80021CB0(Entry *entry, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    Entry **cursor = D_800A07A8.items;
    s32 index = 0;
    s32 result;
    s32 amount;
    if (arg2 < 0) {
        while (*cursor++) {
            index++;
            if (index > D_800A07A8.count)
                { result = -1; goto done; }
        }
    } else {
        index = arg2;
    }
    result = -2;
    if (D_800A07A8.items[index])
        goto done;
    amount = func_80021898(entry, arg1, arg3, arg4);
    if (amount <= 0) { result = -3; goto done; }
    entry->value = amount;
    func_800219EC(entry, amount);
    D_800A07A8.items[index] = entry;
    result = index;
done:
    return result;
}
