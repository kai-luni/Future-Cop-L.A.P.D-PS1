// Categories: Graphics > Buffer Switching
#include "game/types.h"

typedef struct {
    u8 unk00[0x34];
    u8 unk34[2][92];
    u8 unkEC[2][20];
    s32 unk114;
    u8 unk118[4];
    s32 unk11C;
} Unk8009F4BC;

extern Unk8009F4BC *gMovieSliceContext __asm__("D_8009F4BC");
extern void gDrawSync(s32) __asm__("func_80092C8C");
extern void gVSyncWait(s32) __asm__("func_80091270");
extern void gPutDispEnvCached(void *) __asm__("func_800934BC");
extern void gPutDrawEnv(void *) __asm__("func_800932F0");

/**
 *  @brief Processes the current buffer and switches to the other buffer.
 *
 *  When enabled, this routine invokes two setup calls, passes the current
 *  20-byte buffer to a helper, and selects the other 92-byte buffer.
 *  @par Globals
 *  gMovieSliceContext supplies the enable flag, buffer arrays, and current index.
 *  @par Control flow
 *  If unk11C is zero, returns without action; otherwise toggles unk114 as 1 minus
 *  its current value and passes the newly selected buffer to the final helper.
 *  @par Calls
 *  func_80092C8C and func_80091270 receive zero; func_800934BC receives the
 *  current unkEC buffer; func_800932F0 receives the newly selected unk34 buffer.
 *  @par Side effects
 *  Updates unk114 and invokes helpers that may have additional effects.
 *  @par Callers
 *  Confirmed: gMovieStreamDemux.
 */
void gMoviePresentFrame(void) __asm__("func_80085914");

void gMoviePresentFrame(void)
{
    s32 t;

    if (gMovieSliceContext->unk11C != 0) {
        gDrawSync(0);
        gVSyncWait(0);
        gPutDispEnvCached((void *)&gMovieSliceContext->unkEC[gMovieSliceContext->unk114][0]);
        t = 1 - gMovieSliceContext->unk114;
        gMovieSliceContext->unk114 = t;
        gPutDrawEnv((void *)&gMovieSliceContext->unk34[t][0]);
    }
}
