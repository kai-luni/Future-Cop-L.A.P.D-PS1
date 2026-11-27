/* Platform client layer: 35 unchanged src/game units of the CD/SPU/GPU/MDEC
 * library band (0x8008D-0x8009) that no other suite reaches. Coherent groups:
 *  - CD driver clients: the boolean read wrappers (gCdRead/gCdGetSector), the
 *    mode forwarders (gCdDataSync/gCdSync/gCdReady/gResetCdController), the
 *    retail CdInit driver bootstrap (gStartCdInit + gInitCdHardware), the three
 *    fixed-value event dispatchers, the callback exchange clients, the command
 *    name table lookup, the 12-byte filename matcher and the pause callback.
 *  - GPU library state: the debug-level exchange with hook notification, the
 *    packed tpage/clut dump dispatchers, the display environment printer,
 *    the DrawSync callback slot, the VSync callback dispatcher and the CLUT
 *    upload wrappers.
 *  - MDEC/SPU clients: the DMA wrapper, the two busy-flag sync queries, the
 *    option dispatch reset, the SPU warm start, the SPU transfer sequence
 *    issuers and the two top-level SPU write entry points.
 * Every callee is replaced by a stand-in bound to the retail label (see
 * fc_testkit.h rule 1); only func_8008CD10 (CD memcpy) comes from the shared
 * bios_mock object, like in every other suite. `source` class expectations are
 * hand-worked characterizations of the decompiled body, not independent
 * retail measurements.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
#include "bios_mock.h"

/* ========================================================================
 * Global storage
 * ======================================================================== */

/* CD driver tables (linked objects access them via extern views). */
u8 *gCdSyncCallback_g __asm__("D_8009C8A8");
u8 *gCdReadyCallback_g __asm__("D_8009C8AC");
u8 *gCdReadCallback_g __asm__("D_8009C810");
s32 gCdUseDmaFlag_g __asm__("D_8009C814");
/* XA command name pointer table 8008E0EC; the fallback address is a constant
 * (0x80010000 + 0x233C) never dereferenced. */
void *gIndexEntryTable_g[28] __asm__("D_8009C8CC");
/* 800908A0: saved-callback word D_8009CBF8 and the busy flag word right
 * before it (the unit writes p[-10+9] = one word below the label). */
__asm__("\t.data\n"
        "\t.balign 16\n"
        "\t.globl D_8009DD30\n"
        "D_8009DD30:\n\t.space 32\n"
        "\t.globl D_8009DD50\n"
        "D_8009DD50:\n\t.space 4\n"
        "\t.globl D_8009DDE0\n"
        "D_8009DDE0:\n\t.space 12\n"
        "\t.globl D_8009DDEC\n"
        "D_8009DDEC:\n\t.space 4\n"
        "\t.globl D_8009DE00\n"
        "D_8009DE00:\n\t.space 0x5C\n"
        "\t.globl D_8009DE5C\n"
        "D_8009DE5C:\n\t.space 0x14\n"
        "\t.globl D_8009DE70\n"
        "D_8009DE70:\n\t.space 16\n"
        "\t.text\n");
/* 8008DEA0 copies the ADDRESSES of the retail labels D_8008DF6C/94/BC into
 * the driver table. The harness owns those labels here (the retail objects
 * behind them are dispatchers whose code is irrelevant to the copy itself),
 * so the expectation is address equality with these harness symbols. */
u8 gCdSyncEvtLabel[4] __asm__("D_8008DF6C");
u8 gCdReadyEvtLabel[4] __asm__("D_8008DF94");
u8 gCdReadEvtLabel[4] __asm__("D_8008DFBC");

/* 800908A0 reads the saved callback word D_8009CBF8 and the busy word at
 * D_8009CBF4 (one word below the label p[-10+9]). */
__asm__("\t.data\n"
        "\t.balign 4\n"
        "\t.globl D_8009CBF4\n"
        "D_8009CBF4:\n\t.space 4\n"
        "\t.globl D_8009CBF8\n"
        "D_8009CBF8:\n\t.space 4\n"
        "\t.text\n");
extern s32 gCdSavedBusyFlag __asm__("D_8009CBF4");
extern s32 gCdSavedCallbackPtr[] __asm__("D_8009CBF8");
/* 8008DEA0's failure message (address taken only, never dereferenced). */
u8 gCdInitFailMsg_g[24] __asm__("D_8001231C");
extern u8 gGpuStateBase[] __asm__("D_8009DDE0");
/* Debug cluster D_8009DDF0..3: flag byte at +2 (retail D_8009DDF2), its
 * p[-2]/p[1] neighbours at +0/+3. One C array keeps the bytes adjacent;
 * D_8009DDF2/3 alias into it via same-object .set (leaf_entity pattern). */
u8 gGpuDebugCluster[4] __asm__("D_8009DDF0");
__asm__("\t.globl D_8009DDF2\n\t.set D_8009DDF2, D_8009DDF0+2\n"
        "\t.globl D_8009DDF3\n\t.set D_8009DDF3, D_8009DDF0+3\n");
#define GPU4(off_9Dxxx) (*(u32 *)(gGpuStateBase + ((off_9Dxxx) - 0x9DDE0)))
#define GPUB(off_9Dxxx) (gGpuStateBase[(off_9Dxxx) - 0x9DDE0])
/* the retail trace tag strings: address-taken only */
u8 gSetGraphDebugTraceFormat[8] __asm__("D_800129D4");
u8 gTPageTraceFormat[8] __asm__("D_8001289C");
u8 gClutTraceFormat[8] __asm__("D_800128B4");
u8 gExecutableImageBase[16] __asm__("D_80010000");

/* VSync dispatch (80091BF8): D_8009DD30/DD50 live in the GPU state block
 * above; the units bind to those labels directly. */
extern volatile s32 gVSyncTickCounter __asm__("D_8009DD50");
extern void (*gVSyncCallbackTable[8])(void) __asm__("D_8009DD30");

/* Env caches live inside the GPU state block defined above. */
extern u8 gDrawEnvCache[] __asm__("D_8009DE00");
extern u8 gDispEnvCache[] __asm__("D_8009DE5C");
u16 gClutData32[256];

/* SPU transfer state (800976DC/80097760). */
u16 gSpuTransferAddr __asm__("D_8009EEBC");
s32 gSpuAddrShift __asm__("D_8009EECC");
s32 gOpOrderFlag __asm__("D_8009EEC0");
s32 gSpuClearFlag __asm__("D_8009EED8");
s32 gSpuTransferCallback __asm__("D_8009EEDC");

/* ========================================================================
 * Units under test (prototypes copied from the units' __asm__ aliases)
 * ======================================================================== */
