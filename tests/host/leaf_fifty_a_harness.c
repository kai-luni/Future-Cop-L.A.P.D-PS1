/* Source characterizations of 25 unchanged src/game/<address>.c units.
 * Layouts and signatures are verified from the cited units. All state is
 * synthetic RAM fixtures; no retail data, BIOS, or hardware access.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Real dependencies already covered by other suites. The generated runner
 * links only marked units, so compile these here without claiming coverage.
 */
#include "../../src/game/8001BC44.c"
#include "../../src/game/8001C148.c"
#include "../../src/game/800607B8.c"
#include "../../src/game/80093F58.c"
#include "../../src/game/800940A8.c"
#include "../../src/game/800940C4.c"

/* ---- 1 & 2: GPU immediate submit & DrawSync (80093224, 80092C8C) ---- */
struct GpuCbTable {
    u8 unk00[0x14];
    void (*unk14)(u8 *, s32);
    u8 unk18[0x24];
    void (*unk3C)(s32);
};
struct GpuCbTable *gVramWriteCallback_s __asm__("D_8009DDE8");
u8 gGpuDebugLevel_s __asm__("D_8009DDF2");
void (*gLogPrintf_s)(void *fmt, s32 arg) __asm__("D_8009DDEC");
u8 gDrawSyncTraceFormat_s[16] __asm__("D_80012A44");
void gDrawPrimImmediate(u8 *arg0) __asm__("func_80093224");
void gDrawSync(s32 arg0) __asm__("func_80092C8C");

static s32 cb_seq, cb_3c_order, cb_14_order, cb_log_order;
static s32 cb_3c_arg, cb_14_len, cb_log_arg;
static u8 *cb_14_ptr;
static void *cb_log_fmt;

static void rec_unk3C(s32 a)
{
    cb_3c_order = ++cb_seq;
    cb_3c_arg = a;
}
static void rec_unk14(u8 *p, s32 n)
{
    cb_14_order = ++cb_seq;
    cb_14_ptr = p;
    cb_14_len = n;
}
static void rec_log(void *fmt, s32 a)
{
    cb_log_order = ++cb_seq;
    cb_log_fmt = fmt;
    cb_log_arg = a;
}

static void test_gpu_callbacks(void)
{
    struct GpuCbTable tbl;
    u8 pkt[16];

    lk_fill(&tbl, 0, sizeof(tbl));
    tbl.unk14 = rec_unk14;
    tbl.unk3C = rec_unk3C;
    gVramWriteCallback_s = &tbl;
    gLogPrintf_s = rec_log;

    /* @leaf 80093224 source src/game/80093224.c: flush via unk3C(0) then submit payload+4 with length byte */
    LK_FUNC("func_80093224");
    lk_fill(pkt, 0x5A, sizeof(pkt));
    pkt[3] = 7;
    cb_seq = cb_3c_order = cb_14_order = 0;
    cb_3c_arg = -1; cb_14_ptr = 0; cb_14_len = -1;
    gDrawPrimImmediate(pkt);
    FCK_EQ(cb_3c_order, 1, "unk3C runs first");
    FCK_EQ(cb_3c_arg, 0, "unk3C receives 0");
    FCK_EQ(cb_14_order, 2, "unk14 runs second");
    FCK_CHECK(cb_14_ptr == pkt + 4, "unk14 receives packet body at +4");
    FCK_EQ(cb_14_len, 7, "unk14 receives byte 3 length");
    FCK_CHECK(lk_all(pkt, 0, 3, 0x5A) && pkt[3] == 7 && lk_all(pkt, 4, 16, 0x5A), "packet buffer untouched");

    /* @leaf 80092C8C source src/game/80092C8C.c: conditional trace at debug>=2 then unk3C(arg0) */
    LK_FUNC("func_80092C8C");
    gGpuDebugLevel_s = 1;
    cb_seq = cb_3c_order = cb_log_order = 0;
    cb_3c_arg = -1; cb_log_fmt = 0; cb_log_arg = -1;
    gDrawSync(42);
    FCK_EQ(cb_log_order, 0, "debug level 1 skips log");
    FCK_EQ(cb_3c_order, 1, "unk3C called directly");
    FCK_EQ(cb_3c_arg, 42, "arg forwarded to unk3C");

    gGpuDebugLevel_s = 2;
    cb_seq = cb_3c_order = cb_log_order = 0;
    cb_3c_arg = -1; cb_log_fmt = 0; cb_log_arg = -1;
    gDrawSync(-5);
    FCK_EQ(cb_log_order, 1, "debug level 2 logs first");
    FCK_CHECK(cb_log_fmt == gDrawSyncTraceFormat_s, "trace format symbol passed");
    FCK_EQ(cb_log_arg, -5, "arg forwarded to log");
    FCK_EQ(cb_3c_order, 2, "unk3C runs after log");
    FCK_EQ(cb_3c_arg, -5, "arg forwarded to unk3C after log");
}

/* ---- 3, 4 & 5: GPU DMA & GP0/GP1 port helpers (800949A0, 80094960, 80095350) ---- */
u32 *gGpuGp0PortPtr_s __asm__("D_8009DEC4");
u32 *gGpuGp1PortPtr_s __asm__("D_8009DEC8");
u32 *gDma2MemAddrPtr_s __asm__("D_8009DECC");
u32 *gDma2BlockCtrlPtr_s __asm__("D_8009DED0");
u32 *gDma2ChannelCtrlPtr_s __asm__("D_8009DED4");
void gStartGpuLinkedListDma(u32 value) __asm__("func_800949A0");
int gSendGpuWordList(unsigned int *data, unsigned int count) __asm__("func_80094960");
s32 gProbeGpuVersion(u32 flags) __asm__("func_80095350");

