// Categories: Game Engine > Scheduling
#include "game/types.h"

extern u8 gMixerState[] __asm__("D_800A0968");
extern volatile u32 gStreamEventCallback __asm__("D_8009F634");

extern void gSubmitAudioStreamTransfer(void) __asm__("func_80022D84");

void gCompleteAudioStreamTransfer(s32 arg0) __asm__("func_80023204");

/**
 * @brief Advances stream DMA state when the request sequence matches.
 *
 * @param arg0 Identifier compared as a 16-bit value with the state identifier.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Compares the low 16 bits of a0 with mixer +0x10 and increments +0x20. On completing a cycle
 * it marks ready, clears per-channel source pointers and calls the event handler with 3. A
 * negative argument saves the offset before wrap handling and the next eligible transfer.
 *
 * @par Globals
 * Updates gMixerState, including the indices and source words, and reads gStreamEventCallback.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Installed as the completion callback in gSubmitAudioStreamTransfer; the retail listing
 * addresses it as gCompleteAudioStreamTransfer.
 *
 * @par Unknown
 * The callback consumes the high bit as a sign flag while the low halfword identifies the
 * request.
 */
void gCompleteAudioStreamTransfer(s32 arg0)
{
    u8 *base;
    u8 *p;
    register u32 i __asm__("$4");
    u32 cnt;
    register u32 max __asm__("$3");
    u32 fn;
    u32 a;
    u32 b;
    register u32 prod __asm__("$6");
    u32 sum;
    u32 m;
    u8 *q;

    base = gMixerState;
    if ((u16)arg0 != *(volatile u16 *)(base + 0x10)) {
        return;
    }
    cnt = *(volatile u32 *)(base + 0x20);
    max = *(volatile u32 *)(base + 0x14);
    cnt = cnt + 1;
    *(volatile u32 *)(base + 0x20) = cnt;
    if (cnt < max) {
        goto do_call;
    }
    i = 0;
    *(volatile u32 *)(base + 0x80) = 1;
    *(u32 *)(base + 0x20) = 0;
    if (max != 0) {
        p = base;
    loop:
        *(volatile u32 *)(p + 0x34) = 0;
        m = *(volatile u32 *)(base + 0x14);
        i++;
        p += 0x24;
        if (i < m) {
            goto loop;
        }
    }
    fn = gStreamEventCallback;
    if (fn != 0) {
        ((void (*)(s32))fn)(3);
    }
    q = gMixerState;
    __asm__ __volatile__("" ::: "memory");
    if (arg0 < 0) {
        *(volatile u32 *)(q + 0x40) = *(volatile u32 *)(q + 0x78);
    }
    a = *(volatile u32 *)(q + 0x6C);
    b = *(volatile u32 *)(q + 0x70);
    prod = b * a;
    sum = *(volatile u32 *)(q + 0x78) + a;
    *(volatile u32 *)(q + 0x78) = sum;
    if (sum == prod) {
        *(volatile u32 *)(q + 0x78) = 0;
    }
    if (*(volatile u32 *)(q + 0x34) == 0) {
        return;
    }
    if (*(volatile u32 *)(q + 0x74) == 0) {
        return;
    }
do_call:
    gSubmitAudioStreamTransfer();
}
