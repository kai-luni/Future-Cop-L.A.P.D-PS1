#include "game/types.h"

typedef struct TableEntry {
    u16 id;
    u16 offset;
} TableEntry;

typedef struct TableHeader {
    u16 unk0;
    u16 count;
    TableEntry entries[1];
} TableHeader;

typedef struct Unk800899A4 {
    TableHeader *header;
    u8 unk04[0x18];
    s32 unk1C;
} Unk800899A4;

extern Unk800899A4 *D_8009F518;

u8 *func_80089914(u16 id)
{
    register u8 *result __asm__("$9") = 0;
    if (D_8009F518 != 0) {
        register TableHeader *header __asm__("$2") = D_8009F518->header;
        register s32 count __asm__("$3");
        register s32 low __asm__("$7");
        register s32 high __asm__("$6");
        register TableEntry *entries __asm__("$8");
        register TableEntry *entry __asm__("$5");

        low = (s32)result;
        count = header->count;
        entries = header->entries;
        high = count - 1;

        do {
            s32 mid = (low + high) / 2;
            register s32 diff __asm__("$2");
            entry = &entries[mid];
            diff = id - entry->id;

            if (diff < 0) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
            if (diff == 0) {
                goto found;
            }
        } while (low <= high);
        goto done;
found:
        {
            register u32 offset __asm__("$2") = entry->offset;
            result = (u8 *)entries + offset;
        }
    }
done:
    return result;
}
