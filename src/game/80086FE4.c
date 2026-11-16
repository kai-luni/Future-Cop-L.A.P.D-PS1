// Categories: Resources > Movie Playback | Platform > Sound
#include "game/types.h"

struct Unk9F4D8 {
    volatile u32 unk00;
    volatile u32 unk04;
    u8 unk08[0x3C08];
};

extern struct Unk9F4D8 *D_8009F4D8;
extern void func_8008CD10(u8 *arg0, u8 *arg1, u32 arg2);

#define BSWAP(p) ((*(u32 *)(p) >> 24) | ((*(u32 *)(p) >> 8) & 0xFF00) | ((*(u32 *)(p) & 0xFF00) << 8) | (*(u32 *)(p) << 24))

/**
 * @brief Demuxes an audio movie chunk into the dual-channel 0x3C00-byte ring buffer D_8009F4D8.
 *
 * Byte-swaps the big-endian chunk header: "ad*" (0x61640000) skips 0x10 header bytes while "Ad*" skips 0x0C, and "ad2*" (0x61643200) selects split stereo (halving s1, right channel at s2 + s1) rather than duplicating mono. Spins until D_8009F4D8 has s1 free bytes, copies into +0x10 and +0x3C10 (wrapping at 0x3C00), and advances unk04.
 *
 * @param arg0 Pointer to big-endian audio chunk header and payload.
 *
 * @par Globals
 * D_8009F4D8 - dual 0x3C00-byte audio ring buffer struct (unk00 read cursor, unk04 write cursor, +0x10/+0x3C10 rings).
 *
 * @par Control flow
 * Header tag/size decode, spin-wait loop for ring space, and wrap-splitting copy branch (s0 >= s1 vs two-piece copy).
 *
 * @par Calls
 * func_8008CD10 - copies payload bytes into the left (+0x10) and right (+0x3C10) ring buffers.
 *
 * @par Side effects
 * Writes D_8009F4D8 ring buffers at +0x10 and +0x3C10 and advances D_8009F4D8->unk04 by s1.
 *
 * @par Callers
 * Confirmed: func_800879DC (0x80087C64, movie stream demuxer) / Likely: none / Unknown: none.
 */
void func_80086FE4(u8 *arg0)
{
    u32 flag;
    u8 *s2;
    u32 s1;
    u8 *s6;
    u32 s0;
    u32 s4;
    u8 *s5;
    u32 bsw;
    struct Unk9F4D8 *p;

    bsw = BSWAP(arg0);
    flag = bsw & 0xFFFFFF00;
    flag = (flag == 0x61643200);

    if ((bsw & 0xFFFF0000) == 0x61640000) {
        s2 = arg0 + 0x10;
        s1 = BSWAP(arg0 + 4) - 0x10;
    } else {
        s2 = arg0 + 0xC;
        s1 = BSWAP(arg0 + 4) - 0xC;
    }

    if (flag) {
        s1 = s1 >> 1;
        s6 = s2 + s1;
    } else {
        s6 = s2;
    }

    for (;;) {
        u32 f4 = D_8009F4D8->unk04 - 0x3C00;
        u32 f0 = D_8009F4D8->unk00;

        if (!(f0 - f4 < s1)) {
            break;
        }
    }

    p = D_8009F4D8;
    s0 = 0x3C00 - (p->unk04 % 0x3C00);

    if (s0 >= s1) {
        func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x10, s2, s1);
        p = D_8009F4D8;

        if (flag) {
            func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x3C10, s6, s1);
        } else {
            func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x3C10, s2, s1);
        }
    } else {
        func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x10, s2, s0);
        s5 = s2 + s0;
        s4 = s1 - s0;
        func_8008CD10((u8 *)D_8009F4D8 + 0x10, s5, s4);
        p = D_8009F4D8;

        if (flag) {
            func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x3C10, s6, s0);
            func_8008CD10((u8 *)D_8009F4D8 + 0x3C10, s6 + s0, s4);
        } else {
            func_8008CD10((u8 *)((p->unk04 % 0x3C00) + (u32)p) + 0x3C10, s2, s0);
            func_8008CD10((u8 *)D_8009F4D8 + 0x3C10, s5, s4);
        }
    }

    D_8009F4D8->unk04 = D_8009F4D8->unk04 + s1;
}
