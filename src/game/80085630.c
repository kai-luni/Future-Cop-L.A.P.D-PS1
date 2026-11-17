// Categories: Graphics > Video Decode > Frame Slice Upload | Platform > DMA | Resources > Movie Playback | Platform > MDEC
#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    s32 unk24;
    s32 unk28[2];
    s32 unk30;
    u8 pad34[0xE0];
    s32 unk114;
    s32 unk118;
    s32 unk11C;
} State;

typedef struct {
    u16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Local;

extern State *gDecodeJob __asm__("D_8009F4BC");
extern u32 gStripScaleEnable __asm__("D_8009F4C0");
extern u32 D_8009F4C8;
extern u32 D_8009F4D0;

extern s32 func_80095FE8(u16 *value);
extern void func_80096070(s32 destination, u32 size);
extern s32 func_800960CC(s32 mode);
extern void func_80095E90(s32 option);
extern void func_80095FF4(s32 arg0, s32 arg1);
extern void func_80089BBC(u16 *arg0, s32 arg1);
extern void func_80092F38(Local *local, s32 arg1);

/**
 * @brief Uploads one slice of a video frame: reads the 16-byte slice header,
 *        selects the destination buffer, transfers the payload and flips the
 *        double-buffer index.
 *
 * @param data   Slice header. Byte order is big endian: the tag at +0x0 is
 *               compared with 0x4D444543 ("MDEC"), two 16-bit fields at +0x8
 *               and +0x0A are byte swapped, and the payload starts at +0x10.
 * @param count  Slice size; multiplied by D_8009F4C8 and halved before it is
 *               stored in the 16-bit field at +0x0 of the descriptor.
 * @param volume Volume or offset added to the descriptor field at +0x2, plus
 *               0x100 when ctx+0x114 is set.
 *
 * @return Nothing.
 *
 * @par Globals
 * - D_8009F4BC (0x8009F4BC, $gp 0x2A8): the playback context. Fields used:
 *   +0x24 destination address, +0x28/+0x2C the two buffer pointers,
 *   +0x30 the current buffer index, +0x114 a flag that raises the volume by
 *   0x100, +0x118 the call counter, +0x11C a "frame submitted" flag.
 * - D_8009F4C0 (0x8009F4C0, $gp 0x2AC): control word passed to func_80095FF4.
 * - D_8009F4C8 (0x8009F4C8, $gp 0x2B4): row/pixel rate; scaled by 8 to form the
 *   descriptor stride and by count to form the first descriptor field.
 * - D_8009F4D0 (0x8009F4D0, $gp 0x2BC): running maximum of func_80095FE8's
 *   result, updated only when the new value is larger.
 *
 * @par Control flow
 * Confirmed:
 * 1. magic = byte swapped word 0; s3 = byte swapped halfword at +0x0A;
 *    s2 = byte swapped halfword at +0x08; src = data + 0x10.
 * 2. The local descriptor at sp+0x10 is filled: +0x0 = (count * rate) >> 1,
 *    +0x2 = volume (volume + 0x100 when ctx+0x114 != 0), +0x4 = rate << 3,
 *    +0x6 = s3.
 * 3. result = func_80095FE8(src); D_8009F4D0 = max(D_8009F4D0, result).
 * 4. First call (ctx+0x118 == 0): when the tag matches, func_80089BBC(src,
 *    ctx+0x28[ctx+0x30]) decodes the bit stream into the buffer selected by
 *    ctx+0x30; then ctx+0x11C = 0. Nothing is transferred on this path.
 * 5. Later calls: func_80095FF4(ctx+0x28[1 - ctx+0x30], D_8009F4C0) prepares the
 *    other buffer's control word, then
 *    func_80096070(ctx+0x24, ((s3 * (rate << 3)) >> 1) * (s2 >> 4)) starts the
 *    transfer, the optional decode follows, func_800960CC(0) waits for it, and a
 *    loop over s2 in steps of 0x10 calls func_80092F38(&descriptor, dst),
 *    advancing dst by (descriptor+0x4 * descriptor+0x6) << 1 and the
 *    descriptor's first field by rate << 3. Finally ctx+0x11C = 1.
 * 6. func_80095E90(1) in both cases, then ctx+0x118++ and
 *    ctx+0x30 = 1 - ctx+0x30, so the next slice writes the other buffer.
 *
 * @par Calls
 * func_80095FE8, func_80095FF4, func_80096070, func_80089BBC (up to twice),
 * func_800960CC, func_80092F38 (per 0x10 rows), func_80095E90.
 *
 * @par Side effects
 * Uploads image data to the destination held in ctx+0x24, rewrites two buffer
 * control words, increments the call counter, flips the buffer index, and can
 * raise D_8009F4D0. The decode call is issued twice for a tagged slice - once per
 * path - because the tag test is repeated in both branches.
 *
 * @par Likely role
 * The per-slice feeder of the movie player: the header layout (4-byte tag,
 * width and height, payload at +0x10) plus the two alternating buffer pointers
 * and the "first call only decodes" special case fit a streaming decoder that
 * hands every slice to the GPU and the MDEC bit stream decoder in turn.
 *
 * @par Unknown
 * - The exact semantics of the +0x08 and +0x0A fields. They are used as a row
 *   count (the loop advances by 0x10) and as a stride multiplier, but nothing in
 *   the function proves they are width and height.
 * - Why the tag test is repeated in the second branch instead of being folded
 *   once; it can only cost a second func_80089BBC call for the same slice.
 * - The unit of @c count, which is scaled and halved before being stored as a
 *   16-bit field and is never used otherwise.
 *
 * @par Callers
 * func_8008589C, recorded at 0x800858A0 (the direct call is at 0x800858C8); that
 * wrapper calls this function only when ctx+0x12C == 0 and func_80085228
 * otherwise.
 */
void func_80085630(u8 *data, u32 count, s32 volume)
{
    Local local;
    u32 magic;
    u32 s4;
    u32 s3;
    u32 s2;
    s32 s1;
    u32 i;
    u32 result;
    u16 *src;
    u8 *p;
    State *st;

    magic = *(u32 *)data;
    s4 = (magic >> 24) | ((magic >> 8) & 0xFF00) |
         ((magic & 0xFF00) << 8) | (magic << 24);

    p = data + 8;
    s3 = (*(u16 *)(p + 2) >> 8) |
         ((*(u16 *)(p + 2) & 0xFF) << 8);
    s2 = (*(u16 *)(data + 8) >> 8) |
         ((*(u16 *)(data + 8) & 0xFF) << 8);

    if (gDecodeJob->unk114 != 0) {
        local.unk2 = volume + 0x100;
    } else {
        local.unk2 = volume;
    }

    {
        register u32 rate __asm__("$2") = D_8009F4C8;
        local.unk6 = s3;
        local.unk0 = (count * rate) >> 1;
        local.unk4 = rate << 3;
    }

    src = (u16 *)(p + 8);
    result = func_80095FE8(src);
    if (D_8009F4D0 < result) {
        D_8009F4D0 = result;
    }

    st = gDecodeJob;
    if (st->unk118 == 0) {
        if (s4 == 0x4D444543) {
            func_80089BBC(src, st->unk28[st->unk30]);
        }
        *(volatile s32 *)&gDecodeJob->unk11C = 0;
    } else {
        s1 = st->unk24;
        func_80095FF4(st->unk28[1 - st->unk30], gStripScaleEnable);
        func_80096070(s1, ((s3 * (D_8009F4C8 << 3)) >> 1) * (s2 >> 4));
        if (s4 == 0x4D444543) {
            func_80089BBC(src, gDecodeJob->unk28[gDecodeJob->unk30]);
        }
        func_800960CC(0);
        for (i = 0; i < s2; i += 0x10) {
            func_80092F38(&local, s1);
            s1 += (local.unk4 * local.unk6) << 1;
            { u32 step = D_8009F4C8 << 3; local.unk0 += step; }
        }
        gDecodeJob->unk11C = 1;
    }

    func_80095E90(1);

    gDecodeJob->unk118++;
    gDecodeJob->unk30 = 1 - gDecodeJob->unk30;
}