s32 gCdRead(void *destination, s32 size) __asm__("func_8008E598");
s32 gCdGetSector(void *destination, s32 size) __asm__("func_8008E5B8");
s32 gCdDataSync(s32 mode) __asm__("func_8008E5FC");
s32 gCdSync(s32 mode, u8 *result) __asm__("func_8008E154");
s32 gCdReady(s32 mode, u8 *result) __asm__("func_8008E174");
void gResetCdController(void) __asm__("func_8008E0B8");
void *gGetCdCommandName(unsigned char index) __asm__("func_8008E0EC");
u32 gInitCdHardware(void) __asm__("func_8008DF30");
void gLogCdSyncCallback(void) __asm__("func_8008DF6C");
void gLogCdReadyCallback(void) __asm__("func_8008DF94");
void gLogCdReadCallback(void) __asm__("func_8008DFBC");
s32 gStartCdInit(void) __asm__("func_8008DEA0");
void gApplyBiosStateValue(s32 arg0) __asm__("func_8008D690");
void gDispatchVSyncCallbacks(void) __asm__("func_80091BF8");
int gSetGraphDebug(int value) __asm__("func_80092A84");
void gDumpTPage(unsigned short value) __asm__("func_80092284");
void gPrintClutCoordinates(unsigned short value) __asm__("func_800922E4");
typedef void (*PlatCallback)(void);
PlatCallback gDrawSyncCallback(PlatCallback callback) __asm__("func_80092B94");
typedef struct { s16 values[8]; u8 first; u8 second; } PlatValues;
void gCdPauseCallback(void) __asm__("func_800908A0");
s32 gMatchCdFilename(u8 *a, u8 *b) __asm__("func_800901F8");
u8 *gCopyDrawEnv(u8 *arg0) __asm__("func_80093488");
u8 *gCopyDispEnv(u8 *arg0) __asm__("func_800938F0");
void gStartMdecOutDma(s32 destination, u32 size) __asm__("func_80096070");
s32 gMdecInSync(s32 mode) __asm__("func_80096090");
s32 gMdecOutSync(s32 mode) __asm__("func_800960CC");
void gDecDctReset(s32 option) __asm__("func_80095E90");
void gWarmStartSpu(void) __asm__("func_80097A28");
s32 gStartSpuWrite(s32 arg0, s32 arg1) __asm__("func_80097760");
s32 gSpuWriteTransferBlock(s32 arg0, s32 arg1) __asm__("func_800976DC");
u32 gWriteSpuData(s32 arg0, u32 arg1) __asm__("func_80098E28");
u32 gWriteSpuRam(s32 arg0, u32 arg1) __asm__("func_80098E88");

/* ========================================================================
 * Stand-ins bound to retail labels. Records all in statics, returns are
 * configured per test (unconfigured = 0); freestanding, no libc.
 * ======================================================================== */

/* func_8008FC30 gCdRead's loader (ret (dest,size)); returns ret_8008FC30 */
static s32 ret_8008FC30; static void *saw_rd_dst; static s32 saw_rd_size;
static s32 rd_calls;
s32 pl_read_cd(void *d, s32 s) __asm__("func_8008FC30");
s32 pl_read_cd(void *d, s32 s) { rd_calls++; saw_rd_dst = d; saw_rd_size = s; return ret_8008FC30; }

/* func_8008FD30 gCdGetSector's DMA (ret (dest,size)) */
static s32 ret_8008FD30; static void *saw_sec_dst; static s32 saw_sec_size, sec_calls;
s32 pl_cd_sector(void *d, s32 s) __asm__("func_8008FD30");
s32 pl_cd_sector(void *d, s32 s) { sec_calls++; saw_sec_dst = d; saw_sec_size = s; return ret_8008FD30; }

/* func_8008FAC8 gCdDataSync's poll (mode) */
static s32 ret_8008FAC8, saw_dsync_mode, dsync_calls;
s32 pl_data_sync(s32 m) __asm__("func_8008FAC8");
s32 pl_data_sync(s32 m) { dsync_calls++; saw_dsync_mode = m; return ret_8008FAC8; }

/* func_8008ECFC gCdSync's loop (mode,result) */
static s32 ret_8008ECFC, saw_csync_mode, csync_calls; static u8 *saw_csync_res;
s32 pl_cd_sync(s32 m, u8 *r) __asm__("func_8008ECFC");
s32 pl_cd_sync(s32 m, u8 *r) { csync_calls++; saw_csync_mode = m; saw_csync_res = r; return ret_8008ECFC; }

/* func_8008EF7C gCdReady's loop (mode,result) */
static s32 ret_8008EF7C, saw_crdy_mode, crdy_calls; static u8 *saw_crdy_res;
s32 pl_cd_ready(s32 m, u8 *r) __asm__("func_8008EF7C");
s32 pl_cd_ready(s32 m, u8 *r) { crdy_calls++; saw_crdy_mode = m; saw_crdy_res = r; return ret_8008EF7C; }

/* func_8008F6D8 gResetCdController's target */
static s32 cdreset_calls;
void pl_cd_reset(void) __asm__("func_8008F6D8");
void pl_cd_reset(void) { cdreset_calls++; }

/* func_8008F8E8 / func_8008F7AC gInitCdHardware's two helpers */
static s32 ret_8008F8E8, sub_calls; static s32 ret_8008F7AC, mix_calls;
u32 pl_init_cd_sub(void) __asm__("func_8008F8E8");
u32 pl_init_cd_sub(void) { sub_calls++; return (u32)ret_8008F8E8; }
u32 pl_init_cd_mix(void) __asm__("func_8008F7AC");
u32 pl_init_cd_mix(void) { mix_calls++; return (u32)ret_8008F7AC; }

/* func_8008DFF0 gDeliverEvent(spec, value) */
static s32 evt_calls; static u32 evt_a0[8], evt_a1[8];
void pl_deliver_event(u32 a0, s32 a1) __asm__("func_8008DFF0");
void pl_deliver_event(u32 a0, s32 a1)
{
    if (evt_calls < 8) { evt_a0[evt_calls] = a0; evt_a1[evt_calls] = a1; }
    evt_calls++;
}

/* func_8008E000 BIOS printf stub used by gStartCdInit's failure path */
static u8 *saw_failmsg; static s32 msg_calls;
void pl_bios_printf(void *fmt) __asm__("func_8008E000");
void pl_bios_printf(void *fmt) { msg_calls++; saw_failmsg = (u8 *)fmt; }

