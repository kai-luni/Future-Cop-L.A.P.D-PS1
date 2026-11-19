// Categories: Resources > Streaming | Platform > CD-ROM
#include "game/types.h"

typedef struct {
    volatile s32 unk00;
    s32 unk04;
    volatile s32 unk08;
    s32 unk0C;
    s32 unk10;
    void (*unk14)(s32, s32);
    s32 unk18;
    s32 unk1C;
    u8 *unk20;
    u8 unk24;
} GlobalState;

typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
} StreamEntry;

extern GlobalState func_80010000;
extern StreamEntry gFileSlotTable[] __asm__("D_800A6D78");
extern u8 gLengthTable[] __asm__("D_800A85C0");

extern void func_8008CD10(void *dest, const void *src, s32 n);
void gCopyCdSectorChunk(void) __asm__("func_8001CC6C");


/**
 * @brief Copies one chunk of a pending stream into the caller's buffer.
 *
 * The source offset inside the streaming buffer is the sector offset of the
 * open file (0x7FF mask), so a chunk never crosses a sector boundary: the
 * length is clamped to 0x800 and reduced by the offset. func_8008CD10 copies
 * the bytes, then the read pointer, the remaining count and the file position
 * are advanced by the transferred length. When nothing is left the job is
 * marked idle, the file slot is marked done (plus 0x100 when the caller asked
 * for it) and the completion callback stored in the state block is invoked
 * with the transferred byte count and the accumulator.
 *
 * @par Globals
 * func_80010000 - stream state (handle 0x10, remaining 0x04, pointer 0x20,
 * accumulator 0x08, callback 0x14, flag 0x24);
 * D_800A6D78 - four file slots (status 0x00, sector offset 0x0C);
 * D_800A85C0 - streaming buffer.
 *
 * @par Control flow
 * One clamp, one copy, then a completion branch.
 *
 * @par Calls
 * func_8008CD10 - block copy.
 *
 * @par Side effects
 * Advances the stream state, the file slot and calls the completion callback.
 *
 * @par Callers
 * Confirmed: func_8001CE54.
 *
 * @par Role and path
 * Copies up to one sector boundary (0x800) of buffered CD data from
 * gLengthTable (D_800A85C0) at offset gFileSlotTable[0x80010000.unk10].unk0C
 * & 0x7FF into 0x80010000.unk20 via func_8008CD10. Advances pointers and slot
 * position, and when remaining bytes reach 0 marks the slot done and calls
 * 0x80010000.unk14; called by func_8001CE54.
 */
void gCopyCdSectorChunk(void)
{
    s32 chunk;
    s32 offset;

    func_80010000.unk0C = 0;
    offset = gFileSlotTable[func_80010000.unk10].unk0C & 0x7FF;

    chunk = func_80010000.unk04 + offset;
    if (chunk > 0x800) {
        chunk = 0x800;
    }
    chunk -= offset;

    func_8008CD10(func_80010000.unk20, &gLengthTable[offset], chunk);

    func_80010000.unk20 += chunk;
    func_80010000.unk04 -= chunk;
    __asm__ volatile("");
    func_80010000.unk08 += chunk;
    gFileSlotTable[func_80010000.unk10].unk0C += chunk;

    if (func_80010000.unk04 <= 0) {
        func_80010000.unk00 = 0;
        gFileSlotTable[func_80010000.unk10].unk00 = 1;
        if (func_80010000.unk24 == 1) {
            gFileSlotTable[func_80010000.unk10].unk00 |= 0x100;
        }
        func_80010000.unk14(func_80010000.unk08, func_80010000.unk0C);
    }
}
