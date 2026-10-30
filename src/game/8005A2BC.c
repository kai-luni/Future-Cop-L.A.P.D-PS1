#include "game/types.h"

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2[6];
    u8 *unk8;
} Struct5A2BCItem;

typedef struct {
    s32 count;
    Struct5A2BCItem items[1];
} Struct5A2BC;

extern void *func_80017EBC(s32 size, s32 tag);
extern s32 func_800195F0(u32 value);
extern void func_8004979C(void *value);

s32 func_8005A2BC(void **array, s32 count)
{
    Struct5A2BC *header;
    s32 i;
    s32 j;
    s32 col;
    s32 row;

    header = func_80017EBC(count * 12 + 4, 2);
    if (header == 0) {
        return 0;
    }

    header->count = count;

    for (i = 0; i < count; i++) {
        header->items[i].unk0 = ((u8 *)array[i])[2];
        header->items[i].unk1 = func_800195F0(((u8 *)array[i])[3]);
        row = ((u8 *)array[i])[5];
        col = ((u8 *)array[i])[4] >> 4;
        header->items[i].unk8 = (u8 *)array[i] + 8;
        for (j = 0; j < header->items[i].unk0; j++) {
            register u32 offset __asm__("$4") = (u32)j << 3;
            {
                register u8 *bytes __asm__("$3") = (u8 *)array[i];
                bytes += offset;
                bytes[0xE] += 0x80;
            }
            {
                register u32 packed __asm__("$3") = (u32)col << 6;
                register u8 *bytes __asm__("$2");
                col++;
                bytes = (u8 *)array[i];
                packed |= (u32)row;
                bytes += offset;
                bytes[0xF] = packed;
            }
            if (col == 4) {
                col = 0;
                row++;
            }
        }
    }

    func_8004979C(header);
    return 1;
}