static void test_gpu_ports(void)
{
    u32 regs[6];
    u32 words[3];

    gGpuGp0PortPtr_s = &regs[1];
    gGpuGp1PortPtr_s = &regs[2];
    gDma2MemAddrPtr_s = &regs[3];
    gDma2BlockCtrlPtr_s = &regs[4];
    gDma2ChannelCtrlPtr_s = &regs[5];

    /* @leaf 800949A0 source src/game/800949A0.c: four ordered DMA2/GP1 register writes */
    LK_FUNC("func_800949A0");
    regs[0] = 0xDEADBEEFU; regs[1] = 0x11111111U; regs[2] = 0x22222222U;
    regs[3] = 0x33333333U; regs[4] = 0x44444444U; regs[5] = 0x55555555U;
    gStartGpuLinkedListDma(0x00123450U);
    FCK_EQ(regs[2], 0x04000002U, "GP1 DMA direction set");
    FCK_EQ(regs[3], 0x00123450U, "DMA2 MADR receives value");
    FCK_EQ(regs[4], 0U, "DMA2 BCR cleared");
    FCK_EQ(regs[5], 0x01000401U, "DMA2 CHCR linked-list start");
    FCK_EQ(regs[0], 0xDEADBEEFU, "guard word untouched");
    FCK_EQ(regs[1], 0x11111111U, "GP0 word untouched");

    /* @leaf 80094960 source src/game/80094960.c: reset GP1 DMA then stream count words to GP0 */
    LK_FUNC("func_80094960");
    words[0] = 0x11223344U; words[1] = 0x55667788U; words[2] = 0x99AABBCCU;
    regs[1] = 0xCAFEBABEU; regs[2] = 0xFFFFFFFFU;
    FCK_EQ(gSendGpuWordList(words, 0), 0, "zero-count returns 0");
    FCK_EQ(regs[2], 0x04000000U, "GP1 cleared even when count==0");
    FCK_EQ(regs[1], 0xCAFEBABEU, "GP0 untouched when count==0");
    FCK_EQ(gSendGpuWordList(words, 1), 0, "single word returns 0");
    FCK_EQ(regs[1], 0x11223344U, "single word written to GP0");
    FCK_EQ(gSendGpuWordList(words, 3), 0, "three words returns 0");
    FCK_EQ(regs[1], 0x99AABBCCU, "last streamed word lands in GP0");
    FCK_EQ(regs[2], 0x04000000U, "GP1 written on stream");

    /* @leaf 80095350 source src/game/80095350.c: probe GPU info=7 and select version 0/1/2 */
    LK_FUNC("func_80095350");
    regs[1] = 0xAA000001U; regs[2] = 0;
    FCK_EQ(gProbeGpuVersion(0xFF), 0, "non-2 GPU info returns 0");
    FCK_EQ(regs[2], 0x10000007U, "GP1 queried with 0x10000007");
    FCK_EQ(regs[1], 0xE1001007U, "GP0 receives fallback E1 command");
    regs[1] = 0xBB000002U; regs[2] = 0;
    FCK_EQ(gProbeGpuVersion(7), 1, "version 2 without flag bit 3 returns 1");
    FCK_EQ(regs[2], 0x10000007U, "GP1 stays at probe command");
    FCK_EQ(regs[1], 0xBB000002U, "GP0 untouched on version 1 path");
    regs[1] = 0x00000002U; regs[2] = 0;
    FCK_EQ(gProbeGpuVersion(8), 2, "version 2 with flag bit 3 returns 2");
    FCK_EQ(regs[2], 0x09000001U, "GP1 enables 2MB mode");
}

/* ---- 6, 7 & 8: SIO0 clear, SPU delay mode, Pad VSync hook (8008DA14, 800978E8, 8008A3D4) ---- */
struct SioClearState {
    u8 pad[10];
    volatile s16 value;
};
struct SioClearState *gSio0CtrlClearPtr_s __asm__("D_8009C804");
int gClearSio0CtrlAndWait(void) __asm__("func_8008DA14");

unsigned int *gSpuDelayCtrlPtr_s __asm__("D_8009EEB4");
void gSetSpuDelayTimingMode(int arg0) __asm__("func_800978E8");

struct IntcFlags {
    unsigned int first;
    unsigned int second;
};
struct IntcFlags *gInterruptStatusPtr_s __asm__("D_8009C788");
void (*gPadIrqCallback_s)(void) __asm__("D_8009C750");
int gPadVsyncDispatchHook(void) __asm__("func_8008A3D4");

static int pad_irq_calls;
static void rec_pad_irq(void) { pad_irq_calls++; }

