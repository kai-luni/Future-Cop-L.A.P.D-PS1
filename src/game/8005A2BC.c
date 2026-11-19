// Categories: Graphics > Textures | Resources > Descriptor Table
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

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern s32 gCountTrailingZeros(u32 value) __asm__("func_800195F0");
extern void func_8004979C(void *value);
s32 gBuildSpriteDescTable(void **array, s32 count) __asm__("func_8005A2BC");


/**
 * @brief Builds the active sprite/tile descriptor table and publishes it in D_8009F77C.
 *
 * Allocates count * 12 + 4 bytes: a header holding count followed by count 12-byte
 * entries. Each entry keeps the tile count from byte +2, the bit length of byte +3
 * (func_800195F0 returns the number of significant bits) and a pointer to the
 * payload at +8.
 *
 * Every 8-byte tile record of the caller's data is then rewritten in place: byte +6
 * gains 0x80 and byte +7 is stamped with (col << 6) | row, with col walking across
 * four columns before wrapping into the next row. The input data is mutated, not
 * only inspected - a second call would add 0x80 again.
 *
 * The table becomes the active one through func_8004979C. That symbol belongs to a
 * module whose lifecycle is verified: func_80049690 creates it (with a pool of 384
 * blocks of 68 bytes) and func_8004963C frees the table.
 *
 * Hypothesis, not verified: the two stamped bytes are the UV/CLUT halfword of a PSX
 * primitive and 0x80 is a 128-texel u offset. Only the offsets and the 4-column
 * wrapping are established.
 *
 * @par Calls
 * func_80017EBC - allocates the descriptor table.
 * func_800195F0 - significant-bit count of the tile-count byte.
 * func_8004979C - publishes the table as the active one.
 *
 * @par Callers
 * Confirmed: func_8005A414.
 *
 * @par Texture load path
 * Called by func_8005A414 after VRAM upload; allocates 12-byte
 * descriptor entries (+0 tile count, +1 bit length via func_800195F0, +8 payload
 * pointer), rewrites each 8-byte tile record in place (byte +6 += 0x80,
 * byte +7 = (col << 6) | row across 4 columns), and activates the table via
 * func_8004979C (D_8009F77C).
 *
 * @par Environment path
 * Texture descriptor step of the Cpyr atlas loader, not a primitive emitter:
 * for each source block it stores byte 2 (tile count), func_800195F0 of byte
 * 3 (trailing-zero count, i.e. log2 of a power-of-two size) and a pointer to
 * the 8-byte tile records at +8. Each tile record is patched in place: its
 * byte 6 gets 0x80 added and byte 7 becomes (col shifted left 6) or row, with
 * col starting at byte 4 shifted right 4 and row at byte 5, wrapping every
 * four columns. The table is then handed to func_8004979C; called only by
 * func_8005A414 after it uploaded the pixels.
 */
s32 gBuildSpriteDescTable(void **array, s32 count)
{
    Struct5A2BC *header;
    s32 i;
    s32 j;
    s32 col;
    s32 row;

    header = gAllocMemory(count * 12 + 4, 2);
    if (header == 0) {
        return 0;
    }

    header->count = count;

    for (i = 0; i < count; i++) {
        header->items[i].unk0 = ((u8 *)array[i])[2];
        header->items[i].unk1 = gCountTrailingZeros(((u8 *)array[i])[3]);
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
