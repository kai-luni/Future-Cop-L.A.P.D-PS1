// Categories: Platform > GPU | Graphics > GPU Command Lists
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    void (*callback)(s32, s32, s32, s32);
    u8 pad0C[0xC];
    s32 unk18;
} Dispatch;

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(u8 *, s32);
extern Dispatch *D_8009DDE8;
extern u8 D_80012AE8[];

/**
 * @brief Hands an ordering table to the GPU (DrawOTag).
 *
 * @param arg0 Start of the primitive list.
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * With debug level D_8009DDF2 >= 2 the hook is called with D_80012AE8, then the
 * library callback at D_8009DDE8+8 is invoked as
 * callback(unk18, list, 0, 0) - the two trailing zeroes are the standard
 * "no extra parameters" form of the DMA submission.
 *
 * @par Globals
 * D_8009DDE8 (GPU library state), D_8009DDF2 (debug level), D_8009DDEC (hook).
 *
 * @par Callers
 * func_8001B4A8 only, once per frame, with the list built from the ordering
 * table buckets.
 */
void func_80093280(s32 arg0)
{
    Dispatch *dispatch;
    register s32 a1 __asm__("$5");
    register s32 zero __asm__("$6");
    if (D_8009DDF2 >= 2) {
        D_8009DDEC(D_80012AE8, arg0);
    }
    a1 = arg0;
    __asm__ volatile("" : "+r"(a1));
    dispatch = D_8009DDE8;
    __asm__ volatile("" : "+r"(dispatch));
    zero = 0;
    __asm__ volatile("" : "+r"(zero));
    dispatch->callback(dispatch->unk18, a1, zero, 0);
}