static void test_platform_hooks(void)
{
    struct SioClearState sio;
    unsigned int spu_ctrl;
    struct IntcFlags intc;

    /* @leaf 8008DA14 source src/game/8008DA14.c: zero volatile halfword at +0x0A, preserve pad, return 0 */
    LK_FUNC("func_8008DA14");
    lk_fill(&sio, 0xA5, sizeof(sio));
    gSio0CtrlClearPtr_s = &sio;
    FCK_EQ(gClearSio0CtrlAndWait(), 0, "returns 0");
    FCK_EQ(sio.value, 0, "halfword at +0x0A cleared");
    FCK_CHECK(lk_all(&sio, 0, 10, 0xA5), "preceding bytes untouched");

    /* @leaf 800978E8 source src/game/800978E8.c: replace bits 16..18 with 3 (nonzero) or 5 (zero) */
    LK_FUNC("func_800978E8");
    gSpuDelayCtrlPtr_s = &spu_ctrl;
    spu_ctrl = 0xA5A75A5AU;
    gSetSpuDelayTimingMode(1);
    FCK_EQ(spu_ctrl, 0xA5A35A5AU, "nonzero selects 0x30000 preserving other bits");
    gSetSpuDelayTimingMode(0);
    FCK_EQ(spu_ctrl, 0xA5A55A5AU, "zero selects 0x50000 preserving other bits");
    spu_ctrl = 0x00000000U;
    gSetSpuDelayTimingMode(-3);
    FCK_EQ(spu_ctrl, 0x00030000U, "negative nonzero selects 0x30000");

    /* @leaf 8008A3D4 source src/game/8008A3D4.c: check both INTC bit 0 words before invoking callback */
    LK_FUNC("func_8008A3D4");
    gInterruptStatusPtr_s = &intc;
    gPadIrqCallback_s = rec_pad_irq;
    pad_irq_calls = 0;
    intc.first = 1; intc.second = 2;
    FCK_EQ(gPadVsyncDispatchHook(), 0, "missing second bit 0 returns 0");
    FCK_EQ(pad_irq_calls, 0, "callback skipped when second bit 0 clear");
    intc.first = 2; intc.second = 1;
    FCK_EQ(gPadVsyncDispatchHook(), 0, "missing first bit 0 returns 0");
    FCK_EQ(pad_irq_calls, 0, "callback skipped when first bit 0 clear");
    intc.first = 3; intc.second = 5;
    FCK_EQ(gPadVsyncDispatchHook(), 1, "both bits set returns 1");
    FCK_EQ(pad_irq_calls, 1, "callback invoked once");
    gPadIrqCallback_s = 0;
    FCK_EQ(gPadVsyncDispatchHook(), 1, "null callback still returns 1");
    FCK_EQ(pad_irq_calls, 1, "null callback not called");
}

/* ---- 9, 10 & 11: CD-ROM audio mix regs (8008F650, 8008E578, 8008F7AC) ---- */
u8 *gCdromIndexStatusPtr_s __asm__("D_8009CB6C");
u8 *gCdromCommandPtr_s __asm__("D_8009CB70");
u8 *gCdromParameterPtr_s __asm__("D_8009CB74");
u8 *gCdromRequestStatusPtr_s __asm__("D_8009CB78");
struct SpuMixRegs {
    u8 pad[0x180];
    u16 a, b;
    u8 pad2[0x26];
    u16 c;
    u8 pad3[4];
    u16 d, e;
    u8 pad4[4];
    u16 f, g;
};
struct SpuMixRegs *gSpuRegsAltPtr_s __asm__("D_8009CB80");
s32 gWriteCdAudioMixRegs(u8 *arg0) __asm__("func_8008F650");
s32 gSetCdVolume(u8 *arg0) __asm__("func_8008E578");
s32 gInitCdAudioMix(void) __asm__("func_8008F7AC");

static void test_cd_audio_mix(void)
{
    u8 cd_regs[8];
    u8 mix[4];
    struct SpuMixRegs spu;

    gCdromIndexStatusPtr_s = &cd_regs[1];
    gCdromCommandPtr_s = &cd_regs[2];
    gCdromParameterPtr_s = &cd_regs[3];
    gCdromRequestStatusPtr_s = &cd_regs[4];

    /* @leaf 8008F650 source src/game/8008F650.c: write bank 2/3 CD audio mix registers and apply 0x20 */
    LK_FUNC("func_8008F650");
    lk_fill(cd_regs, 0xA5, sizeof(cd_regs));
    mix[0] = 0x11; mix[1] = 0x22; mix[2] = 0x33; mix[3] = 0x44;
    FCK_EQ(gWriteCdAudioMixRegs(mix), 0, "returns 0");
    FCK_EQ(cd_regs[1], 3, "final index register is 3");
    FCK_EQ(cd_regs[2], 0x33, "command reg receives arg0[2]");
    FCK_EQ(cd_regs[3], 0x44, "parameter reg receives arg0[3]");
    FCK_EQ(cd_regs[4], 0x20, "request reg receives apply bit 0x20");
    FCK_CHECK(cd_regs[0] == 0xA5 && lk_all(cd_regs, 5, 8, 0xA5), "adjacent bytes untouched");

    /* @leaf 8008E578 source src/game/8008E578.c: forward to gWriteCdAudioMixRegs and return 1 */
    LK_FUNC("func_8008E578");
    lk_fill(cd_regs, 0x5A, sizeof(cd_regs));
    mix[0] = 0x70; mix[1] = 0x71; mix[2] = 0x72; mix[3] = 0x73;
    FCK_EQ(gSetCdVolume(mix), 1, "returns 1");
    FCK_EQ(cd_regs[1], 3, "index set to 3");
    FCK_EQ(cd_regs[2], 0x72, "cmd set to mix[2]");
    FCK_EQ(cd_regs[3], 0x73, "param set to mix[3]");
    FCK_EQ(cd_regs[4], 0x20, "apply written");

    /* @leaf 8008F7AC source src/game/8008F7AC.c: conditional main volume init, CD vol/ctrl + CD mix apply */
    LK_FUNC("func_8008F7AC");
    gSpuRegsAltPtr_s = &spu;
    lk_fill(&spu, 0xA5, sizeof(spu));
    lk_fill(cd_regs, 0xA5, sizeof(cd_regs));
    spu.f = 0; spu.g = 0;
    FCK_EQ(gInitCdAudioMix(), 0, "returns 0 when f==0 && g==0");
    FCK_EQ(spu.a, 0x3FFF, "main vol L initialized when f==0 && g==0");
    FCK_EQ(spu.b, 0x3FFF, "main vol R initialized when f==0 && g==0");
    FCK_EQ(spu.c, 0xC001, "SPU control set to 0xC001");
    FCK_EQ(spu.d, 0x3FFF, "CD vol L set to 0x3FFF");
    FCK_EQ(spu.e, 0x3FFF, "CD vol R set to 0x3FFF");
    FCK_EQ(cd_regs[1], 3, "CD index 3");
    FCK_EQ(cd_regs[2], 0x80, "CD cmd 0x80");
    FCK_EQ(cd_regs[3], 0, "CD param 0");
    FCK_EQ(cd_regs[4], 0x20, "CD apply 0x20");
    spu.a = 0x1234; spu.b = 0x5678; spu.f = 1; spu.g = 0;
    gInitCdAudioMix();
    FCK_CHECK(spu.a == 0x1234 && spu.b == 0x5678, "nonzero f preserves main vol");
    spu.f = 0; spu.g = 2;
    gInitCdAudioMix();
    FCK_CHECK(spu.a == 0x1234 && spu.b == 0x5678, "nonzero g preserves main vol");
}