/* func_80090890 BIOS strncmp (real semantics so equality cases are honest) */
static s32 ret_80090890; static u8 *saw_sncmp_a, *saw_sncmp_b; static s32 saw_sncmp_n, sncmp_calls;
s32 pl_bios_strncmp(u8 *a, u8 *b, s32 n) __asm__("func_80090890");
s32 pl_bios_strncmp(u8 *a, u8 *b, s32 n)
{
    sncmp_calls++; saw_sncmp_a = a; saw_sncmp_b = b; saw_sncmp_n = n;
    if (ret_80090890 != 0) { ret_80090890 = 0; return ret_80090890; }
    if (!a || !b) return 1;
    {
        s32 i;
        for (i = 0; i < n; i++) {
            if (a[i] != b[i]) return 1;   /* inequality is all the caller tests */
            if (!a[i]) return 0;
        }
        return 0;
    }
}

/* func_8008E194 the pause-restore client's exchange helper: returns the
 * previous word of D_8009C8A8 and installs the new one (the retail unit's
 * contract; linked because func_800908A0 calls it). */
static s32 ret_8008E194;
s32 pl_cd_sync_callback(s32 value) __asm__("func_8008E194");
s32 pl_cd_sync_callback(s32 value)
{
    s32 old = (s32)(unsigned long)gCdSyncCallback_g;
    gCdSyncCallback_g = (u8 *)(unsigned long)value;
    return old;
}

/* func_8008D6D0 / func_8008D670 gApplyBiosStateValue's two helpers */
static s32 anc_arg, anc_calls; static s32 wcs_a0, wcs_a1, wcs_a2, wcs_calls;
void pl_allow_new_card(s32 a) __asm__("func_8008D6D0");
void pl_allow_new_card(s32 a) { anc_calls++; anc_arg = a; }
void pl_write_card_sector(s32 a0, s32 a1, s32 a2) __asm__("func_8008D670");
void pl_write_card_sector(s32 a0, s32 a1, s32 a2) { wcs_calls++; wcs_a0=a0; wcs_a1=a1; wcs_a2=a2; }

/* func_80091480 / func_80096150 gDecDctReset's two helpers */
static s32 irqreset_calls; static s32 mdec_calls, mdec_last_opt;
void pl_reset_irq_dispatch(void) __asm__("func_80091480");
void pl_reset_irq_dispatch(void) { irqreset_calls++; }
void pl_reset_mdec(s32 option) __asm__("func_80096150");
void pl_reset_mdec(s32 option) { mdec_calls++; mdec_last_opt = option; }

/* func_800962D0 gStartMdecOutDma's target */
static s32 prog_calls; static s32 prog_dst; static u32 prog_size;
void pl_program_mdec_dma(s32 d, u32 s) __asm__("func_800962D0");
void pl_program_mdec_dma(s32 d, u32 s) { prog_calls++; prog_dst = d; prog_size = s; }

/* func_8009635C / func_80096484 / func_800963F0 MDEC sync helpers */
static s32 ret_8009635C, wait_calls; static u32 ret_80096484; static s32 ret_800963F0, outwait_calls;
s32 pl_mdec_wait_in(void) __asm__("func_8009635C");
s32 pl_mdec_wait_in(void) { wait_calls++; return ret_8009635C; }
u32 pl_mdec_busy(void) __asm__("func_80096484");
u32 pl_mdec_busy(void) { return ret_80096484; }
s32 pl_mdec_wait_out(void) __asm__("func_800963F0");
s32 pl_mdec_wait_out(void) { outwait_calls++; return ret_800963F0; }

/* func_80096D58 gWarmStartSpu's target */
static s32 spuinit_arg, spuinit_calls;
void pl_spu_init_subsystem(s32 cold_start) __asm__("func_80096D58");
void pl_spu_init_subsystem(s32 cold_start) { spuinit_calls++; spuinit_arg = cold_start; }

/* func_8009745C gSpuTransfer(op, ...). All observed call sites pass at most
 * three word arguments, which travel in a0..a2 for both a variadic and a
 * fixed three-parameter prototype (o32), so a fixed stand-in reads the same
 * registers. */
static s32 tr_ops[3], tr_a1[3], tr_a2[3], tr_calls;
void pl_spu_transfer(s32 op, s32 x1, s32 x2) __asm__("func_8009745C");
void pl_spu_transfer(s32 op, s32 x1, s32 x2)
{
    if (tr_calls < 3) { tr_ops[tr_calls] = op; tr_a1[tr_calls] = x1; tr_a2[tr_calls] = x2; }
    tr_calls++;
}

/* func_80097138 gSpuWriteMemBlock */
static s32 wb_a0, wb_a1, wb_calls;
void pl_spu_write_mem_block(s32 a0, s32 a1) __asm__("func_80097138");
void pl_spu_write_mem_block(s32 a0, s32 a1) { wb_calls++; wb_a0 = a0; wb_a1 = a1; }

/* The debug hook D_8009DDEC: one generic recorder. The wrapper's five value
 * slots cover every calling shape seen in the tested units (2 to 5 words). */
static s32 log_calls; static void *log_fmt;
static u32 log_a0, log_a1, log_a2, log_a3;
static void pl_log_hook(void *fmt, u32 a0, u32 a1, u32 a2, u32 a3)
{
    log_calls++; log_fmt = fmt;
    log_a0 = a0; log_a1 = a1; log_a2 = a2; log_a3 = a3;
}
static void pl_log_set(void)
{
    GPU4(0x9DDEC) = (u32)(unsigned long)&pl_log_hook;   /* D_8009DDEC */
    log_calls = 0; log_fmt = 0; log_a0 = log_a1 = log_a2 = log_a3 = 0x5A5A5A5A;
}

/* ========================================================================
 * Tests
 * ======================================================================== */

