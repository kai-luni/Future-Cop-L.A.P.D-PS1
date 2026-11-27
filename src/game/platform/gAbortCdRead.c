// Categories: Platform > CD-ROM | Platform > Interrupts
typedef int s32;
typedef unsigned int u32;
struct State {
    s32 count, buffer, unk8, mode, command;
    volatile s32 unk14;
    s32 unk18;
    u32 time;
    s32 unk20;
    volatile s32 busy;
    s32 callback, callback2, callback3;
};
extern struct State gCdAsyncReadState __asm__("D_8009CBD0");
extern s32 gCdUseDmaFlag __asm__("D_8009C814");
extern s32 gCdPauseCompletionCallbackAddress[] __asm__("D_800908A0");
extern s32 gCdSavedCallbackPtr[] __asm__("D_8009CBF8");
extern s32 gCdSavedReadCbsPtr[] __asm__("D_8009CBFC");
extern u32 gVSyncWait(s32) __asm__("func_80091270");
extern s32 gSetCdSyncCallback(s32) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(s32) __asm__("func_8008E1A8");
extern s32 gSetCdDmaCallback(s32) __asm__("func_8008E5D8");
extern s32 gCdDataSync(s32) __asm__("func_8008E5FC");
extern void gCdControlF(s32,s32) __asm__("func_8008E2F8");

static __inline__ s32 wait_ready(struct State *state)
{
    register u32 start __asm__("$17");
    register volatile s32 *busy __asm__("$16");
    busy = (volatile s32 *)state;
    if (!busy[9]) return 1;
    start = gVSyncWait(-1);
    if (busy[9]) {
        busy += 9;
        do {
            if (gVSyncWait(-1) - start >= 121) {
                busy = gCdSavedCallbackPtr;
                __asm__ volatile("" : "+r"(busy));
                gSetCdSyncCallback(*busy);
                busy -= 10;
                __asm__("" : "+r"(busy));
                ((s32 *)busy)[9] = 0;
                return -1;
            }
        } while (*busy);
    }
    return 0;
}

static __inline__ void wait_again(s32 *callbacks)
{
    register u32 start __asm__("$16");
    register volatile s32 *busy __asm__("$17");
    busy = callbacks;
    if (!busy[-2]) return;
    start = gVSyncWait(-1);
    if (busy[-2]) {
        busy -= 2;
        do {
            if (gVSyncWait(-1) - start >= 121) {
                register volatile s32 *p __asm__("$16");
                p = gCdSavedCallbackPtr;
                __asm__ volatile("" : "+r"(p));
                gSetCdSyncCallback(*p);
                p -= 10;
                __asm__("" : "+r"(p));
                p[9] = 0;
                return;
            }
        } while (*busy);
    }
}
void gAbortCdRead(void) __asm__("func_80090E00");


/**
 * @brief Cancels an active CD read by waiting for pending transfer completion, restoring callbacks, and issuing CD command 9.
 *
 * Confirmed: calls func_8008E5FC(0) when D_8009C814 & 1 is set, clears D_8009CBD0.unk14, runs wait_ready on D_8009CBD0.busy (polling func_80091270(-1) up to 121 ticks), restores D_8009CBFC[0..1] via func_8008E1A8/func_8008E5D8, installs D_800908A0 (func_800908A0) via func_8008E194, issues func_8008E2F8(9, 0), sets busy = 1, and waits via wait_again.
 * Category follows CD-ROM state D_8009CBD0, mode flag D_8009C814, callback hooks func_8008E194/func_8008E1A8/func_8008E5D8, and CD command retry func_8008E2F8.
 * Likely: PSX libcd CdReadBreak routine issuing CdlPause (command 9) with a 121-frame timeout fallback; Unknown: exact meaning of D_8009CBD0.unk14 vs busy.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009C814 - CD flags (bit 0 gates func_8008E5FC/func_8008E5D8); D_8009CBD0 / D_8009CBF8 / D_8009CBFC - CD read state and saved callback words; D_800908A0 - func_800908A0 completion handler address.
 * @par Control flow
 * Optional mode-sync call, inline wait_ready poll loop (121-tick timeout), callback swap and command-9 dispatch when wait_ready returns nonzero (idle or timed out), then inline wait_again loop.
 * @par Calls
 * func_8008E5FC - resets mode when D_8009C814 & 1; func_80091270 - reads counter D_8009DD50; func_8008E1A8/func_8008E5D8/func_8008E194 - swap CD/data callbacks; func_8008E2F8 - sends CD command 9.
 * @par Side effects
 * Modifies D_8009CBD0.unk14 and busy, swaps CD/DMAC callbacks via func_8008E194/func_8008E1A8/func_8008E5D8, and issues CD command 9.
 * @par Callers
 * Confirmed: none in pkg_m1.md, analysis/edges.json, or asm/us/main.s / Likely: unreferenced PSX libcd CdReadBreak routine in this build / Unknown: any indirect caller outside static references.
 *
 * @par Role and path
 * Cancels an active CD-ROM read by syncing DMA via func_8008E5FC(0) when
 * D_8009C814 & 1, zeroing D_8009CBD0.unk14, and waiting up to 121 ticks for
 * busy to clear. If busy was already clear or timed out, it restores saved
 * callbacks from D_8009CBFC[0..1], installs func_800908A0 via func_8008E194,
 * issues CdlPause via func_8008E2F8(9, 0), sets busy = 1, and waits again up
 * to 121 ticks.
 */
void gAbortCdRead(void)
{
    register s32 *callbacks __asm__("$17");
    register struct State *state __asm__("$16");
    struct State *other;
    if (gCdUseDmaFlag & 1) gCdDataSync(0);
    state = &gCdAsyncReadState;
    __asm__ volatile("" : "+r"(state));
    state->unk14 = 0;
    if (wait_ready(state)) {
        __asm__ volatile("");
        callbacks = gCdSavedReadCbsPtr;
        __asm__ volatile("" : "+r"(callbacks));
        gSetCdReadyCallback(callbacks[0]);
        if (gCdUseDmaFlag & 1) gSetCdDmaCallback(callbacks[1]);
        gSetCdSyncCallback((s32)&gCdPauseCompletionCallbackAddress);
        gCdControlF(9,0);
        other = (struct State *)(callbacks - 11);
        __asm__("" : "+r"(other));
        other->busy = 1;
        wait_again(callbacks);
    }
}
