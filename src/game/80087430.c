// Categories: Platform > Ring Buffer
#include "game/types.h"

typedef struct {
    u32 unk0;
    u32 unk4;
    u8 pad08[8];
    u8 buf0[0x3C00];
    u8 buf1[0x3C00];
} Ring;

extern volatile Ring *gMovieAudioState __asm__("D_8009F4D8");
extern void gExpand15To16Padded(u8 *dst, u8 *src, u32 count) __asm__("func_8008731C");

/**
 *  @brief Copies two packed input streams into a pair of ring buffers.
 *
 *  Derives each stream length from the byte-swapped header word, rounds it
 *  down to 15-byte groups, and accounts for 16 ring bytes per group.
 *  @param arg0 Input block containing the header and two consecutive streams.
 *  @par Globals
 *  gMovieAudioState supplies the ring buffers and producer and consumer counters.
 *  @par Control flow
 *  Busy-waits for space, then copies each stream either once or in two pieces
 *  when the destination wraps around the 0x3C00-byte ring.
 *  @par Calls
 *  func_8008731C copies each contiguous stream segment into its ring buffer.
 *  @par Side effects
 *  Writes both ring buffers and advances the producer counter.
 *  @par Callers
 *  Confirmed: gMovieStreamDemux.
 */
void gMovieAudioChunkPushSplit(u8 *arg0) __asm__("func_80087430");

void gMovieAudioChunkPushSplit(u8 *arg0)
{
    u32 magic;
    u32 s0;
    u32 s1;
    u32 s5;
    u32 used;
    u8 *s2;
    u8 *s4;

    magic = *(u32 *)(arg0 + 4);
    s0 = (((magic >> 24) | ((magic >> 8) & 0xFF00) |
           ((magic & 0xFF00) << 8) | (magic << 24)) - 0x10) >> 1;
    s0 = s0 / 15 * 15;
    s2 = arg0 + 0x10;
    s4 = s2 + s0;
    s5 = s0 / 15 * 16;

    do {
        used = gMovieAudioState->unk4 - 0x3C00;
    } while (gMovieAudioState->unk0 - used < s5);

    s1 = ((0x3C00 - gMovieAudioState->unk4 % 0x3C00) >> 4) * 15;

    if (s1 >= s0) {
        gExpand15To16Padded(gMovieAudioState->buf0 + gMovieAudioState->unk4 % 0x3C00, s2, s0);
        gExpand15To16Padded(gMovieAudioState->buf1 + gMovieAudioState->unk4 % 0x3C00, s4, s0);
    } else {
        gExpand15To16Padded(gMovieAudioState->buf0 + gMovieAudioState->unk4 % 0x3C00, s2, s1);
        gExpand15To16Padded(gMovieAudioState->buf0, s2 + s1, s0 - s1);
        gExpand15To16Padded(gMovieAudioState->buf1 + gMovieAudioState->unk4 % 0x3C00, s4, s1);
        gExpand15To16Padded(gMovieAudioState->buf1, s4 + s1, s0 - s1);
    }
    gMovieAudioState->unk4 += s5;
}