static void test_cd_readwrappers(void)
{
    /* @leaf 8008E598 source src/game/platform/gCdRead.c: forwards (destination,size) to func_8008FC30 and returns 1 exactly when that call returned 0 */
    LK_FUNC("func_8008E598");
    rd_calls = 0; ret_8008FC30 = 0; saw_rd_dst = 0; saw_rd_size = 0;
    FCK_EQ(gCdRead((void *)0x1234, 0x100), 1, "loader completed: 1");
    FCK_EQ(rd_calls, 1, "one read dispatched");
    FCK_EQ((u32)saw_rd_dst, 0x1234, "destination forwarded");
    FCK_EQ(saw_rd_size, 0x100, "size forwarded");
    ret_8008FC30 = 1;
    FCK_EQ(gCdRead((void *)0x2000, 0x40), 0, "partial read: 0");
    ret_8008FC30 = -1;
    FCK_EQ(gCdRead((void *)0x2000, 8), 0, "error read: 0");
    FCK_EQ(rd_calls, 3, "three dispatches total");

    /* @leaf 8008E5B8 source src/game/platform/gCdGetSector.c: forwards (destination,size) to func_8008FD30 and returns 1 exactly when that call returned 0 */
    LK_FUNC("func_8008E5B8");
    sec_calls = 0; ret_8008FD30 = 0;
    FCK_EQ(gCdGetSector((void *)0x5678, 3), 1, "sector DMA done: 1");
    FCK_EQ(sec_calls, 1, "one DMA dispatched");
    FCK_EQ((u32)saw_sec_dst, 0x5678, "destination forwarded");
    FCK_EQ(saw_sec_size, 3, "word count forwarded");
    ret_8008FD30 = 5;
    FCK_EQ(gCdGetSector((void *)0x5678, 1), 0, "pending DMA: 0");

    /* @leaf 8008E5FC source src/game/platform/gCdDataSync.c: forwards the mode word to func_8008FAC8 and returns that result unchanged */
    LK_FUNC("func_8008E5FC");
    dsync_calls = 0; ret_8008FAC8 = -1; saw_dsync_mode = 0;
    FCK_EQ(gCdDataSync(0), -1, "timeout value passes through");
    FCK_EQ(saw_dsync_mode, 0, "blocking mode forwarded");
    ret_8008FAC8 = 1;
    FCK_EQ(gCdDataSync(7), 1, "ready value passes through");
    FCK_EQ(saw_dsync_mode, 7, "mode forwarded again");
    ret_8008FAC8 = 0x100;
    FCK_EQ(gCdDataSync(0x33), 0x100, "no truncation of the callee value");
    FCK_EQ(dsync_calls, 3, "three polls dispatched");

    /* @leaf 8008E154 source src/game/platform/gCdSync.c: forwards mode and result pointer to func_8008ECFC and returns its result */
    LK_FUNC("func_8008E154");
    csync_calls = 0; ret_8008ECFC = -1;
    FCK_EQ(gCdSync(0, (u8 *)0x2468), -1, "sync result forwarded");
    FCK_EQ(saw_csync_mode, 0, "mode forwarded");
    FCK_EQ((u32)saw_csync_res, 0x2468, "result buffer forwarded");
    ret_8008ECFC = 0x22;
    FCK_EQ(gCdSync(0x11, (u8 *)0x1111), 0x22, "non-zero status forwarded");

    /* @leaf 8008E174 source src/game/platform/gCdReady.c: forwards mode and result pointer to func_8008EF7C and returns its result */
    LK_FUNC("func_8008E174");
    crdy_calls = 0; ret_8008EF7C = 3;
    FCK_EQ(gCdReady(1, (u8 *)0x1357), 3, "ready result forwarded");
    FCK_EQ(saw_crdy_mode, 1, "mode forwarded");
    FCK_EQ((u32)saw_crdy_res, 0x1357, "result buffer forwarded");
    ret_8008EF7C = 0;
    FCK_EQ(gCdReady(0, 0), 0, "zero/NULL pass through");

    /* @leaf 8008E0B8 source src/game/platform/gResetCdController.c: single tail call into func_8008F6D8 with no arguments or state of its own */
    LK_FUNC("func_8008E0B8");
    cdreset_calls = 0;
    gResetCdController();
    FCK_EQ(cdreset_calls, 1, "interrupt reset dispatched exactly once");
}

static void test_cdinit(void)
{
    s32 r;

    /* @leaf 8008DF30 source src/game/platform/gInitCdHardware.c: returns 0 when func_8008F8E8 reports a live driver (non-zero), otherwise 1 exactly when func_8008F7AC returns 0 */
    LK_FUNC("func_8008DF30");
    sub_calls = mix_calls = 0;
    ret_8008F8E8 = 1;
    FCK_EQ(gInitCdHardware(), 0, "subsystem ready: 0 without touching the mixer");
    FCK_EQ(mix_calls, 0, "early exit before the mixer call");
    FCK_EQ(sub_calls, 1, "subsystem probed once");
    ret_8008F8E8 = 0; ret_8008F7AC = 0;
    FCK_EQ(gInitCdHardware(), 1, "subsystem down, mixer ok: 1");
    FCK_EQ(mix_calls, 1, "mixer consulted");
    ret_8008F8E8 = 0; ret_8008F7AC = 4;
    FCK_EQ(gInitCdHardware(), 0, "mixer error: 0");
    ret_8008F8E8 = 0; ret_8008F7AC = -1;
    FCK_EQ(gInitCdHardware(), 0, "negative mixer status: 0");

    /* @leaf 8008DEA0 source src/game/platform/gStartCdInit.c: five attempts; only a gInitCdHardware() result of exactly 1 installs D_8008DF6C/94/BC into the three driver words, clears D_8009C814 and returns 1; after the fifth failure it prints D_8001231C via func_8008E000 and returns 0 */
    LK_FUNC("func_8008DEA0");
    gCdSyncCallback_g = (u8 *)(unsigned long)0xDEAD0001u;
    gCdReadyCallback_g = (u8 *)(unsigned long)0xDEAD0002u;
    gCdReadCallback_g = (u8 *)(unsigned long)0xDEAD0003u;
    gCdUseDmaFlag_g = 0x55;
    ret_8008F8E8 = 0; ret_8008F7AC = 0;   /* gInitCdHardware() = 1 on try 1 */
    msg_calls = 0;
    r = gStartCdInit();
    FCK_EQ(r, 1, "first attempt succeeds");
    FCK_EQ(gCdSyncCallback_g, (u32)gCdSyncEvtLabel, "D_8009C8A8 <- D_8008DF6C");
    FCK_EQ(gCdReadyCallback_g, (u32)gCdReadyEvtLabel, "D_8009C8AC <- D_8008DF94");
    FCK_EQ(gCdReadCallback_g, (u32)gCdReadEvtLabel, "D_8009C810 <- D_8008DFBC");
    FCK_EQ(gCdUseDmaFlag_g, 0, "D_8009C814 cleared");
    FCK_EQ(msg_calls, 0, "no failure message on success");

    gCdSyncCallback_g = 0; gCdReadyCallback_g = 0; gCdReadCallback_g = 0;
    gCdUseDmaFlag_g = 1;
    ret_8008F8E8 = 0; ret_8008F7AC = 2;   /* gInitCdHardware() = 0: hard failure */
    msg_calls = 0; saw_failmsg = 0;
    sub_calls = 0; mix_calls = 0;
    r = gStartCdInit();
    FCK_EQ(sub_calls, 5, "exactly five attempts before giving up");
    FCK_EQ(r, 0, "five failures return 0");
    FCK_EQ(msg_calls, 1, "failure message printed once");
    FCK_EQ((u32)saw_failmsg, (u32)&gCdInitFailMsg_g, "the printed string is D_8001231C");
    FCK_EQ(gCdSyncCallback_g, 0, "tables untouched on failure");
    FCK_EQ(gCdUseDmaFlag_g, 1, "DMA flag untouched on failure");
}

