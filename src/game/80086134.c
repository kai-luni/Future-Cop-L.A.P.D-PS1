// Categories: Graphics > Video Decode | Platform > MDEC
#include "game/types.h"

/* gp-relative small data: offsets 0x734 and 0x738 from $gp */
extern s32 D_8009F948;
extern s32 D_8009F94C;

extern void func_80089A88(s32 arg0, s32 arg1, s32 arg2);
extern void func_80085F1C(void);

/**
 * @brief Hands the two decode parameters to the video decoder and marks it busy.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_80089A88(D_8009F948, D_8009F94C, 0) passes the two small-data words ($gp
 * offsets 0x734/0x738) with a zero, then func_80085F1C starts the decode step.
 *
 * @par Likely role
 * The pair (source address, length/count) of the next MDEC slice; the zero is
 * the mode or the "don't wait" flag.
 *
 * @par Callers
 * The frame loop of the video playback path.
 */
void func_80086134(void)
{
    func_80089A88(D_8009F948, D_8009F94C, 0);
    func_80085F1C();
}
