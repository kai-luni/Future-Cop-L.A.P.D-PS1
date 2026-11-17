// Categories: Game Engine > Event Queue | Platform > Controller Input
#include "game/types.h"

struct Unk800A0BC0Entry {
    u16 unk00;
    u8 unk02;
    u8 unk03[5];
};

struct Unk800A0BC0 {
    s16 unk00;
    s16 unk02;
    struct Unk800A0BC0Entry entries[8];
};

extern u32 gScreenState[] __asm__("D_8009FA00");
extern u8 gBitFieldState __asm__("D_8009F6B8");
extern u8 gLatchFlag __asm__("D_8009F6C0");
extern struct Unk800A0BC0 gRingBuffer __asm__("D_800A0BC0");

extern s32 func_800452C8(void);
extern void func_80029DD8(void);
extern void func_800298EC(s32 arg0);

/**
 * @brief Queues an event value into the eight-slot ring buffer.
 *
 * The push only happens while the screen state D_8009FA00[0] is below 5 and
 * func_800452C8 reports an active frame. If no slot is free yet
 * (D_800A0BC0.unk00 == 0) and the pad flags in D_8009F6B8 show no pending bits,
 * func_800298EC is called directly and nothing is queued. Otherwise the pad
 * flags decide: bit 0x40 only sets the latch D_8009F6C0, everything else calls
 * func_80029DD8 first. The slot written is index-1 with wraparound, the value
 * goes into entries[i].unk00, the entry is marked unused and the ring count
 * stops growing at 8.
 *
 * @param arg0 Event value pushed by the script VM.
 *
 * @par Globals
 * D_8009FA00 - screen state; D_8009F6B8 - pad status flags; D_8009F6C0 - latch;
 * D_800A0BC0 - ring buffer (count at 0x00, head at 0x02, eight entries of 8 bytes).
 *
 * @par Control flow
 * Guard, then either the direct call or the ring insert with a wrapping index.
 *
 * @par Calls
 * func_800452C8 - frame/test query. func_800298EC - immediate handler.
 * func_80029DD8 - flush/advance of the ring consumer.
 *
 * @par Side effects
 * Writes one entry, the head index and the fill count.
 *
 * @par Callers
 * Confirmed: func_80069D68 (script VM).
 */
void func_80029974(s32 arg0)
{
    s32 index;

    if (gScreenState[0] < 5 && func_800452C8() != 0) {
        if (gRingBuffer.unk00 <= 0 && (gBitFieldState & 0x70) == 0) {
            func_800298EC(arg0);
        } else {
            if (gBitFieldState & 0x40) {
                gLatchFlag = 1;
            } else {
                func_80029DD8();
            }

            index = gRingBuffer.unk02 - 1;
            if (index < 0) {
                index = 7;
            }

            gRingBuffer.entries[index].unk00 = arg0;
            gRingBuffer.entries[index].unk02 = 0;
            gRingBuffer.unk02 = index;

            if (gRingBuffer.unk00 < 8) {
                gRingBuffer.unk00++;
            }
        }
    }
}