static void test_cd_events(void)
{
    /* @leaf 8008DF6C source src/game/platform/gLogCdSyncCallback.c: dispatches the fixed pair (0xF0000003, 0x20) to func_8008DFF0 */
    LK_FUNC("func_8008DF6C");
    evt_calls = 0; evt_a0[0] = evt_a1[0] = 0;
    gLogCdSyncCallback();
    FCK_EQ(evt_calls, 1, "one event delivered");
    FCK_EQ(evt_a0[0], 0xF0000003, "CD event spec word");
    FCK_EQ(evt_a1[0], 0x20, "sync event value 0x20");

    /* @leaf 8008DF94 source src/game/platform/gLogCdReadyCallback.c: dispatches the fixed pair (0xF0000003, 0x40) to func_8008DFF0 */
    LK_FUNC("func_8008DF94");
    evt_calls = 0; evt_a0[0] = evt_a1[0] = 0;
    gLogCdReadyCallback();
    FCK_EQ(evt_calls, 1, "one event delivered");
    FCK_EQ(evt_a0[0], 0xF0000003, "CD event spec word");
    FCK_EQ(evt_a1[0], 0x40, "ready event value 0x40");

    /* @leaf 8008DFBC source src/game/platform/gLogCdReadCallback.c: dispatches the fixed pair (0xF0000003, 0x40) to func_8008DFF0 */
    LK_FUNC("func_8008DFBC");
    evt_calls = 0; evt_a0[0] = evt_a1[0] = 0;
    gLogCdReadCallback();
    FCK_EQ(evt_calls, 1, "one event delivered");
    FCK_EQ(evt_a0[0], 0xF0000003, "CD event spec word");
    FCK_EQ(evt_a1[0], 0x40, "read event value 0x40");

    /* @leaf 8008D690 source src/game/boot-frontend/gApplyBiosStateValue.c: sends the argument to func_8008D6D0 alone, then (argument, 0x3F, 0) to func_8008D670 */
    LK_FUNC("func_8008D690");
    anc_calls = 0; wcs_calls = 0; anc_arg = 0;
    wcs_a0 = wcs_a1 = wcs_a2 = 0;
    gApplyBiosStateValue(0x1234);
    FCK_EQ(anc_calls, 1, "allow-new-card dispatched once");
    FCK_EQ(anc_arg, 0x1234, "argument forwarded");
    FCK_EQ(wcs_calls, 1, "sector write dispatched once");
    FCK_EQ(wcs_a0, 0x1234, "same argument again");
    FCK_EQ(wcs_a1, 0x3F, "fixed middle value 0x3F");
    FCK_EQ(wcs_a2, 0, "fixed tail value 0");
    gApplyBiosStateValue(-1);
    FCK_EQ(anc_arg, -1, "negative argument forwarded");
    FCK_EQ(wcs_a0, -1, "negative argument forwarded to the sector write");
}

/* the retail fallback constant 0x8001233C kept as a signed word to avoid
 * unsigned-truncation noise in the FCK_EQ casts */
static const s32 cd_cmd_fallback = (s32)0x8001233C;

static void test_cd_clients(void)
{
    /* @leaf 8008E0EC source src/game/memory-resources/gGetCdCommandName.c: index < 28 returns D_8009C8CC[index], anything else the constant 0x8001233C */
    LK_FUNC("func_8008E0EC");
    {
        s32 i;
        for (i = 0; i < 28; i++) gIndexEntryTable_g[i] = (void *)(0x3000 + i);
    }
    FCK_EQ((u32)gGetCdCommandName(0), 0x3000, "low boundary index 0");
    FCK_EQ((u32)gGetCdCommandName(27), 0x301B, "high boundary index 27");
    FCK_EQ((u32)gGetCdCommandName(28), (u32)cd_cmd_fallback, "first out-of-range index");
    FCK_EQ((u32)gGetCdCommandName(0xFF), (u32)cd_cmd_fallback, "far out-of-range index");
    FCK_EQ((u32)gGetCdCommandName(0x100), 0x3000, "0x100 truncates to byte 0");
    FCK_EQ((u32)gGetCdCommandName(0x11C), (u32)cd_cmd_fallback, "0x11C truncates to byte 28: fallback");

    /* @leaf 800901F8 source src/game/platform/gMatchCdFilename.c: 12-byte equality via the BIOS strncmp stand-in func_80090890; returns 1 only for equality */
    LK_FUNC("func_800901F8");
    ret_80090890 = 0;
    FCK_EQ(gMatchCdFilename((u8 *)"AAAABBBBCCC", (u8 *)"AAAABBBBCCC"), 1,
           "equal names match");
    FCK_EQ(saw_sncmp_n, 0xC, "comparison length is the 0xC byte name field");
    FCK_EQ(gMatchCdFilename((u8 *)"AAAABBBBCCC", (u8 *)"AAAABBBBCCD"), 0,
           "last byte differs: no match");
    FCK_EQ(gMatchCdFilename((u8 *)"SAME\\0AAAAAAAA", (u8 *)"SAME\\0AAAAAAAA"), 1,
           "embedded terminator with equal bytes still matches");
    FCK_EQ(gMatchCdFilename((u8 *)"AAAAAAAAAAAA", (u8 *)"BBBBBBBBBBBB"), 0,
           "totally different names: no match");

    /* @leaf 800908A0 source src/game/platform/gCdPauseCallback.c: restores *D_8009CBF8 into D_8009C8A8 through func_8008E194 and clears the busy word below the saved slot */
    LK_FUNC("func_800908A0");
    gCdSavedCallbackPtr[0] = 0x7777;
    gCdSavedBusyFlag = 1;
    gCdSyncCallback_g = 0;
    gCdPauseCallback();
    FCK_EQ((u32)gCdSyncCallback_g, 0x7777, "saved callback restored into D_8009C8A8");
    FCK_EQ(gCdSavedBusyFlag, 0, "busy flag word cleared");
}

static s32 fired[8];

static void vsync_cb(int slot)
{
    fired[slot]++;
}

static void vsync_cb_0(void) { vsync_cb(0); }
static void vsync_cb_7(void) { vsync_cb(7); }