/* ---- 12, 13 & 14: Pad port state, query & timer (80089E8C, 80089F4C, 8008C77C) ---- */
struct PadPortState {
    u8 pad0[0x10];
    struct PadPortState *next;
    u8 pad14[0x1C];
    u8 *data;
    u32 flags;
    u8 busy;
    u8 pad39[0x10];
    u8 mode;
};
struct PadInfoData {
    u16 *table;
    u8 pad04[0x48];
    u32 field4c;
    u8 pad50[0x93];
    u8 count;
    u8 fieldE4;
    u8 padE5;
    u16 fieldE6;
    u8 fieldE8;
};
struct PadTimerObj {
    u8 pad0[0x30];
    u8 *out;
    u8 pad34[8];
    u8 *cmd;
    u8 pad40[6];
    u8 state;
    u8 pad47[2];
    u8 flag;
    u8 count;
    u8 pad4b;
    s32 timer;
    u8 pad50[0x98];
    u8 active;
};
void *(*gPadLookupPortFn_s)(int) __asm__("D_8009C73C");
void (*gPadResetPortHook_s)(struct PadTimerObj *) __asm__("D_8009C72C");
int gGetPadPortState(void) __asm__("func_80089E8C");
int gQueryPadInfo(int arg0, int kind, int index) __asm__("func_80089F4C");
void gUpdatePadTimer(struct PadTimerObj *self) __asm__("func_8008C77C");

static struct PadPortState pad_st_a, pad_st_b;
static struct PadInfoData pad_info_fix;
static int pad_lookup_last_arg;
static int pad_reset_calls;
static struct PadTimerObj *pad_reset_last;

static void *lookup_port_state(int unused) { (void)unused; return &pad_st_a; }
static void *lookup_pad_info(int a) { pad_lookup_last_arg = a; return &pad_info_fix; }
static void rec_pad_reset(struct PadTimerObj *o) { pad_reset_calls++; pad_reset_last = o; }

