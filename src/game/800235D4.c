#include "game/types.h"

typedef struct { u16 id, count, offset; } Entry;
typedef struct { u16 unk, count; Entry entries[1]; } Header;
typedef struct { u8 value; s8 id; u8 pad[14]; } Mapping;
typedef struct { u16 key; u8 value; u8 pad[9]; } Item;
extern Mapping D_800A0A00[];
extern Header *func_80023528(s32, s32 *);
extern s32 func_800241D0(s32, s32, s32, s32, void *);

s32 func_800235D4(s32 id)
{
    register s32 result __asm__("$16") = 0;
    s32 index;
    Header *header;
    s32 i, j, count;
    s8 value;
    Item *item;

    header = func_80023528(id, &index);
    value = -1;
    if (header != 0) {
        __asm__("" : "+r"(result));
        for (i = 0; i < 8; i++) {
            if (D_800A0A00[i].id == id) {
                value = D_800A0A00[i].value;
                break;
            }
        }
        if (value != -1) {
            item = (Item *)((u8 *)header + header->entries[index].offset);
            j = 0;
            if (j < header->entries[index].count) {
                do {
                    func_800241D0(item->key, item->value, (s8)id, value, item);
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
