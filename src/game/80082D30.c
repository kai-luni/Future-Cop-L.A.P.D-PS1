// Categories: Game Engine > Timing
#include "game/types.h"

typedef struct Unk8009F468 {
    u8 pad[0x1C];
    s32 (*unk1C)(s32);
} Unk8009F468;

extern Unk8009F468 *D_8009F468;
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");

/**
 *  @brief Tests a callback mask and elapsed time to trigger a timed result.
 *
 *  A nonzero timestamp and a matching callback result are required before
 *  the elapsed-time threshold is checked and the timestamp is adjusted.
 *  @param arg0 Pointer to the timestamp, with zero disabling the test.
 *  @param arg1 Mask tested against the callback result.
 *  @return 1 if the threshold is reached, otherwise 0.
 *  @par Globals
 *  D_8009F468 supplies the callback; D_8009F8B8 is its argument.
 *  D_8009FA2C supplies the volatile current time value.
 *  @par Control flow
 *  Requires a callback, nonzero timestamp, matching mask, and elapsed time of at least 0xB5.
 *  @par Calls
 *  The callback at D_8009F468 offset 0x1C supplies the mask value.
 *  @par Side effects
 *  On success, writes the current time minus 0x96 to arg0; the callback may have effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_80082D30(s32 *arg0, s32 arg1)
{
    s32 result = 0;
    s32 (*callback)(s32) = D_8009F468->unk1C;

    if (callback != 0) {
        if (*arg0 != 0) {
            if (callback(gActivePlayerIndex) & arg1) {
                if (gFrameTickCounter - *arg0 >= 0xB5) {
                    result = 1;
                    *arg0 = gFrameTickCounter - 0x96;
                }
            }
        }
    }
    return result;
}