static void test_pad_helpers(void)
{
    u8 data_buf[4];
    u16 tbl[3];
    struct PadTimerObj obj;
    u8 out_buf[4], cmd_buf[4];

    /* @leaf 80089E8C source src/game/80089E8C.c: remap modes 2/3->1 and 6->4 when busy/flagged/data[0] */
    LK_FUNC("func_80089E8C");
    gPadLookupPortFn_s = lookup_port_state;
    lk_fill(&pad_st_a, 0, sizeof(pad_st_a));
    data_buf[0] = 0;
    pad_st_a.next = &pad_st_a;
    pad_st_a.data = data_buf;
    pad_st_a.flags = 0x0000FFFFU;
    pad_st_a.busy = 1;
    pad_st_a.mode = 2;
    FCK_EQ(gGetPadPortState(), 2, "self-linked busy + low flags keeps mode 2");
    pad_st_a.flags = 0x00010000U;
    FCK_EQ(gGetPadPortState(), 1, "high flag remaps mode 2 to 1");
    pad_st_a.mode = 3;
    FCK_EQ(gGetPadPortState(), 1, "high flag remaps mode 3 to 1");
    pad_st_a.mode = 6;
    FCK_EQ(gGetPadPortState(), 4, "high flag remaps mode 6 to 4");
    pad_st_a.mode = 5;
    FCK_EQ(gGetPadPortState(), 5, "high flag leaves mode 5 unchanged");
    pad_st_a.flags = 0; pad_st_a.mode = 6; pad_st_a.next = &pad_st_b; pad_st_a.busy = 1;
    FCK_EQ(gGetPadPortState(), 4, "non-self next with busy remaps mode 6 to 4");
    pad_st_a.busy = 0;
    FCK_EQ(gGetPadPortState(), 6, "non-self next without busy keeps mode 6");
    data_buf[0] = 1;
    FCK_EQ(gGetPadPortState(), 4, "nonzero data[0] remaps mode 6 to 4");

    /* @leaf 80089F4C source src/game/80089F4C.c: dispatch pad info kinds 1/2/3/4/100 with bounds */
    LK_FUNC("func_80089F4C");
    gPadLookupPortFn_s = lookup_pad_info;
    lk_fill(&pad_info_fix, 0, sizeof(pad_info_fix));
    tbl[0] = 0x1111; tbl[1] = 0x2222; tbl[2] = 0x3333;
    pad_info_fix.table = tbl;
    pad_info_fix.field4c = 0x12345678U;
    pad_info_fix.count = 3;
    pad_info_fix.fieldE4 = 0x44;
    pad_info_fix.fieldE6 = 0x6677;
    pad_info_fix.fieldE8 = 0x88;
    pad_lookup_last_arg = -1;
    FCK_EQ(gQueryPadInfo(7, 1, 0), 0x88, "kind 1 returns fieldE8");
    FCK_EQ(pad_lookup_last_arg, 7, "port arg forwarded to lookup");
    FCK_EQ(gQueryPadInfo(1, 2, 0), 0x6677, "kind 2 returns fieldE6");
    FCK_EQ(gQueryPadInfo(1, 3, 0), 0x44, "kind 3 returns fieldE4");
    FCK_EQ(gQueryPadInfo(1, 4, -1), 3, "kind 4 negative index returns count");
    FCK_EQ(gQueryPadInfo(1, 4, 0), 0x1111, "kind 4 index 0");
    FCK_EQ(gQueryPadInfo(1, 4, 2), 0x3333, "kind 4 last index");
    FCK_EQ(gQueryPadInfo(1, 4, 3), 0, "kind 4 out-of-range returns 0");
    FCK_EQ(gQueryPadInfo(1, 100, 0), 0x12345678, "kind 100 returns field4c");
    FCK_EQ(gQueryPadInfo(1, 99, 0), 0, "unknown kind returns 0");

    /* @leaf 8008C77C source src/game/8008C77C.c: pad state timer, retry thresholds, reset hook and disconnect clear */
    LK_FUNC("func_8008C77C");
    gPadResetPortHook_s = rec_pad_reset;
    lk_fill(&obj, 0, sizeof(obj));
    obj.out = out_buf; obj.cmd = cmd_buf;
    out_buf[0] = 0x11; out_buf[1] = 0x22; cmd_buf[0] = 0x42;
    obj.state = 0; obj.timer = 10; obj.active = 1;
    pad_reset_calls = 0; pad_reset_last = 0;
    gUpdatePadTimer(&obj);
    FCK_CHECK(obj.timer == 11 && out_buf[0] == 255 && out_buf[1] == 0 && obj.active == 0,
              "state 0 non-0xF3 clears output and active");
    FCK_EQ(pad_reset_calls, 0, "state 0 does not call reset hook");

    out_buf[0] = 0x11; out_buf[1] = 0x22; cmd_buf[0] = 0xF3; obj.active = 1;
    gUpdatePadTimer(&obj);
    FCK_CHECK(obj.timer == 12 && out_buf[0] == 0x11 && out_buf[1] == 0x22 && obj.active == 1,
              "0xF3 command preserves output and active");

    obj.state = 1; obj.count = 1; obj.flag = 0; cmd_buf[0] = 0x42;
    gUpdatePadTimer(&obj);
    FCK_CHECK(obj.count == 2 && obj.state == 1 && obj.active == 1, "state 1 count<2 increments and returns");
    gUpdatePadTimer(&obj);
    FCK_CHECK(obj.flag == 2 && obj.state == 255 && obj.active == 1, "state 1 count==2 sets flag=2 state=255");

    obj.state = 2; obj.count = 3; obj.flag = 1;
    gUpdatePadTimer(&obj);
    FCK_CHECK(obj.count == 4 && pad_reset_calls == 0 && obj.active == 1, "state>1 count<4 increments and returns");
    gUpdatePadTimer(&obj);
    FCK_CHECK(pad_reset_calls == 1 && pad_reset_last == &obj && out_buf[0] == 255 && obj.active == 0,
              "state>1 count==4 invokes reset hook and clears non-0xF3");
}