/* ========================================================================
 * GPU library state clients. The GPU state block keeps the debug level at
 * D_8009DDF2 with its neighbour bytes inside the same buffer, so the
 * p[-2]/p[1] accesses of the tested units read harness memory.
 * ======================================================================== */

static void test_gpu_stateclients(void)
{
    s32 i;

    /* @leaf 80091BF8 source src/game/boot-frontend/gDispatchVSyncCallbacks.c: increments the volatile tick counter D_8009DD50 once and calls each of the eight D_8009DD30 slots that is non-NULL, skipping NULL slots */
    LK_FUNC("func_80091BF8");
    for (i = 0; i < 8; i++) fired[i] = 0;
    for (i = 0; i < 8; i++) gVSyncCallbackTable[i] = 0;
    gVSyncTickCounter = 41;
    for (i = 0; i < 8; i++) gVSyncCallbackTable[i] = 0;
    gVSyncCallbackTable[0] = vsync_cb_0;
    gVSyncCallbackTable[7] = vsync_cb_7;
    gDispatchVSyncCallbacks();
    FCK_EQ(gVSyncTickCounter, 42, "tick counter incremented by one");
    FCK_EQ(fired[0], 1, "slot 0 dispatched");
    FCK_EQ(fired[7], 1, "slot 7 dispatched");
    for (i = 1; i < 7; i++) FCK_EQ(fired[i], 0, "NULL slot skipped");
    for (i = 0; i < 8; i++) gVSyncCallbackTable[i] = 0;
    for (i = 0; i < 8; i++) fired[i] = 0;
    gVSyncTickCounter = 0;
    gDispatchVSyncCallbacks();
    FCK_EQ(gVSyncTickCounter, 1, "NULL-only table still ticks");
    for (i = 0; i < 8; i++) FCK_EQ(fired[i], 0, "no dispatch with all slots NULL");
    /* double dispatch runs each slot twice */
    gVSyncCallbackTable[3] = vsync_cb_0;
    gDispatchVSyncCallbacks();
    gDispatchVSyncCallbacks();
    FCK_EQ(fired[0], 2, "slot 0 dispatched twice across two ticks");
    for (i = 0; i < 8; i++) gVSyncCallbackTable[i] = 0;
}



/* ========================================================================
 * GPU debug exchange and packed-field dump dispatchers
 * ======================================================================== */

static void test_gpu_debugclients(void)
{
    /* @leaf 80092A84 source src/game/platform/gSetGraphDebug.c: returns and replaces the flag byte at D_8009DDF2[0]; a nonzero new value also runs the printf hook with the trace tag and the bytes at p[0]/p[-2]/p[1] */
    LK_FUNC("func_80092A84");
    pl_log_set();
    gGpuDebugCluster[0] = 0x40;
    gGpuDebugCluster[2] = 0x11;
    gGpuDebugCluster[3] = 0x66;
    FCK_EQ(gSetGraphDebug(0), 0x11, "old flag returned");
    FCK_EQ(gGpuDebugCluster[2], 0, "flag cleared to zero");
    FCK_EQ(log_calls, 0, "zero new value does not run the hook");
    FCK_EQ(gSetGraphDebug(0x88), 0, "previous zero returned");
    FCK_EQ(gGpuDebugCluster[2], 0x88, "nonzero flag installed");
    FCK_EQ(log_calls, 1, "nonzero change runs the hook once");
    FCK_EQ(log_a0, 0x88, "hook arg1 is the new flag byte");
    FCK_EQ(log_a1, 0x40, "hook arg2 is the byte two below (p[-2])");
    FCK_EQ(log_a2, 0x66, "hook arg3 is the byte above (p[1])");

    /* @leaf 80092284 source src/game/gpu-2d-hud/gDumpTPage.c: splits the packed tpage into four masked fields ((v>>7)&3, (v>>5)&3, (v<<6)&0x7C0, ((v<<4)&0x100)|((v>>2)&0x200)) and passes them with the tag address to the hook */
    LK_FUNC("func_80092284");
    pl_log_set();
    gDumpTPage(0x81);
    FCK_EQ(log_calls, 1, "one hook call");
    FCK_EQ(log_a0, (0x81 >> 7) & 3, "field 1 = (v>>7)&3");
    FCK_EQ(log_a1, (0x81 >> 5) & 3, "field 2 = (v>>5)&3");
    FCK_EQ(log_a2, (0x81 << 6) & 0x7C0, "field 3 = (v<<6)&0x7C0");
    FCK_EQ(log_a3, ((0x81 << 4) & 0x100) | ((0x81 >> 2) & 0x200), "field 4 packs bits 12 and 9 of v");
    gDumpTPage(0);
    FCK_EQ(log_calls, 2, "zero value still dispatches");
    FCK_EQ(log_a0, 0, "zero fields for a zero tpage");
    FCK_EQ(log_a1, 0, "zero fields for a zero tpage (m)");
    FCK_EQ(log_a2, 0, "zero fields for a zero tpage (h)");
    FCK_EQ(log_a3, 0, "zero fields for a zero tpage (st)");
    gDumpTPage(0x2FF);
    FCK_EQ(log_a0, (0x2FF >> 7) & 3, "high bits reach field 1 (masked to 3)");
    FCK_EQ(log_a1, (0x2FF >> 5) & 3, "high bits reach field 2 (masked to 3)");
    FCK_EQ(log_a2, (0x2FF << 6) & 0x7C0, "field 3 masks to 0x7C0");
    FCK_EQ(log_a3, 0x100, "0x2FF case: bit 8 of value<<4 set, bit 9 of value>>2 clear");

    /* @leaf 800922E4 source src/game/platform/gPrintClutCoordinates.c: passes ((v & 0x3F) << 4) and (v >> 6) with the tag address to the hook */
    LK_FUNC("func_800922E4");
    pl_log_set();
    gPrintClutCoordinates(0x123);
    FCK_EQ(log_calls, 1, "one hook call");
    FCK_EQ((u32)log_fmt, (u32)&gExecutableImageBase[0x28b4], "format tag is image base + 0x28B4");
    FCK_EQ((log_a0 & 0xFFFF), (0x123 & 0x3F) << 4, "low 6 bits shifted left by 4");
    FCK_EQ(log_a1, 0x123 >> 6, "upper bits shifted right by 6");
    gPrintClutCoordinates(0);
    FCK_EQ(log_calls, 2, "zero word dispatches");
    FCK_EQ(log_a0, 0, "zero fields");
    FCK_EQ(log_a1, 0, "zero fields (high)");
    gPrintClutCoordinates(0xFFFF);
    FCK_EQ((log_a0 & 0xFFFF), 0x3F0, "maximal low field");
    FCK_EQ(log_a1, 0x3FF, "maximal high field");
}

