// Categories: Game Engine > Event Queue
#include "game/types.h"

extern u32 gScreenState __asm__("D_8009FA00");
extern s32 func_800452C8(void);

typedef struct {
    s16 value;
    u8 state;
    u8 reserved[5];
} QueueSlot298EC;

typedef struct {
    u16 length;
    s16 baseIndex;
    QueueSlot298EC slots[8];
} Queue298EC;

extern Queue298EC gRingBuffer __asm__("D_800A0BC0");

/**
 * @brief Appends a value to the eight-slot event ring at its next free position.
 *
 * Unlike func_80029974, which writes at index-1 and only when the ring already
 * holds entries, this variant appends: the new slot is (baseIndex + length) & 7
 * and the length grows by one, so the ring fills from the base index
 * outwards. Both the screen state (D_8009FA00 below 5) and func_800452C8 must
 * allow the push, and a full ring (8 pending) drops the value silently.
 *
 * @param arg0 Event value to append.
 *
 * @par Globals
 * D_800A0BC0 - ring buffer (length at 0x00, base index at 0x02, eight 8-byte slots);
 * D_8009FA00 - screen state word.
 *
 * @par Control flow
 * Two guards, then one bounded append.
 *
 * @par Calls
 * func_800452C8 - frame/activity query.
 *
 * @par Side effects
 * Writes one slot and increments the ring length.
 *
 * @par Callers
 * Confirmed: func_80069D68 (script VM). Likely: func_80029974 (immediate path).
 */
void func_800298EC(s16 arg0)
{
    if (gScreenState < 5) {
        if (func_800452C8() != 0) {
            s16 pending = gRingBuffer.length;
            if (pending < 8) {
                s32 slot = (gRingBuffer.baseIndex + pending) & 7;
                gRingBuffer.slots[slot].value = arg0;
                gRingBuffer.slots[slot].state = 0;
                gRingBuffer.length++;
            }
        }
    }
}