/* ---- 15: Entity msg 0x0D dispatch (8004EC74) ---- */
struct MsgEntity {
    struct MsgEntity *next;
    s32 (*update)(s32 message, struct MsgEntity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 unk0C[0x7C];
    s32 unk88;
    u8 unk8C[4];
    s16 unk90;
};
struct MsgEntityTable {
    s32 count;
    s32 unk04;
    struct MsgEntity *items[4];
};
struct MsgEntityTable gCommandListHead_s __asm__("D_800A8F90");
void gDispatchEntityMsg0D(void) __asm__("func_8004EC74");

static int ent_calls;
static s32 ent_last_msg, ent_last_a2, ent_last_a3;
static struct MsgEntity *ent_last_ptr;
static s32 rec_ent_update(s32 msg, struct MsgEntity *e, s32 a2, s32 a3)
{
    ent_calls++;
    ent_last_msg = msg;
    ent_last_ptr = e;
    ent_last_a2 = a2;
    ent_last_a3 = a3;
    return 0;
}

static void test_entity_msg(void)
{
    struct MsgEntity e[3];

    /* @leaf 8004EC74 source src/game/8004EC74.c: dispatch msg 0xD only for id>0 && unk88!=0 and reset fields */
    LK_FUNC("func_8004EC74");
    lk_fill(e, 0, sizeof(e));
    e[0].update = rec_ent_update; e[0].id = 0; e[0].unk88 = 1; e[0].unk90 = 11;
    e[1].update = rec_ent_update; e[1].id = 5; e[1].unk88 = 0; e[1].unk90 = 22;
    e[2].update = rec_ent_update; e[2].id = 7; e[2].unk88 = 3; e[2].unk90 = 33;
    gCommandListHead_s.count = 3;
    gCommandListHead_s.items[0] = &e[0];
    gCommandListHead_s.items[1] = &e[1];
    gCommandListHead_s.items[2] = &e[2];
    ent_calls = 0; ent_last_msg = -1; ent_last_ptr = 0; ent_last_a2 = -1; ent_last_a3 = -1;
    gDispatchEntityMsg0D();
    FCK_EQ(ent_calls, 1, "only eligible entity invoked");
    FCK_CHECK(ent_last_msg == 0xD && ent_last_ptr == &e[2] && ent_last_a2 == 0 && ent_last_a3 == 0,
              "message 0xD with zero args sent to entity 2");
    FCK_CHECK(e[2].unk88 == 0 && e[2].unk90 == 0x7F00, "dispatched entity unk88 cleared and unk90 set");
    FCK_CHECK(e[0].unk88 == 1 && e[0].unk90 == 11 && e[1].unk90 == 22, "ineligible entities untouched");
    gCommandListHead_s.count = 0;
    gDispatchEntityMsg0D();
    FCK_EQ(ent_calls, 1, "count 0 dispatches nothing");
}

/* ---- 16, 17, 18 & 25: Prim pool & screen fade / translucent tile (80082FA4, 80026E9C, 80026F68, 8001C214) ---- */
struct FlatQuadPoly {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};
struct FlatQuadPoly *gPrimPoolNext_s __asm__("D_8009F9F0");
struct FlatQuadPoly *gPrimPoolEnd_s __asm__("D_8009FA28");
void gEmitFlatQuadRectMode(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b, s32 code) __asm__("func_80082FA4");

u8 *gPacketCursor_s __asm__("D_8009FA90");
u8 *gPacketLimit_s __asm__("D_8009F9C0");
void gEmitScreenFadeBar(s32 r, s32 g, s32 b, s32 w) __asm__("func_80026E9C");

s16 gTeamFlag_s[2] __asm__("D_8009F65C");
s16 gPlayerFadeTimers_s[2] __asm__("D_8009FA44");
s32 gFrameDelta_s __asm__("D_8009F9FC");
s32 gScreenState_s __asm__("D_8009FA00");
void gTickPlayerTransitionFade(s32 arg0, s32 arg1) __asm__("func_80026F68");

void gEmitTranslucentTileRect(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");

struct FadePage { u32 addr : 24; u32 len : 8; u32 code; };
struct FadeTile { u32 addr : 24; u32 len : 8; u8 r, g, b, code; s16 x, y, w, h; };

static void test_prim_emitters(void)
{
    u8 quad_buf[0x68];
    u8 pkt_buf[0x54];
    struct FlatQuadPoly *q;
    struct FadePage *pg;
    struct FadeTile *tl;

    /* @leaf 80082FA4 source src/game/80082FA4.c: emit 5-word flat quad and advance pool only below end */
    LK_FUNC("func_80082FA4");
    lk_fill(quad_buf, 0xA5, sizeof(quad_buf));
    gPrimPoolNext_s = (struct FlatQuadPoly *)quad_buf;
    gPrimPoolEnd_s = (struct FlatQuadPoly *)(quad_buf + 0x34);
    gEmitFlatQuadRectMode(10, 20, 30, 40, 0x123456U, 0x11, 0x22, 0x33, 2);
    FCK_CHECK((u8 *)gPrimPoolNext_s == quad_buf + 0x34, "cursor advances by 0x34 below limit");
    q = (struct FlatQuadPoly *)quad_buf;
    FCK_CHECK(q->len == 5 && q->code == 0x2A && q->addr == 0x123456U, "header len/code/link");
    FCK_CHECK(q->r == 0x11 && q->g == 0x22 && q->b == 0x33, "rgb bytes");
    FCK_CHECK(q->xy[0] == 10 && q->xy[1] == 20 && q->xy[2] == 40 && q->xy[3] == 20 &&
              q->xy[4] == 10 && q->xy[5] == 60 && q->xy[6] == 40 && q->xy[7] == 60, "quad corners");
    gEmitFlatQuadRectMode(-5, 3, 8, 9, 0x000044U, 1, 2, 3, 0);
    FCK_CHECK((u8 *)gPrimPoolNext_s == quad_buf + 0x34, "cursor stops at end");
    q = (struct FlatQuadPoly *)(quad_buf + 0x34);
    FCK_CHECK(q->code == 0x28 && q->xy[0] == -5 && q->xy[2] == 3 && q->xy[5] == 12, "second quad at end slot");

    /* @leaf 80026E9C source src/game/80026E9C.c: emit TPage 0xE10006A0 and 0x62 fade tile (h=0xF0) */
    LK_FUNC("func_80026E9C");
    lk_fill(pkt_buf, 0xA5, sizeof(pkt_buf));
    gPacketCursor_s = pkt_buf;
    gPacketLimit_s = pkt_buf + 0x38;
    gEmitScreenFadeBar(0x10, 0x20, 0x30, 0x140);
    FCK_CHECK(gPacketCursor_s == pkt_buf + 0x38, "cursor advances by two 0x1C slots");
    pg = (struct FadePage *)pkt_buf;
    tl = (struct FadeTile *)(pkt_buf + 0x1C);
    FCK_CHECK(pg->len == 1 && pg->addr == 0 && pg->code == 0xE10006A0U, "tpage packet");
    FCK_CHECK(tl->len == 3 && tl->addr == 0 && tl->code == 0x62 &&
              tl->r == 0x10 && tl->g == 0x20 && tl->b == 0x30 &&
              tl->x == 0 && tl->y == 0 && tl->w == 0x140 && tl->h == 0xF0, "fade tile packet");
    gPacketCursor_s = pkt_buf; gPacketLimit_s = pkt_buf;
    gEmitScreenFadeBar(1, 2, 3, 0xA0);
    FCK_CHECK(gPacketCursor_s == pkt_buf, "at limit cursor does not advance");
    tl = (struct FadeTile *)pkt_buf;
    FCK_CHECK(tl->len == 3 && tl->w == 0xA0, "tile overwrites slot 0 at limit");

    /* @leaf 80026F68 source src/game/80026F68.c: team retire timer vs fade countdown and clamped fade bar */
    LK_FUNC("func_80026F68");
    gTeamFlag_s[0] = 0x380; gTeamFlag_s[1] = 0;
    gPlayerFadeTimers_s[0] = 50; gPlayerFadeTimers_s[1] = 0;
    gFrameDelta_s = 4; gScreenState_s = 2;
    gTickPlayerTransitionFade(0, 0);
    FCK_CHECK(gTeamFlag_s[0] == 0x384 && gScreenState_s == 2, "team flag increments below 0x384");
    gTickPlayerTransitionFade(0, 0);
    FCK_EQ(gScreenState_s, 6, "team flag >= 0x384 sets screen state 6");

    gTeamFlag_s[1] = 0; gPlayerFadeTimers_s[1] = 10; gFrameDelta_s = 8;
    gPacketCursor_s = pkt_buf; gPacketLimit_s = pkt_buf + 0x38;
    gTickPlayerTransitionFade(1, 1);
    FCK_CHECK(gPlayerFadeTimers_s[1] == 0 && gPacketCursor_s == pkt_buf, "expired fade clamps to 0 without emitting");

    gPlayerFadeTimers_s[1] = 300; gFrameDelta_s = 10;
    gTickPlayerTransitionFade(1, 0);
    tl = (struct FadeTile *)(pkt_buf + 0x1C);
    FCK_CHECK(gPlayerFadeTimers_s[1] == 285 && gPacketCursor_s == pkt_buf + 0x38 &&
              tl->r == 0xFF && tl->g == 0xFF && tl->b == 0xFF && tl->w == 0xA0,
              "temp >= 0x100 clamps to 0xFF with width 0xA0");

    gPacketCursor_s = pkt_buf; gPlayerFadeTimers_s[1] = 100; gFrameDelta_s = 4;
    gTickPlayerTransitionFade(1, 1);
    FCK_CHECK(gPlayerFadeTimers_s[1] == 94 && tl->r == 94 && tl->w == 0x140,
              "arg1 low byte nonzero selects width 0x140");

    /* @leaf 8001C214 source src/game/8001C214.c: unpack 15-bit color, halve rect coords, forward to 8001C148 */
    LK_FUNC("func_8001C214");
    lk_fill(pkt_buf, 0xA5, sizeof(pkt_buf));
    gPacketCursor_s = pkt_buf; gPacketLimit_s = pkt_buf + 0x38;
    gEmitTranslucentTileRect(0x7C00U, 40, 60, 80, 100, 1);
    pg = (struct FadePage *)pkt_buf;
    tl = (struct FadeTile *)(pkt_buf + 0x1C);
    FCK_CHECK(pg->code == 0xE1000000U && tl->code == 0x62, "blend=1 emits 0x62 tile");
    FCK_CHECK(tl->r == 0x7C && tl->g == 0 && tl->b == 0, "red channel extracted");
    FCK_CHECK(tl->x == 20 && tl->y == 30 && tl->w == 40 && tl->h == 50, "rect coordinates halved");
    gPacketCursor_s = pkt_buf;
    gEmitTranslucentTileRect(0x03E0U | 0x0015U, -4, 10, 6, 8, 0);
    FCK_CHECK(tl->code == 0x60 && tl->r == 0 && tl->g == 0x7C && tl->b == 0x54 &&
              tl->x == -2 && tl->y == 5 && tl->w == 3 && tl->h == 4, "green/blue/blend=0/signed x");
}

/* ---- 19: Profile name entry status message (8008227C) ---- */
s8 gPlayerMenuDepth_s[4] __asm__("D_8009F8C0");
s8 *gFontBankTable_s __asm__("D_8009F468");
u8 *gSaveProfileImage_s __asm__("D_8009F92C");
void gSetProfileNameEntryStatusMessage(void) __asm__("func_8008227C");

static void test_profile_status(void)
{
    static u8 profile_buf[0x950];

    /* @leaf 8008227C source src/game/8008227C.c: write 0x385E halfword at +0x942 only */
    LK_FUNC("func_8008227C");
    lk_fill(profile_buf, 0xA5, sizeof(profile_buf));
    gSaveProfileImage_s = profile_buf;
    gSetProfileNameEntryStatusMessage();
    FCK_EQ(*(u16 *)(profile_buf + 0x942), 0x385E, "status word written at +0x942");
    FCK_CHECK(lk_all(profile_buf, 0, 0x942, 0xA5) && lk_all(profile_buf, 0x944, 0x950, 0xA5),
              "surrounding profile image bytes untouched");
}

/* ---- 20 & 21: RGB15->RGB24 and registry field scaling (8001BDE4, 8006A898) ---- */
unsigned int gExpandRgb15ToRgb24(unsigned int color) __asm__("func_8001BDE4");
struct ScaleObj {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
};
void gScaleRegistryObjectFields(struct ScaleObj *object) __asm__("func_8006A898");

static void test_color_and_scale(void)
{
    struct ScaleObj obj;

    /* @leaf 8001BDE4 source src/game/8001BDE4.c: 5-to-8 bit replication per channel packed via 8001BC44 */
    LK_FUNC("func_8001BDE4");
    FCK_EQ(gExpandRgb15ToRgb24(0x0000U), 0x000000U, "black");
    FCK_EQ(gExpandRgb15ToRgb24(0x7FFFU), 0xFFFFFFU, "white");
    FCK_EQ(gExpandRgb15ToRgb24(0x7C00U), 0xFF0000U, "max red");
    FCK_EQ(gExpandRgb15ToRgb24(0x03E0U), 0x00FF00U, "max green");
    FCK_EQ(gExpandRgb15ToRgb24(0x001FU), 0x0000FFU, "max blue");
    FCK_EQ(gExpandRgb15ToRgb24(0x4210U), 0x848484U, "bit replication on 16/16/16");
    FCK_EQ(gExpandRgb15ToRgb24(0x14A5U), 0x292929U, "bit replication on 5/5/5");

    /* @leaf 8006A898 source src/game/8006A898.c: scale embedded subrecord then (<<12)/218 on unk0A & unk08 */
    LK_FUNC("func_8006A898");
    lk_fill(&obj, 0xA5, sizeof(obj));
    obj.unk08 = 109;
    obj.unk0A = -218;
    obj.sub.unk06 = 900;
    obj.sub.unk08 = 300;
    obj.sub.unk0C = 218;
    gScaleRegistryObjectFields(&obj);
    FCK_EQ(obj.unk08, 2048, "unk08 scaled by 4096/218");
    FCK_EQ(obj.unk0A, -4096, "negative unk0A scaled by 4096/218");
    FCK_EQ(obj.sub.unk06, 3, "sub.unk06 / 300");
    FCK_EQ(obj.sub.unk08, 256, "sub.unk08 << 8 / 300");
    FCK_EQ(obj.sub.unk0C, 4096, "sub.unk0C << 12 / 218");
    FCK_CHECK(lk_all(&obj, 0, 8, 0xA5) && lk_all(&obj.sub, 0, 6, 0xA5) && lk_all(&obj.sub, 0xA, 0xC, 0xA5),
              "padding bytes untouched");
}

/* ---- 22, 23 & 24: GPU packet builders (80093954, 80093A0C, 80093A9C) ---- */
void gSetTexWindow(u8 *obj, u8 *arg) __asm__("func_80093954");
void gSetDrawOffsetPacket(u8 *obj, s16 *rect) __asm__("func_80093A0C");
struct TPagePkt { u8 pad[3]; u8 length; u32 command; u32 data; };
void gSetDrawTPage(struct TPagePkt *packet, int arg1, int arg2, u16 arg3, int arg4) __asm__("func_80093A9C");

static void test_gpu_packet_builders(void)
{
    u8 pkt[16];
    s16 rect[4];
    struct TPagePkt tp;

    /* @leaf 80093954 source src/game/80093954.c: 2-word GP0 0xE2 texture-window packet */
    LK_FUNC("func_80093954");
    lk_fill(pkt, 0xA5, sizeof(pkt));
    gSetTexWindow(pkt, 0);
    FCK_CHECK(pkt[3] == 2 && *(u32 *)(pkt + 4) == 0 && *(u32 *)(pkt + 8) == 0, "null arg emits 0 command");
    FCK_CHECK(lk_all(pkt, 0, 3, 0xA5) && lk_all(pkt, 12, 16, 0xA5), "surrounding bytes untouched");
    rect[0] = 16; rect[1] = 24; rect[2] = 32; rect[3] = 40;
    gSetTexWindow(pkt, (u8 *)rect);
    FCK_EQ(*(u32 *)(pkt + 4), 0xE2018B7CU, "packed 0xE2 tex-window word");
    FCK_EQ(*(u32 *)(pkt + 8), 0U, "trailing word cleared");

    /* @leaf 80093A0C source src/game/80093A0C.c: 2-word GP0 0xE5 drawing-offset packet */
    LK_FUNC("func_80093A0C");
    lk_fill(pkt, 0xA5, sizeof(pkt));
    rect[0] = 320; rect[1] = 240;
    gSetDrawOffsetPacket(pkt, rect);
    FCK_CHECK(pkt[3] == 2 && *(u32 *)(pkt + 4) == 0xE5078140U && *(u32 *)(pkt + 8) == 0,
              "positive offset packed");
    FCK_CHECK(lk_all(pkt, 0, 3, 0xA5) && lk_all(pkt, 12, 16, 0xA5), "surrounding bytes untouched");
    rect[0] = -1; rect[1] = -2;
    gSetDrawOffsetPacket(pkt, rect);
    FCK_EQ(*(u32 *)(pkt + 4), 0xE53FF7FFU, "negative 11-bit offsets masked");

    /* @leaf 80093A9C source src/game/80093A9C.c: 2-word draw-mode (0xE1) + tex-window (0xE2) packet */
    LK_FUNC("func_80093A9C");
    lk_fill(&tp, 0xA5, sizeof(tp));
    gSetDrawTPage(&tp, 1, 1, 0x135, 0);
    FCK_CHECK(tp.length == 2 && tp.command == 0xE1000735U && tp.data == 0, "dfe=1 dtd=1 null texwin");
    FCK_CHECK(lk_all(&tp, 0, 3, 0xA5), "tag bytes untouched");
    rect[0] = 16; rect[1] = 24; rect[2] = 32; rect[3] = 40;
    gSetDrawTPage(&tp, 0, 1, 0xFFFF, (int)rect);
    FCK_CHECK(tp.command == 0xE1000BFFU && tp.data == 0xE2018B7CU, "dfe=0 dtd=1 masked tpage + texwin");
}

int main(void)
{
    test_gpu_callbacks();
    test_gpu_ports();
    test_platform_hooks();
    test_cd_audio_mix();
    test_pad_helpers();
    test_entity_msg();
    test_prim_emitters();
    test_profile_status();
    test_color_and_scale();
    test_gpu_packet_builders();
    return lk_done("leaf_fifty_a");
}
