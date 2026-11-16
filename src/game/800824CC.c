// Categories: Resources > Texture Atlas | Graphics > Image Transfer | Platform > GPU
#include "game/types.h"

typedef struct Unk824CCEntry {
    u32 unk00;
    s16 unk04;
    s16 unk06;
} Unk824CCEntry;

typedef struct Unk824CCHeader {
    u16 type;
    u16 count;
    u32 magic;
    Unk824CCEntry entries[100];
} Unk824CCHeader;

typedef struct Unk824CCObject {
    Unk824CCHeader *header;
} Unk824CCObject;

extern void func_800953F0(Unk824CCEntry *entry, u8 *ptr);
extern void func_800181D4(Unk824CCObject *object);

/**
 * @brief Uploads texture atlas image blocks from a Ctdt resource via func_800953F0 and frees object.
 *
 * Validates header->type == 2 and magic == 0x35BFBC73 on object->header, computes the pixel payload pointer immediately past the 8-byte header and header->count 8-byte entries, submits each rectangle to func_800953F0, and frees object.
 *
 * @param object Loaded resource node (+0x00 header pointer to Unk824CCHeader).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Checks header->type == 2 and header->magic == 0x35BFBC73; if valid and header->count != 0, computes ptr = (u8 *)header - ((100 - count) * 8 - 0x328) (= header + 8 + 8 * count) and loops count times calling func_800953F0(entry, ptr) and advancing ptr by unk04 * (unk06 << 1). Unconditionally calls func_800181D4(object).
 *
 * @par Calls
 * func_800953F0 - transfers one {unk00, unk04, unk06} entry rectangle from ptr.
 * func_800181D4 - frees the resource wrapper object.
 *
 * @par Side effects
 * Dispatches GPU/device transfers via func_800953F0 and frees object.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (0x800251D0, asset loader dispatch on tag 0x43746474).
 */
void func_800824CC(Unk824CCObject *object)
{
    register Unk824CCHeader *header __asm__("$19");
    register u8 *ptr __asm__("$20");
    register s32 i __asm__("$17");
    register Unk824CCEntry *cursor __asm__("$16");
    register s32 offset __asm__("$18");
    register s32 byte_count __asm__("$6");
    register s32 table_size __asm__("$2");

    header = object->header;
    if (header->type == 2 && header->magic == 0x35BFBC73) {
        i = 0;
        table_size = (100 - header->count) * 8 - 0x328;
        __asm__("" : "=r"(table_size) : "0"(table_size));
        ptr = (u8 *)header - table_size;
        if (header->count != 0) {
            cursor = (Unk824CCEntry *)header;
            offset = 8;
            do {
                func_800953F0(
                    (Unk824CCEntry *)((u8 *)header + offset), ptr);
                byte_count = cursor[1].unk04 *
                             (cursor[1].unk06 << 1);
                cursor++;
                offset += 8;
                ptr += byte_count;
                i++;
            } while (i < header->count);
        }
    }
    func_800181D4(object);
}