/* ========================================================================
 * MDEC/SPU and env-copy clients
 * ======================================================================== */

static void test_mdec_spu_clients(void)
{
    /* @leaf 80096070 source src/game/gpu-2d-hud/gStartMdecOutDma.c: forwards (destination,size) to func_800962D0 unchanged */
    LK_FUNC("func_80096070");
    prog_calls = 0; prog_dst = 0; prog_size = 0;
    gStartMdecOutDma(0x7000, 0x240);
    FCK_EQ(prog_calls, 1, "one dispatch");
    FCK_EQ(prog_dst, 0x7000, "destination forwarded");
    FCK_EQ(prog_size, 0x240, "size forwarded");
    gStartMdecOutDma(-1, 0);
    FCK_EQ(prog_dst, -1, "negative destination forwarded untouched");

    /* @leaf 80096090 source src/game/platform/gMdecInSync.c: mode 0 returns func_8009635C()'s wait result, any other mode returns bit 29 of func_80096484()'s status */
    LK_FUNC("func_80096090");
    wait_calls = 0;
    ret_8009635C = -1;
    FCK_EQ(gMdecInSync(0), -1, "mode 0: wait result forwarded");
    FCK_EQ(wait_calls, 1, "wait issued once");
    ret_8009635C = 0;
    FCK_EQ(gMdecInSync(0), 0, "mode 0: cleared wait");
    ret_80096484 = 0x20000000;
    FCK_EQ(gMdecInSync(1), 1, "mode != 0: bit 29 extracted");
    ret_80096484 = 0x6FFFFFFF;
    FCK_EQ(gMdecInSync(0x33), 1, "other bits do not disturb bit 29");
    ret_80096484 = 0x1FFFFFFF;
    FCK_EQ(gMdecInSync(1), 0, "bit 29 clear: 0");
    FCK_EQ(wait_calls, 2, "nonzero modes bypass the wait");

    /* @leaf 800960CC source src/game/platform/gMdecOutSync.c: mode 0 returns func_800963F0()'s wait result, any other mode returns bit 24 of func_80096484()'s status */
    LK_FUNC("func_800960CC");
    outwait_calls = 0;
    ret_800963F0 = -1;
    FCK_EQ(gMdecOutSync(0), -1, "mode 0: wait result forwarded");
    FCK_EQ(outwait_calls, 1, "wait issued once");
    ret_800963F0 = 0;
    FCK_EQ(gMdecOutSync(0), 0, "mode 0: cleared wait");
    ret_80096484 = 0x01000000;
    FCK_EQ(gMdecOutSync(1), 1, "mode != 0: bit 24 extracted");
    ret_80096484 = 0xA1FFFFFF;
    FCK_EQ(gMdecOutSync(2), 1, "other bits do not disturb bit 24");
    ret_80096484 = 0x00FFFFFF;
    FCK_EQ(gMdecOutSync(7), 0, "bit 24 clear: 0");
    FCK_EQ(outwait_calls, 2, "nonzero modes bypass the wait");

    /* @leaf 80095E90 source src/game/platform/gDecDctReset.c: option 0 additionally runs func_80091480 first; func_80096150 receives the option in every case */
    LK_FUNC("func_80095E90");
    irqreset_calls = 0; mdec_calls = 0; mdec_last_opt = 0;
    gDecDctReset(0);
    FCK_EQ(irqreset_calls, 1, "option 0 resets the IRQ dispatch");
    FCK_EQ(mdec_calls, 1, "MDEC reset run");
    FCK_EQ(mdec_last_opt, 0, "option forwarded");
    gDecDctReset(3);
    FCK_EQ(irqreset_calls, 1, "nonzero option skips the IRQ dispatch");
    FCK_EQ(mdec_calls, 2, "MDEC reset still run");
    FCK_EQ(mdec_last_opt, 3, "nonzero option forwarded");
    gDecDctReset(-1);
    FCK_EQ(irqreset_calls, 1, "negative option also skips the IRQ path");
    FCK_EQ(mdec_last_opt, -1, "negative option forwarded");

    /* @leaf 80097A28 source src/game/platform/gWarmStartSpu.c: calls the init entry with the constant 0 (warm start) */
    LK_FUNC("func_80097A28");
    spuinit_calls = 0; spuinit_arg = 5;
    gWarmStartSpu();
    FCK_EQ(spuinit_calls, 1, "init dispatched once");
    FCK_EQ(spuinit_arg, 0, "warm start passes cold_start = 0");

    /* @leaf 80097760 source src/game/platform/gStartSpuWrite.c: issues gSpuTransfer(2, addr << shift), then gSpuTransfer(0), then gSpuTransfer(3, arg0, arg1) and returns arg1 */
    LK_FUNC("func_80097760");
    tr_calls = 0;
    gSpuTransferAddr = 0x345;
    gSpuAddrShift = 3;
    FCK_EQ(gStartSpuWrite(0x7000, 0x40), 0x40, "arg1 returned untouched");
    FCK_EQ(tr_calls, 3, "three transfers issued");
    FCK_EQ(tr_ops[0], 2, "first op is 2");
    FCK_EQ(tr_a1[0], 0x345 << 3, "op 2 carries addr << shift");
    FCK_EQ(tr_ops[1], 0, "second op is 0");
    FCK_EQ(tr_ops[2], 3, "third op is 3");
    FCK_EQ(tr_a1[2], 0x7000, "op 3 carries arg0");
    FCK_EQ(tr_a2[2], 0x40, "op 3 carries arg1");
    tr_calls = 0;
    gSpuTransferAddr = 0;
    gSpuAddrShift = 0;
    FCK_EQ(gStartSpuWrite(1, 1), 1, "arg1 returned untouched (min case)");
    FCK_EQ(tr_a1[0], 0, "zero address and shift forward zero");
    FCK_EQ(gStartSpuWrite(-1, -5), -5, "negative values pass through");

    /* @leaf 800976DC source src/game/platform/gSpuWriteTransferBlock.c: D_8009EEC0 == 0 issues ops 2/1/3 (with addr << shift), otherwise one func_80097138(arg0,arg1) call; arg1 returned in both paths */
    LK_FUNC("func_800976DC");
    tr_calls = 0; wb_calls = 0;
    gOpOrderFlag = 0;
    gSpuTransferAddr = 0x102;
    gSpuAddrShift = 2;
    FCK_EQ(gSpuWriteTransferBlock(0x6000, 0x20), 0x20, "ordered path returns arg1");
    FCK_EQ(wb_calls, 0, "block write not used in the ordered path");
    FCK_EQ(tr_calls, 3, "three transfers issued");
    FCK_EQ(tr_ops[0], 2, "ordered path op 2 first");
    FCK_EQ(tr_a1[0], 0x102 << 2, "op 2 carries the shifted address");
    FCK_EQ(tr_ops[1], 1, "op 1 second");
    FCK_EQ(tr_ops[2], 3, "op 3 third");
    FCK_EQ(tr_a1[2], 0x6000, "arg0 forwarded on op 3");
    FCK_EQ(tr_a2[2], 0x20, "arg1 forwarded on op 3");
    gOpOrderFlag = 1;
    tr_calls = 0;
    FCK_EQ(gSpuWriteTransferBlock(0x6000, 0x21), 0x21, "block path returns arg1");
    FCK_EQ(tr_calls, 0, "no transfers in the block path");
    FCK_EQ(wb_calls, 1, "one block write dispatched");
    FCK_EQ(wb_a0, 0x6000, "arg0 forwarded to the block write");
    FCK_EQ(wb_a1, 0x21, "arg1 forwarded to the block write");
    gOpOrderFlag = -1;
    wb_calls = 0;
    gSpuWriteTransferBlock(7, 8);
    FCK_EQ(wb_calls, 1, "any nonzero flag selects the block path");

    /* @leaf 80098E28 source src/game/platform/gWriteSpuData.c: caps the size at 0x7EFF0, forwards to func_80097760 and clears D_8009EED8 only when D_8009EEDC is zero; returns the capped size */
    LK_FUNC("func_80098E28");
    tr_calls = 0;
    gSpuClearFlag = 9;
    gSpuTransferCallback = 0x1234;
    FCK_EQ(gWriteSpuData(0x100, 0x1000), 0x1000, "small size passes uncapped");
    FCK_EQ(gSpuClearFlag, 9, "callback set: clear flag untouched");
    FCK_EQ(tr_calls, 3, "one write issues the 3-op transfer sequence");
    gSpuTransferCallback = 0;
    FCK_EQ(gWriteSpuData(0x100, 0x7EFF0), 0x7EFF0, "boundary cap value accepted");
    FCK_EQ(gSpuClearFlag, 0, "callback zero: flag cleared after the write");
    FCK_EQ(gWriteSpuData(0x100, 0x7EFF1), 0x7EFF0, "one over the cap is capped");
    FCK_EQ(gWriteSpuData(0x100, 0x7EFEF), 0x7EFEF, "one under the cap passes as-is");
    FCK_EQ(gSpuClearFlag, 0, "flag stays clear on repeat writes");
    FCK_EQ(tr_calls, 12, "each write issues its 3-op sequence");
    FCK_EQ(gWriteSpuData(0x100, 0xFFFFFFFF), 0x7EFF0, "u32 wrap caps to 0x7EFF0");
    FCK_EQ(gSpuTransferCallback, 0, "the write path never touches the callback slot");

    /* @leaf 80098E88 source src/game/platform/gWriteSpuRam.c: same contract as func_80098E28 through func_800976DC */
    LK_FUNC("func_80098E88");
    wb_calls = 0;
    gOpOrderFlag = 1;
    gSpuClearFlag = 3;
    gSpuTransferCallback = 0xFFFF;
    FCK_EQ(gWriteSpuRam(4, 0x200), 0x200, "small size passes uncapped");
    FCK_EQ(gSpuClearFlag, 3, "callback set: clear flag untouched");
    gSpuTransferCallback = 0;
    FCK_EQ(gWriteSpuRam(4, 0x7EFF0), 0x7EFF0, "boundary cap value accepted");
    FCK_EQ(gSpuClearFlag, 0, "callback clear: flag cleared");
    FCK_EQ(gWriteSpuRam(4, 0x7EFF0), 0x7EFF0, "exact cap boundary passes uncapped");
    FCK_EQ(gWriteSpuRam(4, 0x80000), 0x7EFF0, "oversize capped");
    FCK_EQ(gWriteSpuRam(4, 0x7EFF1), 0x7EFF0, "one over the boundary is capped");
    FCK_EQ(wb_calls, 5, "each write dispatches its block");
}

