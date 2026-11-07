#include "game/types.h"

struct Entry { u16 count; u16 value; };
struct ByteEntry { u8 count; u8 value; };
struct Data { s8 count; u8 pad[19]; struct Entry *entries[3]; struct ByteEntry *last; };
extern s32 D_8009F7C0;
extern s32 output[2] __asm__("D_8009F7C0");
extern s32 D_8009F7C4;
extern struct Data *D_8009F7D0;

s32 func_80059DB8(void)
{
    struct Data *data;
    struct Entry *entry;
    struct ByteEntry *last;
    s32 i;
    s32 count;
    struct Data *countdata;
    register s32 *out __asm__("$7");
    if (D_8009F7C0 != 0 || D_8009F7C4 != 0)
        return -1;
    data = D_8009F7D0;
    count = data->count;
    __asm__ volatile("" : "+r"(count));
    i = 0;
    if (count > 0) {
    countdata = data;
    out = output;
    do {
        entry = data->entries[i];
        if (entry->count == 0) {
            entry++;
            if (entry->count == 0)
                return -1;
            data->entries[i] = entry;
        }
        entry->count--;
        *out = entry->value << 16;
        i++;
        out++;
    } while (i < countdata->count);
    }
    last = data->last;
    if (last->count == 0) {
        last++;
        data->last = last;
    }
    last->count--;
    return last->value;
}