static void test_env_copies(void)
{
    static u8 buf[0x90];
    u8 *r;
    s32 i;

    /* @leaf 80093488 source src/game/gpu-2d-hud/gCopyDrawEnv.c: copies the 0x5C-byte cached DRAWENV at D_8009DE00 into the caller buffer through func_8008CD10 and returns the buffer */
    LK_FUNC("func_80093488");
    for (i = 0; i < 0x5C; i++) { gDrawEnvCache[i] = (u8)(0x40 + i); buf[i] = 0; }
    for (i = 0x5C; i < 0x90; i++) buf[i] = 0xC7;   /* sentinel beyond the copy */
    r = gCopyDrawEnv(buf);
    FCK_EQ((u32)(unsigned long)r, (u32)(unsigned long)buf, "destination returned");
    for (i = 0; i < 0x5C; i++) {
        if (buf[i] != (u8)(0x40 + i)) { FCK_EQ(buf[i], (u8)(0x40 + i), "DRAWENV byte copied"); break; }
    }
    FCK_EQ(buf[0], 0x40, "first byte copied");
    FCK_EQ(buf[0x5B], 0x40 + 0x5B, "last byte copied");
    FCK_EQ(buf[0x5C], 0xC7, "byte just past the copy stays untouched");

    /* @leaf 800938F0 source src/game/gpu-2d-hud/gCopyDispEnv.c: copies the 0x14-byte cached DISPENV at D_8009DE5C through func_8008CD10 and returns the buffer */
    LK_FUNC("func_800938F0");
    for (i = 0; i < 0x28; i++) buf[i] = 0;
    for (i = 0; i < 0x14; i++) gDispEnvCache[i] = (u8)(0x80 + i);
    for (i = 0x14; i < 0x28; i++) buf[i] = 0xC7;
    r = gCopyDispEnv(buf);
    FCK_EQ((u32)(unsigned long)r, (u32)(unsigned long)buf, "destination returned");
    FCK_EQ(buf[0], 0x80, "first byte copied");
    FCK_EQ(buf[0x13], 0x80 + 0x13, "last byte copied");
    FCK_EQ(buf[0x14], 0xC7, "byte just past the 0x14-byte copy is untouched");
}

/* ========================================================================
 * main
 * ======================================================================== */

int main(void)
{
    fcm_bios_reset();
    test_cd_readwrappers();
    test_cdinit();
    test_cd_events();
    test_cd_clients();
    test_gpu_stateclients();
    test_gpu_debugclients();
    test_mdec_spu_clients();
    test_env_copies();
    return lk_done("leaf_plat");
}
