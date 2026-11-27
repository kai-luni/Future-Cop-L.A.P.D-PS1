/* Hardware-free tranche 1 leaf suite: 25 unchanged original MIPS C units from
 * src/game/ running under qemu-mipsel on explicit synthetic RAM fixtures and
 * recording boundary stand-ins. All expectations are marked `source` because
 * they pin the behavior of the decompiled units on synthetic inputs.
 */
/* @mock sound_mock */
#include "leaf_kit.h"
#include "mocks/sound_mock.h"

/* ============================================================================
 * Shared synthetic globals (D_ labels referenced by the 25 units)
 * ============================================================================
 */

/* 80091E8C */
unsigned int *gDmaInterruptCtrlPtr __asm__("D_8009DD60");
unsigned int gDmaCallbackTable[16] __asm__("D_8009DD64");

/* 800867B0 */
void *gMovieAudioState __asm__("D_8009F4D8");
static u8 sMovieAudioStorage[0x7B80];

/* 80048978 */
s32 gRampFillCounter __asm__("D_8009F360");

/* 80059E9C / 8007A450 / 8007E7A8 / 80025D6C */
void *gGameSession[2] __asm__("D_8009FA94");
void *gSinglePlayerEntity[2] __asm__("D_8009F9EC");
void *gPlayerEntities[4] __asm__("D_8009FA58");

/* 800987B8 */
volatile u32 gModeEntryWord __asm__("D_8009EA5C");
volatile u32 gSpuRegisterShadowMode __asm__("D_8009EE90");
volatile u16 *gSpuRegBasePtr __asm__("D_8009EEA4");
volatile u16 gSpuShadowTable[0x100] __asm__("D_800A6EA0");

/* 8007A450 */
u8 gCwRulesMovieName[16] __asm__("D_80012088");
u8 gLayoutPointerFlagB __asm__("D_8009F3E7");
u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
u8 gCrimeWarProgress __asm__("D_8009F458");
void *gPendingRowCapture __asm__("D_8009F8C4");
s32 gDeferredTransition __asm__("D_8009F8C8");
u8 gAudioOptionsMenuMode[4] __asm__("D_800A377D");
u8 gCrimeWarHubMenuInputMode[4] __asm__("D_800A386D");

/* 8007E7A8 */
u8 gHexDigitTable[16] __asm__("D_8009C1AC");
u8 gOctalDigitTable[8] __asm__("D_8009C1BC");
volatile s32 gScalarCursor __asm__("D_8009F8F8");
u16 gStatusTextId __asm__("D_8009F92A");
u8 gArmedFlag __asm__("D_8009FAC4");
u8 gStateByteFB[4] __asm__("D_800A35FB");
u8 gStateByteFD[4] __asm__("D_800A35FD");
u8 gPackedPasswordBytes[16] __asm__("D_800A3938");
u8 gPasswordBuffer[16] __asm__("D_800A3948");

/* 8008352C */
void *gMissionTable __asm__("D_8009F930");

/* 8004DDF4 */
void *gUnitListHead __asm__("D_8009FA40");

/* 80025CFC */
s16 gTeamFlag[4] __asm__("D_8009F65C");

/* 800217A8 / 800216D8 / 80021998 */
struct HwSoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    void *channels;
};
struct HwSoundSystem gSoundSystem __asm__("D_800A07A8");

/* 80066684 */
u8 gFrameTime __asm__("D_8009FAA4");
s32 gMarkerColorTable[32] __asm__("D_800A6F60");

/* 800825AC */
void *gAtlasTable __asm__("D_8009F45C");
void *gSpriteAtlasBlock __asm__("D_8009F460");

/* 8005A828 */
s16 gRadiusTable[6] __asm__("D_8009AFB4");
u8 *gPacketLimit __asm__("D_8009F9C0");
u8 *gPacketCursor __asm__("D_8009FA90");

/* 80025D6C */
s32 gPlayerCount __asm__("D_8009FA0C");
s16 gPlayerFadeTimers[4] __asm__("D_8009FA44");

/* 8007C80C */
s32 gActivePlayerIndex __asm__("D_8009F8B8");
u8 gRumbleEnabled[4] __asm__("D_8009F8D8");
s8 gStagedControllerPresets[4] __asm__("D_8009F900");
u8 gControllerDiagramViews[4] __asm__("D_8009F902");
s8 gSnapshotResult __asm__("D_8009F904");

/* ============================================================================
 * Boundary stand-ins (all bound to retail labels via __asm__)
 * ============================================================================
 */

/* --- func_80096748: unsigned 64-bit div/mod stand-in --- */
typedef unsigned long long u64;
typedef union { u64 whole; struct { u32 lo, hi; } words; } Pair64;

static struct {
    s32 calls;
    u32 a, b, c, d;
    s32 mode;
    u32 ret_lo, ret_hi;
} s_div64;

static void reset_div64(u32 ret_lo, u32 ret_hi)
{
    memset(&s_div64, 0, sizeof(s_div64));
    s_div64.ret_lo = ret_lo;
    s_div64.ret_hi = ret_hi;
}

u64 mock_div64(u32 a, u32 b, u32 c, u32 d, int mode) __asm__("func_80096748");
u64 mock_div64(u32 a, u32 b, u32 c, u32 d, int mode)
{
    Pair64 p;
    s_div64.calls++;
    s_div64.a = a;
    s_div64.b = b;
    s_div64.c = c;
    s_div64.d = d;
    s_div64.mode = mode;
    p.words.lo = s_div64.ret_lo;
    p.words.hi = s_div64.ret_hi;
    return p.whole;
}

/* --- func_800488A4: text palette row writer --- */
static struct {
    s32 calls;
    s32 index;
    u8 blue, green, red;
} s_palette;

void mock_build_ramp(s32 index, u8 blue, u8 green, u8 red) __asm__("func_800488A4");
void mock_build_ramp(s32 index, u8 blue, u8 green, u8 red)
{
    s_palette.calls++;
    s_palette.index = index;
    s_palette.blue = blue;
    s_palette.green = green;
    s_palette.red = red;
}

/* --- func_800776B8: Crime War 2P reset callback --- */
static struct {
    s32 calls;
    s32 seen_players;
    s32 seen_split;
    u8 seen_saved;
    u8 seen_progress;
    u8 seen_audio_mode;
    u8 seen_hub_mode;
    s32 seen_transition;
} s_cw_reset;

void mock_cw_reset(void) __asm__("func_800776B8");
void mock_cw_reset(void)
{
    s32 *mode = (s32 *)gGameSession[0];
    s_cw_reset.calls++;
    s_cw_reset.seen_players = mode[3];
    s_cw_reset.seen_split = mode[0x5C / 4];
    s_cw_reset.seen_saved = gSavedCrimeWarProgress;
    s_cw_reset.seen_progress = gCrimeWarProgress;
    s_cw_reset.seen_audio_mode = gAudioOptionsMenuMode[0];
    s_cw_reset.seen_hub_mode = gCrimeWarHubMenuInputMode[0];
    s_cw_reset.seen_transition = gDeferredTransition;
}

/* --- func_8007F02C: pack current progress password --- */
static struct {
    s32 calls;
    u8 seen_armed;
    u8 seen_fd;
    u8 next_bytes[10];
} s_pack_pw;

void mock_pack_pw(void) __asm__("func_8007F02C");
void mock_pack_pw(void)
{
    s32 i;
    s_pack_pw.calls++;
    s_pack_pw.seen_armed = gArmedFlag;
    s_pack_pw.seen_fd = gStateByteFD[0];
    for (i = 0; i < 10; i++) {
        gPackedPasswordBytes[i] = s_pack_pw.next_bytes[i];
    }
}

/* --- func_8001EA5C: large vibration motor setter --- */
static struct {
    s32 calls;
    s32 last_index;
} s_large_motor;

void mock_large_motor(int index) __asm__("func_8001EA5C");
void mock_large_motor(int index)
{
    s_large_motor.calls++;
    s_large_motor.last_index = index;
}

/* --- func_80099C58: SPU single voice attribute setter --- */
static struct {
    s32 calls;
    s32 ch;
    u8 attr[0x40];
} s_spu_one;

void mock_spu_one(s32 ch, void *attr) __asm__("func_80099C58");
void mock_spu_one(s32 ch, void *attr)
{
    s_spu_one.calls++;
    s_spu_one.ch = ch;
    memcpy(s_spu_one.attr, attr, sizeof(s_spu_one.attr));
}

/* --- func_80099368: SPU multi-voice attribute setter --- */
static struct {
    s32 calls;
    u8 req[0x40];
} s_spu_multi;

void mock_spu_multi(void *req) __asm__("func_80099368");
void mock_spu_multi(void *req)
{
    s_spu_multi.calls++;
    memcpy(s_spu_multi.req, req, sizeof(s_spu_multi.req));
}

/* --- func_80045768 / func_800457B8: sound slot active query & stop --- */
static struct {
    s32 active_calls;
    s32 active_a0[8];
    s32 active_idx[8];
    s32 active_ret[8];
    s32 stop_calls;
    s32 stop_a0[8];
    s32 stop_idx[8];
} s_slot_snd;

s32 mock_slot_active(s32 arg0, s32 index) __asm__("func_80045768");
s32 mock_slot_active(s32 arg0, s32 index)
{
    s32 n = s_slot_snd.active_calls++;
    if (n < 8) {
        s_slot_snd.active_a0[n] = arg0;
        s_slot_snd.active_idx[n] = index;
        return s_slot_snd.active_ret[n];
    }
    return 0;
}

void mock_slot_stop(s32 arg0, s32 index) __asm__("func_800457B8");
void mock_slot_stop(s32 arg0, s32 index)
{
    s32 n = s_slot_snd.stop_calls++;
    if (n < 8) {
        s_slot_snd.stop_a0[n] = arg0;
        s_slot_snd.stop_idx[n] = index;
    }
}

/* --- func_8004DA84: entity command dispatcher --- */
static struct {
    s32 calls;
    void *entity[8];
    s32 kind[8];
    s32 value[8];
    s32 retry[8];
} s_ent_cmd;

u8 mock_ent_cmd(void *entity, s32 kind, s32 value, s32 retry) __asm__("func_8004DA84");
u8 mock_ent_cmd(void *entity, s32 kind, s32 value, s32 retry)
{
    s32 n = s_ent_cmd.calls++;
    if (n < 8) {
        s_ent_cmd.entity[n] = entity;
        s_ent_cmd.kind[n] = kind;
        s_ent_cmd.value[n] = value;
        s_ent_cmd.retry[n] = retry;
    }
    return 0;
}

/* --- func_80047284: positional sound effect --- */
static struct {
    s32 calls;
    s32 id, a1, a2, a3;
} s_pos_sfx;

void mock_pos_sfx(s32 id, s32 a1, s32 a2, s32 a3) __asm__("func_80047284");
void mock_pos_sfx(s32 id, s32 a1, s32 a2, s32 a3)
{
    s_pos_sfx.calls++;
    s_pos_sfx.id = id;
    s_pos_sfx.a1 = a1;
    s_pos_sfx.a2 = a2;
    s_pos_sfx.a3 = a3;
}

/* --- func_80021DAC: unregister sound bank --- */
static struct {
    s32 calls;
    s32 indices[8];
    s32 shrink_on_index;
    s32 shrink_to;
} s_unreg_bank;

void mock_unreg_bank(s32 index) __asm__("func_80021DAC");
void mock_unreg_bank(s32 index)
{
    s32 n = s_unreg_bank.calls++;
    if (n < 8) {
        s_unreg_bank.indices[n] = index;
    }
    if (s_unreg_bank.shrink_on_index >= 0 && index == s_unreg_bank.shrink_on_index) {
        gSoundSystem.bankCount = s_unreg_bank.shrink_to;
    }
}

/* --- func_80098708: claim/release SPU entry --- */
static struct {
    s32 calls;
    s32 last_handle;
} s_spu_claim;

void mock_spu_claim(s32 handle) __asm__("func_80098708");
void mock_spu_claim(s32 handle)
{
    s_spu_claim.calls++;
    s_spu_claim.last_handle = handle;
}

/* --- func_8001484C: gouraud disc emitter --- */
struct Vec3s16 { s16 x, y, z; };
static struct {
    s32 calls;
    struct Vec3s16 pos[8];
    s32 color[8];
    s32 a2[8], a3[8], a4[8], a5[8], a6[8];
} s_disc;

void mock_emit_disc(struct Vec3s16 *v, s32 color, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6) __asm__("func_8001484C");
void mock_emit_disc(struct Vec3s16 *v, s32 color, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6)
{
    s32 n = s_disc.calls++;
    if (n < 8) {
        s_disc.pos[n] = *v;
        s_disc.color[n] = color;
        s_disc.a2[n] = a2;
        s_disc.a3[n] = a3;
        s_disc.a4[n] = a4;
        s_disc.a5[n] = a5;
        s_disc.a6[n] = a6;
    }
}

/* --- func_800181D4: block free --- */
static struct {
    s32 calls;
    void *ptrs[8];
} s_free_blk;

void mock_free_block(void *p) __asm__("func_800181D4");
void mock_free_block(void *p)
{
    s32 n = s_free_blk.calls++;
    if (n < 8) {
        s_free_blk.ptrs[n] = p;
    }
}

/* --- func_8001846C: sin/cos stand-in --- */
static struct {
    s32 calls;
    u32 last_angle;
    s32 sine;
    s32 cosine;
} s_sincos;

void mock_sincos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void mock_sincos(u32 angle, s32 *sine, s32 *cosine)
{
    s_sincos.calls++;
    s_sincos.last_angle = angle;
    *sine = s_sincos.sine;
    *cosine = s_sincos.cosine;
}

/* --- func_8001A4E8: distance helper stand-in --- */
static struct {
    s32 calls;
    s32 *arg_a[8];
    s32 *arg_b[8];
    s32 ret[8];
} s_dist;

s32 mock_dist(s32 *a, s32 *b) __asm__("func_8001A4E8");
s32 mock_dist(s32 *a, s32 *b)
{
    s32 n = s_dist.calls++;
    if (n < 8) {
        s_dist.arg_a[n] = a;
        s_dist.arg_b[n] = b;
        return s_dist.ret[n];
    }
    return 0;
}

/* --- func_800207CC / func_80020DF8: memory card directory & slot state --- */
static struct {
    s32 parse_calls;
    s32 state_calls;
    s32 states[16];
} s_card;

void mock_parse_card(void) __asm__("func_800207CC");
void mock_parse_card(void)
{
    s_card.parse_calls++;
}

s32 mock_card_state(u32 slot) __asm__("func_80020DF8");
s32 mock_card_state(u32 slot)
{
    s_card.state_calls++;
    if (slot < 16) {
        return s_card.states[slot];
    }
    return -1;
}

/* --- func_800684AC: map raycast stand-in --- */
struct Vec3s32 { s32 x, y, z; };
static struct {
    s32 calls;
    struct Vec3s32 p0[4];
    struct Vec3s32 p1[4];
    s32 hit_nonnull[4];
    s32 a3[4];
    s32 a4[4];
    s32 ret[4];
    struct Vec3s32 write_hit[4];
} s_ray;

s32 mock_trace_ray(struct Vec3s32 *a, struct Vec3s32 *b, struct Vec3s32 *hit, s32 a3, s32 a4) __asm__("func_800684AC");
s32 mock_trace_ray(struct Vec3s32 *a, struct Vec3s32 *b, struct Vec3s32 *hit, s32 a3, s32 a4)
{
    s32 n = s_ray.calls++;
    if (n < 4) {
        s_ray.p0[n] = *a;
        s_ray.p1[n] = *b;
        s_ray.hit_nonnull[n] = (hit != 0);
        s_ray.a3[n] = a3;
        s_ray.a4[n] = a4;
        if (hit != 0) {
            *hit = s_ray.write_hit[n];
        }
        return s_ray.ret[n];
    }
    return 0;
}

/* --- func_8007BFCC / func_8001EC04: controller preset & rumble apply --- */
static struct {
    s32 preset_calls;
    s32 preset_port;
    s32 preset_val;
    s32 rumble_calls;
    s32 rumble_port;
    s32 rumble_enable;
} s_ctrl_apply;

void mock_apply_preset(s32 port, s32 value) __asm__("func_8007BFCC");
void mock_apply_preset(s32 port, s32 value)
{
    s_ctrl_apply.preset_calls++;
    s_ctrl_apply.preset_port = port;
    s_ctrl_apply.preset_val = value;
}

void mock_set_rumble(s32 port, s32 enable) __asm__("func_8001EC04");
void mock_set_rumble(s32 port, s32 enable)
{
    s_ctrl_apply.rumble_calls++;
    s_ctrl_apply.rumble_port = port;
    s_ctrl_apply.rumble_enable = enable;
}

/* ============================================================================
 * Prototypes of the 25 original units under test
 * ============================================================================
 */

unsigned int gSetDmaChannelInterruptCallback(int arg0, unsigned int arg1) __asm__("func_80091E8C");
void gPackInputRecord(u8 *out0, u32 a, u32 b, u32 c0, u32 d0, u32 e0, u32 f0, u32 g0, u32 h0, u32 i0, u32 j0, u32 k0, u32 l0, u32 m0) __asm__("func_8007E8D8");
u64 gDivSigned64(u32 a, u32 b, u32 c, u32 d) __asm__("func_800966B0");
s32 gScaleMovieAudioClock(u32 arg0) __asm__("func_800867B0");
void gAddTextPaletteColor(s32 blue, s32 green, s32 red) __asm__("func_80048978");
s32 gGetPlayerResultsStat(void *player, s32 id) __asm__("func_80059E9C");
u32 gSpuAccessVoiceMaskRegs(s32 mode, u32 bits, s32 low, s32 high) __asm__("func_800987B8");
void gSelectCrimeWar2P(s32 a0) __asm__("func_8007A450");
void gInitModeDependentState(void) __asm__("func_8007E7A8");
void gSetEntityLargeRumble(void *state) __asm__("func_8001EA20");
void gApplySoundVoiceAttributes(void *arg0, s16 arg1, s16 arg2, s16 arg3) __asm__("func_80021AD0");
void gSilenceSpuVoicesExcept(s32 voice) __asm__("func_80021BD0");
void gUpdateSoundSlotTimers(s32 elapsed) __asm__("func_8008352C");
void gSendCommandByTeam(s32 team, s32 kind, s32 value) __asm__("func_8004DDF4");
void gSignalFirstTeamRetirement(u8 *arg0) __asm__("func_80025CFC");
void gUnloadAllSoundBanks(void) __asm__("func_800217A8");
void gClearSoundStorage(void) __asm__("func_800216D8");
void gReleaseSoundBankSpuHandle(s32 index) __asm__("func_80021998");
void gDrawMinimapMarkers(u8 *obj, s32 *pos, s32 width, s32 height) __asm__("func_80066684");
void gValidateAtlasSpriteId(void *object) __asm__("func_800825AC");
void gEmitRadarTriangle(s32 origin, u32 angle, s32 code) __asm__("func_8005A828");
void gSetPlayerFadeTimersByDistance(s32 *arg0) __asm__("func_80025D6C");
u32 gFindFreeCardBlockRun(u32 size) __asm__("func_80020E68");
s32 gTestCameraSightLine(struct Vec3s32 *from, struct Vec3s32 *to) __asm__("func_80055EE8");
void gHandleControllerSetupAction(s32 action) __asm__("func_8007C80C");

/* ============================================================================
 * 1. 80091E8C - gSetDmaChannelInterruptCallback
 * ============================================================================
 */
static void test_80091E8C(void)
{
    unsigned int ctrl;
    unsigned int prev;
    s32 i;

    /* @leaf 80091E8C source DMA callback table update, bit-23/channel enable/disable, unchanged no-op and index-7 mask boundary */
    LK_FUNC("func_80091E8C");

    for (i = 0; i < 16; i++) {
        gDmaCallbackTable[i] = 0x55000000U + (unsigned int)i;
    }
    gDmaInterruptCtrlPtr = &ctrl;

    /* Install nonzero callback at channel 2: sets bit 18 and bit 23, drops top byte. */
    gDmaCallbackTable[2] = 0x11111111U;
    ctrl = 0xAA001234U;
    prev = gSetDmaChannelInterruptCallback(2, 0x80042222U);
    FCK_EQ((s32)prev, (s32)0x11111111U, "returns old entry");
    FCK_EQ((s32)gDmaCallbackTable[2], (s32)0x80042222U, "stores new entry");
    FCK_EQ((s32)ctrl, (s32)0x00841234U, "sets bit 23 and bit 18 and drops top byte");
    FCK_EQ((s32)gDmaCallbackTable[1], (s32)0x55000001U, "lower neighbor untouched");
    FCK_EQ((s32)gDmaCallbackTable[3], (s32)0x55000003U, "upper neighbor untouched");

    /* Same value: no write to control word (top byte preserved). */
    ctrl = 0xBB005678U;
    prev = gSetDmaChannelInterruptCallback(2, 0x80042222U);
    FCK_EQ((s32)prev, (s32)0x80042222U, "unchanged returns existing");
    FCK_EQ((s32)ctrl, (s32)0xBB005678U, "unchanged skips ctrl write");

    /* Clear to 0 at channel 2: clears bit 18, keeps bit 23, drops top byte. */
    ctrl = 0xCC041234U;
    prev = gSetDmaChannelInterruptCallback(2, 0U);
    FCK_EQ((s32)prev, (s32)0x80042222U, "clear returns old entry");
    FCK_EQ((s32)gDmaCallbackTable[2], 0, "table entry cleared");
    FCK_EQ((s32)ctrl, (s32)0x00801234U, "clears bit 18 while setting bit 23");

    /* Clear channel 7 (shift 23): clearing ~(1<<23) wins after ORing 0x800000. */
    gDmaCallbackTable[7] = 0xDEADBEEFU;
    ctrl = 0xFF810005U;
    prev = gSetDmaChannelInterruptCallback(7, 0U);
    FCK_EQ((s32)prev, (s32)0xDEADBEEFU, "ch7 returns old entry");
    FCK_EQ((s32)gDmaCallbackTable[7], 0, "ch7 cleared");
    FCK_EQ((s32)ctrl, (s32)0x00010005U, "ch7 clear clears bit 23 itself");
}

/* ============================================================================
 * 2. 8007E8D8 - gPackInputRecord
 * ============================================================================
 */
static void test_8007E8D8(void)
{
    u8 buf[14];
    static const u8 exp_normal[10] = { 15, 5, 13, 30, 4, 5, 6, 11, 5, 3 };
    static const u8 exp_sp_lo[10]  = {  9, 9, 10,  5, 10, 4, 10, 6, 7, 4 };
    static const u8 exp_sp_eq6[10] = { 10, 0, 14, 11, 11, 5, 10, 13, 3, 5 };
    static const u8 exp_sp_hi[10]  = {  9, 0, 13, 11, 14, 5, 10, 13, 7, 3 };

    /* @leaf 8007E8D8 source 13-input 10-byte bit packer, normal path, special-case f..i==10 for l<6 and l>=6 with high-bit masking and guard bytes */
    LK_FUNC("func_8007E8D8");

    /* Normal path (f0 != 10). */
    lk_fill(buf, 0xA5, sizeof(buf));
    gPackInputRecord(&buf[2], 1, 1, 2, 7, 9, 7, 4, 6, 11, 1, 1, 2, 1);
    FCK_CHECK(memcmp(&buf[2], exp_normal, 10) == 0, "normal 10-byte pack");
    FCK_CHECK(lk_all(buf, 0, 2, 0xA5) && lk_all(buf, 12, 14, 0xA5), "normal guards intact");

    /* Special case (f0..i0 low bytes all 10) with l0 < 6 (l0 = 4 -> f += 4, g += 0). */
    lk_fill(buf, 0x5A, sizeof(buf));
    gPackInputRecord(&buf[2], 2, 0, 1, 1, 5, 10, 10, 10, 10, 0, 1, 4, 0);
    FCK_CHECK(memcmp(&buf[2], exp_sp_lo, 10) == 0, "special case l<6 pack");
    FCK_CHECK(lk_all(buf, 0, 2, 0x5A) && lk_all(buf, 12, 14, 0x5A), "special l<6 guards intact");

    /* Special case at exact threshold l0 == 6 (f += 5, g += 1). */
    lk_fill(buf, 0x3C, sizeof(buf));
    gPackInputRecord(&buf[2], 0, 1, 3, 2, 12, 10, 10, 10, 10, 1, 0, 6, 1);
    FCK_CHECK(memcmp(&buf[2], exp_sp_eq6, 10) == 0, "special case l==6 pack");
    FCK_CHECK(lk_all(buf, 0, 2, 0x3C) && lk_all(buf, 12, 14, 0x3C), "special l==6 guards intact");

    /* Special case with upper bits in f0..i0 and l0 (l0 & 0xFF == 9 >= 6 -> f += 5, g += 4). */
    lk_fill(buf, 0xC3, sizeof(buf));
    gPackInputRecord(&buf[2], 0, 1, 3, 2, 12, 0x10A, 0x20A, 0x0A, 0x30A, 1, 0, 0x109, 1);
    FCK_CHECK(memcmp(&buf[2], exp_sp_hi, 10) == 0, "special case l>=6 pack");
    FCK_CHECK(lk_all(buf, 0, 2, 0xC3) && lk_all(buf, 12, 14, 0xC3), "special l>=6 guards intact");
}

/* ============================================================================
 * 3. 800966B0 - gDivSigned64
 * ============================================================================
 */
static void test_800966B0(void)
{
    Pair64 res;

    /* @leaf 800966B0 source signed 64-bit magnitude conversion and result negation across all four sign quadrants including lo==0 borrow */
    LK_FUNC("func_800966B0");

    /* (+ / +) */
    reset_div64(0x11223344U, 0x00000005U);
    res.whole = gDivSigned64(0x12345678U, 0x00000002U, 0x00000010U, 0x00000000U);
    FCK_EQ(s_div64.calls, 1, "pos/pos calls helper");
    FCK_EQ((s32)s_div64.a, (s32)0x12345678U, "pos/pos a");
    FCK_EQ((s32)s_div64.b, 2, "pos/pos b");
    FCK_EQ((s32)s_div64.c, 0x10, "pos/pos c");
    FCK_EQ((s32)s_div64.d, 0, "pos/pos d");
    FCK_EQ(s_div64.mode, 0, "pos/pos mode 0");
    FCK_EQ((s32)res.words.lo, (s32)0x11223344U, "pos/pos res lo");
    FCK_EQ((s32)res.words.hi, 5, "pos/pos res hi");

    /* (- / +) with lo != 0: -0x0000000200000005 = (0xFFFFFFFB, 0xFFFFFFFD) */
    reset_div64(0x00000007U, 0x00000001U);
    res.whole = gDivSigned64(0xFFFFFFFBU, 0xFFFFFFFDU, 0x00000003U, 0x00000000U);
    FCK_EQ((s32)s_div64.a, 5, "neg/pos mag a");
    FCK_EQ((s32)s_div64.b, 2, "neg/pos mag b");
    FCK_EQ((s32)res.words.lo, (s32)0xFFFFFFF9U, "neg/pos negated res lo");
    FCK_EQ((s32)res.words.hi, (s32)0xFFFFFFFEU, "neg/pos negated res hi");

    /* (+ / -) with lo == 0 (no borrow into hi): -0x0000000300000000 = (0, 0xFFFFFFFD) */
    reset_div64(0x00000000U, 0x00000004U);
    res.whole = gDivSigned64(0x00000020U, 0x00000001U, 0x00000000U, 0xFFFFFFFDU);
    FCK_EQ((s32)s_div64.c, 0, "pos/neg zero-lo c");
    FCK_EQ((s32)s_div64.d, 3, "pos/neg zero-lo d");
    FCK_EQ((s32)res.words.lo, 0, "pos/neg zero-lo res lo");
    FCK_EQ((s32)res.words.hi, (s32)0xFFFFFFFCU, "pos/neg zero-lo res hi");

    /* (- / -) cancels sign: result stays positive. */
    reset_div64(0x0000ABCDU, 0x00000009U);
    res.whole = gDivSigned64(0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFF0U, 0xFFFFFFFFU);
    FCK_EQ((s32)s_div64.a, 1, "neg/neg a");
    FCK_EQ((s32)s_div64.b, 0, "neg/neg b");
    FCK_EQ((s32)s_div64.c, 0x10, "neg/neg c");
    FCK_EQ((s32)s_div64.d, 0, "neg/neg d");
    FCK_EQ((s32)res.words.lo, 0xABCD, "neg/neg positive res lo");
    FCK_EQ((s32)res.words.hi, 9, "neg/neg positive res hi");

    /* (+ / +) with high word == 0 must stay non-negative (tests b < 0 vs b <= 0). */
    reset_div64(0x00001234U, 0x00000000U);
    res.whole = gDivSigned64(0x12345678U, 0x00000000U, 0x00000010U, 0x00000000U);
    FCK_EQ((s32)s_div64.a, (s32)0x12345678U, "zero-hi pos a unchanged");
    FCK_EQ((s32)s_div64.b, 0, "zero-hi pos b unchanged");
    FCK_EQ((s32)res.words.lo, 0x1234, "zero-hi pos res lo");
    FCK_EQ((s32)res.words.hi, 0, "zero-hi pos res hi");
}

/* ============================================================================
 * 4. 800867B0 - gScaleMovieAudioClock (calls real func_800966B0 -> mock 80096748)
 * ============================================================================
 */
static void test_800867B0(void)
{
    s32 *mode_ptr = (s32 *)(sMovieAudioStorage + 0x7B74);
    s32 *val_ptr  = (s32 *)(sMovieAudioStorage + 0x7B7C);
    s32 r;

    /* @leaf 800867B0 source movie audio clock 4-way mode/rate multiplier+divisor selection via real func_800966B0 */
    LK_FUNC("func_800867B0");

    memset(sMovieAudioStorage, 0xAA, sizeof(sMovieAudioStorage));
    gMovieAudioState = sMovieAudioStorage;

    /* Mode 1: product = arg0 * 0x40, divisor = 0x3F. */
    *mode_ptr = 1;
    *val_ptr = 0x800;
    reset_div64(0x123456U, 0U);
    r = gScaleMovieAudioClock(0x10000005U);
    FCK_EQ(r, 0x123456, "mode 1 return");
    FCK_EQ(s_div64.calls, 1, "mode 1 calls divider");
    FCK_EQ((s32)s_div64.a, 0x140, "mode 1 prod lo");
    FCK_EQ((s32)s_div64.b, 4, "mode 1 prod hi");
    FCK_EQ((s32)s_div64.c, 0x3F, "mode 1 div lo");
    FCK_EQ((s32)s_div64.d, 0, "mode 1 div hi");

    /* Mode 0, val == 0x800: product = arg0 * 0x14D0, divisor = 0x1117. */
    *mode_ptr = 0;
    *val_ptr = 0x800;
    reset_div64(0x222222U, 0U);
    r = gScaleMovieAudioClock(0x00100003U);
    FCK_EQ(r, 0x222222, "mode 0/0x800 return");
    FCK_EQ((s32)s_div64.a, 0x4D003E70, "mode 0/0x800 prod lo");
    FCK_EQ((s32)s_div64.b, 1, "mode 0/0x800 prod hi");
    FCK_EQ((s32)s_div64.c, 0x1117, "mode 0/0x800 div lo");
    FCK_EQ((s32)s_div64.d, 0, "mode 0/0x800 div hi");

    /* Mode 0, val != 0x800: product = arg0 * 0x6D440, divisor = 0x82BA3. */
    *mode_ptr = 0;
    *val_ptr = 0x7FF;
    reset_div64(0x333333U, 0U);
    r = gScaleMovieAudioClock(0x00010002U);
    FCK_EQ(r, 0x333333, "mode 0/other return");
    FCK_EQ((s32)s_div64.a, (s32)0xD44DA880U, "mode 0/other prod lo");
    FCK_EQ((s32)s_div64.b, 6, "mode 0/other prod hi");
    FCK_EQ((s32)s_div64.c, 0x82BA3, "mode 0/other div lo");
    FCK_EQ((s32)s_div64.d, 0, "mode 0/other div hi");

    /* Unsupported modes return 0 without invoking divider. */
    *mode_ptr = 2;
    reset_div64(0x999999U, 0U);
    FCK_EQ(gScaleMovieAudioClock(100U), 0, "mode 2 returns 0");
    FCK_EQ(s_div64.calls, 0, "mode 2 skips divider");

    *mode_ptr = -1;
    FCK_EQ(gScaleMovieAudioClock(100U), 0, "mode -1 returns 0");
    FCK_EQ(s_div64.calls, 0, "mode -1 skips divider");
}

/* ============================================================================
 * 5. 80048978 - gAddTextPaletteColor
 * ============================================================================
 */
static void test_80048978(void)
{
    /* @leaf 80048978 source bounded 20-entry text palette append, u8 channel forwarding and saturation at 0x14 */
    LK_FUNC("func_80048978");

    memset(&s_palette, 0, sizeof(s_palette));
    gRampFillCounter = 0;
    gAddTextPaletteColor(0x12, 0x34, 0x56);
    FCK_EQ(s_palette.calls, 1, "first append calls writer");
    FCK_EQ(s_palette.index, 0, "first index 0");
    FCK_EQ(s_palette.blue, 0x12, "blue forwarded");
    FCK_EQ(s_palette.green, 0x34, "green forwarded");
    FCK_EQ(s_palette.red, 0x56, "red forwarded");
    FCK_EQ(gRampFillCounter, 1, "counter increments to 1");

    /* Last free slot (19 == 0x13) with high bits truncated to u8. */
    gRampFillCounter = 19;
    gAddTextPaletteColor(0x1AB, 0x2CD, 0x3EF);
    FCK_EQ(s_palette.calls, 2, "slot 19 calls writer");
    FCK_EQ(s_palette.index, 19, "slot 19 index");
    FCK_EQ(s_palette.blue, 0xAB, "blue u8");
    FCK_EQ(s_palette.green, 0xCD, "green u8");
    FCK_EQ(s_palette.red, 0xEF, "red u8");
    FCK_EQ(gRampFillCounter, 20, "counter reaches 20");

    /* Full table (20 == 0x14) drops further appends. */
    gAddTextPaletteColor(1, 2, 3);
    FCK_EQ(s_palette.calls, 2, "full table skips writer");
    FCK_EQ(gRampFillCounter, 20, "counter stays at 20");
}

/* ============================================================================
 * 6. 80059E9C - gGetPlayerResultsStat
 * ============================================================================
 */
struct ResPlayer {
    u16 values[0x14]; /* +0x00..+0x27 (offsets 0x20/0x24 alias ids 15/16) */
    s32 a;            /* +0x28 (id 17) */
    s32 b;            /* +0x2C (id 18) */
    u8 pad[0x91];     /* +0x30..+0xC0 */
    s8 index;         /* +0xC1 */
};
struct ResDefaults {
    u8 pad[0x28];
    u16 values[0x10]; /* +0x28..+0x47 */
    s32 ext15;        /* +0x48 (15*4 + 12) */
    s32 ext16;        /* +0x4C (16*4 + 12) */
};
struct ResState {
    u8 pad[0x134];
    struct ResDefaults *defaults;
};
struct ResGlobal {
    u8 pad[0xAC];
    struct ResPlayer *player;
};

static void test_80059E9C(void)
{
    struct ResPlayer p1, p_fallback;
    struct ResDefaults defs;
    struct ResState sp_state;
    struct ResGlobal p_glob;
    s32 session[0x30];
    s32 i;

    /* @leaf 80059E9C source results-stat dispatcher across low ids, session ids 12..14, s32 ids 15..18, combined sum >=19, and null-player fallbacks */
    LK_FUNC("func_80059E9C");

    memset(&p1, 0, sizeof(p1));
    memset(&p_fallback, 0, sizeof(p_fallback));
    memset(&defs, 0, sizeof(defs));
    memset(&sp_state, 0, sizeof(sp_state));
    memset(&p_glob, 0, sizeof(p_glob));
    for (i = 0; i < 0x30; i++) {
        session[i] = 1000 + i * 7;
    }

    p1.values[5] = 0xBEEF;
    defs.values[5] = 0x4321;
    *(s32 *)((u8 *)&p1 + 0x20) = -1501;
    *(s32 *)((u8 *)&p1 + 0x24) = 1602;
    p1.a = 700;
    p1.b = -250;
    p1.index = 1;

    defs.ext15 = 1555;
    defs.ext16 = -1666;
    p_fallback.a = 3000;
    p_fallback.b = 450;

    sp_state.defaults = &defs;
    p_glob.player = &p_fallback;
    gGameSession[0] = session;
    gSinglePlayerEntity[0] = &sp_state;
    gPlayerEntities[0] = &p_glob;

    /* Low id (<15, not 12..14): u16 zero-extended from player vs defaults. */
    FCK_EQ(gGetPlayerResultsStat(&p1, 5), 0xBEEF, "id 5 player u16");
    FCK_EQ(gGetPlayerResultsStat(0, 5), 0x4321, "id 5 default u16");

    /* Session ids 12/13/14: indexed by signed player->index vs null offsets 0x22/0x1A/0x20. */
    FCK_EQ(gGetPlayerResultsStat(&p1, 12), session[0x22], "id 12 player idx +1");
    FCK_EQ(gGetPlayerResultsStat(&p1, 13), session[0x1A], "id 13 player idx +1");
    FCK_EQ(gGetPlayerResultsStat(&p1, 14), session[0x20], "id 14 player idx +1");
    p1.index = -1;
    FCK_EQ(gGetPlayerResultsStat(&p1, 12), session[0x20], "id 12 player idx -1");
    FCK_EQ(gGetPlayerResultsStat(&p1, 13), session[0x18], "id 13 player idx -1");
    FCK_EQ(gGetPlayerResultsStat(&p1, 14), session[0x1E], "id 14 player idx -1");
    FCK_EQ(gGetPlayerResultsStat(0, 12), session[0x22], "id 12 null");
    FCK_EQ(gGetPlayerResultsStat(0, 13), session[0x1A], "id 13 null");
    FCK_EQ(gGetPlayerResultsStat(0, 14), session[0x20], "id 14 null");

    /* Ids 15..16: s32 at player+(id*4-28) vs defaults+(id*4+12). */
    FCK_EQ(gGetPlayerResultsStat(&p1, 15), -1501, "id 15 player");
    FCK_EQ(gGetPlayerResultsStat(&p1, 16), 1602, "id 16 player");
    FCK_EQ(gGetPlayerResultsStat(0, 15), 1555, "id 15 default");
    FCK_EQ(gGetPlayerResultsStat(0, 16), -1666, "id 16 default");

    /* Ids 17..18: player a/b vs 0 when player is null. */
    FCK_EQ(gGetPlayerResultsStat(&p1, 17), 700, "id 17 player");
    FCK_EQ(gGetPlayerResultsStat(&p1, 18), -250, "id 18 player");
    FCK_EQ(gGetPlayerResultsStat(0, 17), 0, "id 17 null returns 0");
    FCK_EQ(gGetPlayerResultsStat(0, 18), 0, "id 18 null returns 0");

    /* Ids >= 19: a + b from player or gPlayerEntities[0]->player fallback. */
    FCK_EQ(gGetPlayerResultsStat(&p1, 19), 450, "id 19 player sum");
    FCK_EQ(gGetPlayerResultsStat(0, 19), 3450, "id 19 null fallback sum");
    FCK_EQ(gGetPlayerResultsStat(&p1, 25), 450, "id 25 player sum");
}

/* ============================================================================
 * 7. 800987B8 - gSpuAccessVoiceMaskRegs
 * ============================================================================
 */
static void test_800987B8(void)
{
    u16 direct_regs[0x100];
    u32 r;

    /* @leaf 800987B8 source 24-bit split-halfword voice mask read/set/clear/replace in shadow mode (with dirty bit) and direct mode */
    LK_FUNC("func_800987B8");

    memset((void *)gSpuShadowTable, 0, sizeof(gSpuShadowTable));
    memset(direct_regs, 0, sizeof(direct_regs));
    gSpuRegBasePtr = direct_regs;

    /* Shadow mode (D_8009EE90 & 1 == 1), low=0xCC -> dirty bit = 1<<((0xCC-0xC6)>>1) = 8. */
    gSpuRegisterShadowMode = 1;
    gModeEntryWord = 0x100;
    gSpuShadowTable[0xCC] = 0x1234;
    gSpuShadowTable[0xCD] = 0xAB05; /* upper byte 0xAB preserved by mode 1/0, cleared by mode 8 */

    /* Read-only mode (2): returns 24-bit value, no writes, no dirty bit. */
    r = gSpuAccessVoiceMaskRegs(2, 0xFFFFFFU, 0xCC, 0xCD);
    FCK_EQ((s32)r, 0x051234, "shadow read 24-bit");
    FCK_EQ((s32)gModeEntryWord, 0x100, "shadow read leaves dirty word unchanged");

    /* Mode 1 (set bits), with top-byte junk in bits masked out of return. */
    r = gSpuAccessVoiceMaskRegs(1, 0xFF3000F0U, 0xCC, 0xCD);
    FCK_EQ((s32)r, 0x3512F4, "shadow set return masked to 24-bit");
    FCK_EQ(gSpuShadowTable[0xCC], 0x12F4, "shadow set low");
    FCK_EQ(gSpuShadowTable[0xCD], 0xAB35, "shadow set high preserves upper byte");
    FCK_EQ((s32)gModeEntryWord, 0x108, "shadow set marks dirty bit 3");

    /* Mode 0 (clear bits). */
    gModeEntryWord = 0;
    r = gSpuAccessVoiceMaskRegs(0, 0xFF100204U, 0xCC, 0xCD);
    FCK_EQ((s32)r, 0x2510F0, "shadow clear return");
    FCK_EQ(gSpuShadowTable[0xCC], 0x10F0, "shadow clear low");
    FCK_EQ(gSpuShadowTable[0xCD], 0xAB25, "shadow clear high preserves upper byte");
    FCK_EQ((s32)gModeEntryWord, 8, "shadow clear marks dirty bit");

    /* Mode 8 (replace): writes low and low byte of high (zeroing upper byte of high). */
    gModeEntryWord = 0;
    r = gSpuAccessVoiceMaskRegs(8, 0xFF7A9BC1U, 0xCC, 0xCD);
    FCK_EQ((s32)r, 0x7A9BC1, "shadow replace return");
    FCK_EQ(gSpuShadowTable[0xCC], 0x9BC1, "shadow replace low");
    FCK_EQ(gSpuShadowTable[0xCD], 0x007A, "shadow replace high clears upper byte");
    FCK_EQ((s32)gModeEntryWord, 8, "shadow replace marks dirty bit");

    /* Direct mode (D_8009EE90 & 1 == 0): uses gSpuRegBasePtr and does not touch gModeEntryWord. */
    gSpuRegisterShadowMode = 2; /* bit 0 clear */
    gModeEntryWord = 0x55;
    direct_regs[0xCA] = 0x00FF;
    direct_regs[0xCB] = 0xEE12;
    r = gSpuAccessVoiceMaskRegs(1, 0x040100U, 0xCA, 0xCB);
    FCK_EQ((s32)r, 0x1601FF, "direct set return");
    FCK_EQ(direct_regs[0xCA], 0x01FF, "direct set low");
    FCK_EQ(direct_regs[0xCB], 0xEE16, "direct set high");
    FCK_EQ((s32)gModeEntryWord, 0x55, "direct set skips dirty word");

    r = gSpuAccessVoiceMaskRegs(0, 0x02000FU, 0xCA, 0xCB);
    FCK_EQ((s32)r, 0x1401F0, "direct clear return");
    FCK_EQ(direct_regs[0xCA], 0x01F0, "direct clear low");
    FCK_EQ(direct_regs[0xCB], 0xEE14, "direct clear high");

    r = gSpuAccessVoiceMaskRegs(8, 0xAA654321U, 0xCA, 0xCB);
    FCK_EQ((s32)r, 0x654321, "direct replace return");
    FCK_EQ(direct_regs[0xCA], 0x4321, "direct replace low");
    FCK_EQ(direct_regs[0xCB], 0x0065, "direct replace high");
    FCK_EQ((s32)gModeEntryWord, 0x55, "direct replace skips dirty word");
}

/* ============================================================================
 * 8. 8007A450 - gSelectCrimeWar2P
 * ============================================================================
 */
struct CwMode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

static void test_8007A450(void)
{
    struct CwMode mode;

    /* @leaf 8007A450 source selector guard a0!=3, Crime War 2P field resets before callback, and one-shot movie pointer latch */
    LK_FUNC("func_8007A450");

    memset(&mode, 0, sizeof(mode));
    gGameSession[0] = &mode;

    /* Non-3 selector leaves everything untouched. */
    memset(&s_cw_reset, 0, sizeof(s_cw_reset));
    mode.players = 1;
    mode.split = 1;
    gSavedCrimeWarProgress = 9;
    gCrimeWarProgress = 8;
    gAudioOptionsMenuMode[0] = 7;
    gCrimeWarHubMenuInputMode[0] = 6;
    gLayoutPointerFlagB = 0;
    gDeferredTransition = 99;
    gPendingRowCapture = (void *)0x1234;
    gSelectCrimeWar2P(2);
    FCK_EQ(s_cw_reset.calls, 0, "a0!=3 skips callback");
    FCK_EQ(mode.players, 1, "a0!=3 keeps players");
    FCK_EQ(gDeferredTransition, 99, "a0!=3 keeps transition");

    /* First call with a0 == 3 and flag == 0: latches flag=1 and movie name pointer. */
    gSelectCrimeWar2P(3);
    FCK_EQ(s_cw_reset.calls, 1, "a0==3 calls reset");
    FCK_EQ(s_cw_reset.seen_players, 2, "players set before callback");
    FCK_EQ(s_cw_reset.seen_split, 0, "split cleared before callback");
    FCK_EQ(s_cw_reset.seen_saved, 0, "saved progress cleared before callback");
    FCK_EQ(s_cw_reset.seen_progress, 0, "progress cleared before callback");
    FCK_EQ(s_cw_reset.seen_audio_mode, 0, "audio mode cleared before callback");
    FCK_EQ(s_cw_reset.seen_hub_mode, 0, "hub mode cleared before callback");
    FCK_EQ(s_cw_reset.seen_transition, 99, "transition updated after callback");
    FCK_EQ(gDeferredTransition, 4, "transition set to 4");
    FCK_EQ(gLayoutPointerFlagB, 1, "flag latched to 1");
    FCK_CHECK(gPendingRowCapture == gCwRulesMovieName, "first call installs movie name");

    /* Second call with flag already 1: clears gPendingRowCapture to null. */
    gDeferredTransition = 11;
    gSelectCrimeWar2P(3);
    FCK_EQ(s_cw_reset.calls, 2, "second call invokes reset");
    FCK_EQ(gDeferredTransition, 4, "second call sets transition 4");
    FCK_EQ(gLayoutPointerFlagB, 1, "flag stays 1");
    FCK_CHECK(gPendingRowCapture == 0, "second call leaves pending capture null");
}

/* ============================================================================
 * 9. 8007E7A8 - gInitModeDependentState
 * ============================================================================
 */
static void test_8007E7A8(void)
{
    struct CwMode mode;
    static const u8 hex_tbl[16] = {
        'A','C','E','G','I','K','M','O','Q','S','U','W','Y','B','D','F'
    };
    static const u8 oct_tbl[8] = {
        'H','J','L','N','P','R','T','V'
    };
    /* Packed input bytes:
     * i=0 (even): (0+0)&15=0 -> 'A'-0x41 = 0
     * i=1 (odd):  (2+1)&7=3  -> 'N'-0x41 = 13
     * i=2 (even): (14+2)&15=0-> 'A'-0x41 = 0
     * i=3 (odd):  (5+3)&7=0  -> 'H'-0x41 = 7
     * i=4 (even): (11+4)&15=15->'F'-0x41 = 5
     * i=5 (odd):  (1+5)&7=6  -> 'T'-0x41 = 19
     * i=6 (even): (7+6)&15=13-> 'B'-0x41 = 1
     * i=7 (odd):  (0+7)&7=7  -> 'V'-0x41 = 21
     * i=8 (even): (9+8)&15=1 -> 'C'-0x41 = 2
     * i=9 (odd):  (4+9)&7=5  -> 'R'-0x41 = 17
     */
    static const u8 src_bytes[10] = { 0, 2, 14, 5, 11, 1, 7, 0, 9, 4 };
    static const u8 exp_pw[10]    = { 0, 13, 0, 7, 5, 19, 1, 21, 2, 17 };

    /* @leaf 8007E7A8 source alternating hex/octal password lookup transformation, cursor resets and 4-way mode state dispatch */
    LK_FUNC("func_8007E7A8");

    memcpy(gHexDigitTable, hex_tbl, 16);
    memcpy(gOctalDigitTable, oct_tbl, 8);
    gGameSession[0] = &mode;

    /* Case 1: unk5C=1, unk0C=1 -> FD=1, FB=9. */
    memset(&s_pack_pw, 0, sizeof(s_pack_pw));
    memcpy(s_pack_pw.next_bytes, src_bytes, 10);
    lk_fill(gPasswordBuffer, 0xEE, sizeof(gPasswordBuffer));
    mode.players = 1;
    mode.split = 1;
    gArmedFlag = 0;
    gScalarCursor = 77;
    gStatusTextId = 88;
    gStateByteFD[0] = 9;
    gStateByteFB[0] = 99;
    gInitModeDependentState();
    FCK_EQ(s_pack_pw.calls, 1, "calls pack helper");
    FCK_EQ(s_pack_pw.seen_armed, 1, "armed set before pack");
    FCK_EQ(s_pack_pw.seen_fd, 1, "FD set before pack for unk0C!=2");
    FCK_CHECK(memcmp(gPasswordBuffer, exp_pw, 10) == 0, "10-byte alternating table transform");
    FCK_EQ(gPasswordBuffer[10], 0xEE, "guard past 10th byte untouched");
    FCK_EQ((s32)gScalarCursor, 0, "scalar cursor cleared");
    FCK_EQ(gStatusTextId, 0, "status text cleared");
    FCK_EQ(gStateByteFB[0], 9, "split=1 players=1 -> FB=9");

    /* Case 2: unk5C=1, unk0C=2 -> FD=0, FB=3. */
    mode.players = 2;
    mode.split = 1;
    gInitModeDependentState();
    FCK_EQ(s_pack_pw.seen_fd, 0, "FD=0 when unk0C==2");
    FCK_EQ(gStateByteFD[0], 0, "FD stays 0");
    FCK_EQ(gStateByteFB[0], 3, "split=1 players=2 -> FB=3");

    /* Case 3: unk5C=0, unk0C=1 -> FD=1, FB=1. */
    mode.players = 1;
    mode.split = 0;
    gInitModeDependentState();
    FCK_EQ(gStateByteFD[0], 1, "FD=1 when unk0C==1");
    FCK_EQ(gStateByteFB[0], 1, "split=0 players=1 -> FB=1");

    /* Case 4: unk5C=0, unk0C=3 -> FD=1, FB=8. */
    mode.players = 3;
    mode.split = 0;
    gInitModeDependentState();
    FCK_EQ(gStateByteFB[0], 8, "split=0 players!=1 -> FB=8");
}

/* ============================================================================
 * 10. 8001EA20 - gSetEntityLargeRumble
 * ============================================================================
 */
struct RumbleInner {
    u8 pad[0xC1];
    s8 index;
};
struct RumbleOuter {
    u8 pad[0xAC];
    struct RumbleInner *inner;
};

static void test_8001EA20(void)
{
    struct RumbleInner inner;
    struct RumbleOuter outer;

    /* @leaf 8001EA20 source null state defaults to port 0, non-null forwards signed inner->index */
    LK_FUNC("func_8001EA20");

    memset(&s_large_motor, 0, sizeof(s_large_motor));
    memset(&inner, 0, sizeof(inner));
    memset(&outer, 0, sizeof(outer));
    outer.inner = &inner;

    gSetEntityLargeRumble(0);
    FCK_EQ(s_large_motor.calls, 1, "null calls motor");
    FCK_EQ(s_large_motor.last_index, 0, "null passes index 0");

    inner.index = 1;
    gSetEntityLargeRumble(&outer);
    FCK_EQ(s_large_motor.calls, 2, "non-null calls motor");
    FCK_EQ(s_large_motor.last_index, 1, "positive index forwarded");

    inner.index = -2;
    gSetEntityLargeRumble(&outer);
    FCK_EQ(s_large_motor.last_index, -2, "negative s8 sign-extended");
}

/* ============================================================================
 * 11. 80021AD0 - gApplySoundVoiceAttributes
 * ============================================================================
 */
struct VoiceSub {
    u8 unk00[0xA];
    u16 unk0A;
    u16 unk0C;
    u8 unk0E[2];
    u32 unk10;
};
struct VoiceArg {
    struct VoiceSub *unk00;
    u8 unk04[8];
    s32 unk0C;
};

static void test_80021AD0(void)
{
    struct VoiceSub sub;
    struct VoiceArg arg;

    /* @leaf 80021AD0 source stack SPU voice attribute packing with voice bit, fixed mask 0x70093, signed halfwords and subrecord fields */
    LK_FUNC("func_80021AD0");

    memset(&s_spu_one, 0, sizeof(s_spu_one));
    memset(&sub, 0, sizeof(sub));
    memset(&arg, 0, sizeof(arg));
    arg.unk00 = &sub;
    arg.unk0C = 5;
    sub.unk0A = 0x1234;
    sub.unk0C = 0x5678;
    sub.unk10 = 0x00018020U;

    gApplySoundVoiceAttributes(&arg, -1024, 0x3FFF, -512);
    FCK_EQ(s_spu_one.calls, 1, "calls SPU setter");
    FCK_EQ(s_spu_one.ch, 5, "channel forwarded");
    FCK_EQ(*(s32 *)(s_spu_one.attr + 0x00), (1 << 5), "voice bit mask");
    FCK_EQ(*(s32 *)(s_spu_one.attr + 0x04), 0x70093, "attribute mask");
    FCK_EQ(*(s16 *)(s_spu_one.attr + 0x08), 0x3FFF, "arg2 stored at +0x08");
    FCK_EQ(*(s16 *)(s_spu_one.attr + 0x0A), -512, "arg3 stored at +0x0A");
    FCK_EQ(*(s16 *)(s_spu_one.attr + 0x14), -1024, "arg1 stored at +0x14");
    FCK_EQ(*(s32 *)(s_spu_one.attr + 0x1C), 0x00018020, "sub->unk10 at +0x1C");
    FCK_EQ(*(s32 *)(s_spu_one.attr + 0x20), 0x00018020, "sub->unk10 at +0x20");
    FCK_EQ(*(u16 *)(s_spu_one.attr + 0x3A), 0x1234, "sub->unk0A at +0x3A");
    FCK_EQ(*(u16 *)(s_spu_one.attr + 0x3C), 0x5678, "sub->unk0C at +0x3C");

    /* High channel index (19) and distinct values. */
    arg.unk0C = 19;
    sub.unk0A = 0xBEEF;
    sub.unk0C = 0xCAFE;
    sub.unk10 = 0x0007C000U;
    gApplySoundVoiceAttributes(&arg, 0x1000, 0x2000, 0x3000);
    FCK_EQ(s_spu_one.ch, 19, "ch19 forwarded");
    FCK_EQ(*(s32 *)(s_spu_one.attr + 0x00), (1 << 19), "ch19 voice bit");
    FCK_EQ(*(u16 *)(s_spu_one.attr + 0x3A), 0xBEEF, "ch19 unk0A");
    FCK_EQ(*(u16 *)(s_spu_one.attr + 0x3C), 0xCAFE, "ch19 unk0C");
}

/* ============================================================================
 * 12. 80021BD0 - gSilenceSpuVoicesExcept
 * ============================================================================
 */
static void test_80021BD0(void)
{
    /* @leaf 80021BD0 source SPU voice mute mask ~(1<<voice)&0xFFFFFF with attribute mask 0x13 and zero volume/pitch */
    LK_FUNC("func_80021BD0");

    memset(&s_spu_multi, 0xAA, sizeof(s_spu_multi));
    s_spu_multi.calls = 0;

    gSilenceSpuVoicesExcept(0);
    FCK_EQ(s_spu_multi.calls, 1, "voice 0 calls setter");
    FCK_EQ(*(s32 *)(s_spu_multi.req + 0x00), 0xFFFFFE, "spares voice 0");
    FCK_EQ(*(s32 *)(s_spu_multi.req + 0x04), 0x13, "mask 0x13");
    FCK_EQ(*(s16 *)(s_spu_multi.req + 0x08), 0, "left 0");
    FCK_EQ(*(s16 *)(s_spu_multi.req + 0x0A), 0, "right 0");
    FCK_EQ(*(s16 *)(s_spu_multi.req + 0x14), 0, "pitch 0");

    gSilenceSpuVoicesExcept(5);
    FCK_EQ(*(s32 *)(s_spu_multi.req + 0x00), 0xFFFFDF, "spares voice 5");

    gSilenceSpuVoicesExcept(23);
    FCK_EQ(*(s32 *)(s_spu_multi.req + 0x00), 0x7FFFFF, "spares voice 23");
}

/* ============================================================================
 * 13. 8008352C - gUpdateSoundSlotTimers
 * ============================================================================
 */
struct SndSlotActor {
    u8 pad0[0x4B];
    s8 unk4B;
    u8 pad4C[0x54 - 0x4C];
    u16 unk54;
};
struct SndSlot {
    struct SndSlotActor *actor;
    s16 timer;
    s8 sound_index;
    s8 pad;
};
struct SndMgr {
    u32 mask;
    struct SndSlot slots[32];
};

static void test_8008352C(void)
{
    struct SndMgr mgr;
    struct SndSlotActor actors[6];
    s32 i;

    /* @leaf 8008352C source 32-slot sound timer loop, full-mask early exit, masked-slot timer decrement, active reset to 300, u8 truncation on stop */
    LK_FUNC("func_8008352C");

    memset(&mgr, 0, sizeof(mgr));
    memset(actors, 0, sizeof(actors));
    gMissionTable = &mgr;

    /* Full mask (0xFFFFFFFF): skips loop entirely. */
    memset(&s_slot_snd, 0, sizeof(s_slot_snd));
    mgr.mask = 0xFFFFFFFFU;
    mgr.slots[0].timer = 42;
    gUpdateSoundSlotTimers(5);
    FCK_EQ(mgr.slots[0].timer, 42, "full mask skips slot 0");
    FCK_EQ(s_slot_snd.active_calls, 0, "full mask no queries");

    /* Mask = 0xFFFFFFEA (bits 0, 2, 4 active/unmasked; bits 1, 3, 5..31 masked). */
    memset(&s_slot_snd, 0, sizeof(s_slot_snd));
    for (i = 0; i < 6; i++) {
        actors[i].unk4B = (s8)(10 + i);
        actors[i].unk54 = (u16)(0x100 + i);
        mgr.slots[i].actor = &actors[i];
        mgr.slots[i].sound_index = (s8)(i + 3);
    }
    mgr.mask = 0xFFFFFFEAU;
    mgr.slots[0].timer = 5;    /* 5 - 5 = 0 (not < 0): no query */
    mgr.slots[1].timer = 20;   /* masked: decremented to 15, no query */
    mgr.slots[2].timer = 2;    /* 2 - 5 = -3 < 0: active ret=1 -> reset 300 */
    mgr.slots[3].timer = -10;  /* masked: decremented to -15, no query */
    mgr.slots[4].timer = 1;    /* 1 - 5 = -4 < 0: active ret=0x100 -> (u8)==0 -> stop! */
    mgr.slots[5].timer = 99;   /* beyond highest 0-bit: loop stops before slot 5 */

    s_slot_snd.active_ret[0] = 1;
    s_slot_snd.active_ret[1] = 0x100; /* non-zero s32, zero u8! */

    gUpdateSoundSlotTimers(5);

    FCK_EQ(mgr.slots[0].timer, 0, "slot 0 reaches 0 without query");
    FCK_EQ(mgr.slots[1].timer, 15, "masked slot 1 still decremented");
    FCK_EQ(mgr.slots[2].timer, 300, "active slot 2 reset to 300");
    FCK_EQ(mgr.slots[3].timer, -15, "masked slot 3 still decremented");
    FCK_EQ(mgr.slots[4].timer, -4, "stopped slot 4 keeps negative timer");
    FCK_EQ(mgr.slots[5].timer, 99, "slot 5 beyond loop untouched");

    FCK_EQ(s_slot_snd.active_calls, 2, "only slots 2 and 4 queried");
    FCK_EQ(s_slot_snd.active_a0[0], 0x40000102, "slot 2 query handle");
    FCK_EQ(s_slot_snd.active_idx[0], 5, "slot 2 sound index");
    FCK_EQ(s_slot_snd.active_a0[1], 0x40000104, "slot 4 query handle");
    FCK_EQ(s_slot_snd.active_idx[1], 7, "slot 4 sound index");

    FCK_EQ(s_slot_snd.stop_calls, 1, "only slot 4 stopped (u8 truncation)");
    FCK_EQ(s_slot_snd.stop_a0[0], 0x40000104, "slot 4 stop handle");
    FCK_EQ(s_slot_snd.stop_idx[0], 7, "slot 4 stop index");
    FCK_EQ(actors[2].unk4B, 12, "active actor unk4B untouched");
    FCK_EQ(actors[4].unk4B, -1, "stopped actor unk4B set to -1");
    FCK_EQ((s32)mgr.mask, (s32)0xFFFFFFFAU, "bit 4 marked in mask, bits 0 and 2 stay clear");
}

/* ============================================================================
 * 14. 8004DDF4 - gSendCommandByTeam
 * ============================================================================
 */
struct TeamEnt {
    struct TeamEnt *next;
    void *update;
    s32 id;
    u8 unk0C[0x0C];
    u8 team;
};

static void test_8004DDF4(void)
{
    struct TeamEnt e[4];

    /* @leaf 8004DDF4 source linked-list command broadcast filtered by team and nonzero id with retry=1 */
    LK_FUNC("func_8004DDF4");

    /* Empty list */
    memset(&s_ent_cmd, 0, sizeof(s_ent_cmd));
    gUnitListHead = 0;
    gSendCommandByTeam(2, 7, 99);
    FCK_EQ(s_ent_cmd.calls, 0, "empty list sends nothing");

    /* 4 entities: match, wrong team, zero id, match */
    memset(e, 0, sizeof(e));
    e[0].next = &e[1]; e[0].team = 2; e[0].id = 11;
    e[1].next = &e[2]; e[1].team = 1; e[1].id = 22;
    e[2].next = &e[3]; e[2].team = 2; e[2].id = 0;
    e[3].next = 0;     e[3].team = 2; e[3].id = -5;
    gUnitListHead = &e[0];

    gSendCommandByTeam(2, 13, -777);
    FCK_EQ(s_ent_cmd.calls, 2, "matches exactly 2 entities");
    FCK_CHECK(s_ent_cmd.entity[0] == &e[0], "first match e[0]");
    FCK_EQ(s_ent_cmd.kind[0], 13, "first kind");
    FCK_EQ(s_ent_cmd.value[0], -777, "first value");
    FCK_EQ(s_ent_cmd.retry[0], 1, "first retry=1");
    FCK_CHECK(s_ent_cmd.entity[1] == &e[3], "second match e[3]");
    FCK_EQ(s_ent_cmd.retry[1], 1, "second retry=1");
}

/* ============================================================================
 * 15. 80025CFC - gSignalFirstTeamRetirement
 * ============================================================================
 */
static void test_80025CFC(void)
{
    u8 unit[0xB4];
    u8 data[0xE4];

    /* @leaf 80025CFC source per-team one-shot retirement flag latch and +0xDE sound suppression */
    LK_FUNC("func_80025CFC");

    memset(unit, 0, sizeof(unit));
    memset(data, 0, sizeof(data));
    *(u8 **)(unit + 0xAC) = data;

    /* First retirement for team 1, not suppressed: latches flag and plays SFX 0x18. */
    memset(&s_pos_sfx, 0, sizeof(s_pos_sfx));
    gTeamFlag[0] = 0;
    gTeamFlag[1] = 0;
    gTeamFlag[2] = 0x77;
    data[0xC1] = 1;
    data[0xDE] = 0;
    gSignalFirstTeamRetirement(unit);
    FCK_EQ(gTeamFlag[1], 1, "team 1 flag latched");
    FCK_EQ(gTeamFlag[0], 0, "team 0 flag untouched");
    FCK_EQ(gTeamFlag[2], 0x77, "team 2 flag untouched");
    FCK_EQ(s_pos_sfx.calls, 1, "plays retirement SFX");
    FCK_EQ(s_pos_sfx.id, 0x18, "SFX id 0x18");
    FCK_EQ(s_pos_sfx.a1, 0, "a1 == 0");
    FCK_EQ(s_pos_sfx.a2, 0, "a2 == 0");
    FCK_EQ(s_pos_sfx.a3, -1, "a3 == -1");

    /* Repeat call for team 1: already latched, no SFX. */
    gSignalFirstTeamRetirement(unit);
    FCK_EQ(s_pos_sfx.calls, 1, "second call does not replay SFX");

    /* Team 0 with suppression byte +0xDE != 0: latches flag, skips SFX. */
    data[0xC1] = 0;
    data[0xDE] = 1;
    gSignalFirstTeamRetirement(unit);
    FCK_EQ(gTeamFlag[0], 1, "suppressed team 0 still latches flag");
    FCK_EQ(s_pos_sfx.calls, 1, "suppressed team skips SFX");
}

/* ============================================================================
 * 16. 800217A8 - gUnloadAllSoundBanks
 * ============================================================================
 */
static void test_800217A8(void)
{
    /* @leaf 800217A8 source loops 0..bankCount-1 calling func_80021DAC and reloads bankCount each iteration */
    LK_FUNC("func_800217A8");

    /* Zero banks -> no calls. */
    memset(&s_unreg_bank, 0, sizeof(s_unreg_bank));
    s_unreg_bank.shrink_on_index = -1;
    gSoundSystem.bankCount = 0;
    gUnloadAllSoundBanks();
    FCK_EQ(s_unreg_bank.calls, 0, "zero count makes 0 calls");

    /* 3 banks -> calls 0, 1, 2 in order. */
    gSoundSystem.bankCount = 3;
    gUnloadAllSoundBanks();
    FCK_EQ(s_unreg_bank.calls, 3, "count 3 makes 3 calls");
    FCK_EQ(s_unreg_bank.indices[0], 0, "call 0 index 0");
    FCK_EQ(s_unreg_bank.indices[1], 1, "call 1 index 1");
    FCK_EQ(s_unreg_bank.indices[2], 2, "call 2 index 2");

    /* Reloads gSoundSystem.bankCount after each call (shrinking 4 -> 2 stops after index 1). */
    memset(&s_unreg_bank, 0, sizeof(s_unreg_bank));
    s_unreg_bank.shrink_on_index = 1;
    s_unreg_bank.shrink_to = 2;
    gSoundSystem.bankCount = 4;
    gUnloadAllSoundBanks();
    FCK_EQ(s_unreg_bank.calls, 2, "stops early when bankCount shrinks to 2");
    FCK_EQ(s_unreg_bank.indices[1], 1, "last call was index 1");
}

/* ============================================================================
 * 17. 800216D8 - gClearSoundStorage (calls real func_800217A8)
 * ============================================================================
 */
static void test_800216D8(void)
{
    void *dummy_banks[2];
    u8 dummy_channels[8];

    /* @leaf 800216D8 source independent banks and channels teardown, delegating to real func_800217A8 only when banks != 0 */
    LK_FUNC("func_800216D8");

    /* Both non-null: unloads 2 banks via real gUnloadAllSoundBanks and clears all 4 fields. */
    memset(&s_unreg_bank, 0, sizeof(s_unreg_bank));
    s_unreg_bank.shrink_on_index = -1;
    gSoundSystem.bankCount = 2;
    gSoundSystem.banks = dummy_banks;
    gSoundSystem.channelCount = 5;
    gSoundSystem.channels = dummy_channels;
    gClearSoundStorage();
    FCK_EQ(s_unreg_bank.calls, 2, "unloaded both banks");
    FCK_EQ(s_unreg_bank.indices[0], 0, "unloaded bank 0");
    FCK_EQ(s_unreg_bank.indices[1], 1, "unloaded bank 1");
    FCK_CHECK(gSoundSystem.banks == 0, "banks cleared");
    FCK_EQ(gSoundSystem.bankCount, 0, "bankCount cleared");
    FCK_CHECK(gSoundSystem.channels == 0, "channels cleared");
    FCK_EQ(gSoundSystem.channelCount, 0, "channelCount cleared");

    /* banks == 0, channels != 0: skips unload, leaves bankCount untouched, clears channels. */
    memset(&s_unreg_bank, 0, sizeof(s_unreg_bank));
    s_unreg_bank.shrink_on_index = -1;
    gSoundSystem.bankCount = 7;
    gSoundSystem.banks = 0;
    gSoundSystem.channelCount = 4;
    gSoundSystem.channels = dummy_channels;
    gClearSoundStorage();
    FCK_EQ(s_unreg_bank.calls, 0, "null banks skips unload");
    FCK_EQ(gSoundSystem.bankCount, 7, "null banks keeps bankCount");
    FCK_CHECK(gSoundSystem.channels == 0, "channels cleared");
    FCK_EQ(gSoundSystem.channelCount, 0, "channelCount cleared");

    /* banks != 0, channels == 0: unloads banks, leaves channelCount untouched. */
    memset(&s_unreg_bank, 0, sizeof(s_unreg_bank));
    s_unreg_bank.shrink_on_index = -1;
    gSoundSystem.bankCount = 1;
    gSoundSystem.banks = dummy_banks;
    gSoundSystem.channelCount = 9;
    gSoundSystem.channels = 0;
    gClearSoundStorage();
    FCK_EQ(s_unreg_bank.calls, 1, "unloads single bank");
    FCK_EQ(gSoundSystem.bankCount, 0, "bankCount cleared");
    FCK_EQ(gSoundSystem.channelCount, 9, "null channels keeps channelCount");
}

/* ============================================================================
 * 18. 80021998 - gReleaseSoundBankSpuHandle
 * ============================================================================
 */
struct HwSoundBank {
    u8 unk00[8];
    s32 handle;
};

static void test_80021998(void)
{
    struct HwSoundBank b0, b2;
    void *slots[3];

    /* @leaf 80021998 source indexed bank null check, SPU handle release call, and -1 invalidation */
    LK_FUNC("func_80021998");

    memset(&s_spu_claim, 0, sizeof(s_spu_claim));
    memset(&b0, 0, sizeof(b0));
    memset(&b2, 0, sizeof(b2));
    b0.handle = 0x1110;
    b2.handle = 0x12340;
    slots[0] = &b0;
    slots[1] = 0;
    slots[2] = &b2;
    gSoundSystem.bankCount = 3;
    gSoundSystem.banks = slots;

    /* Null slot does nothing. */
    gReleaseSoundBankSpuHandle(1);
    FCK_EQ(s_spu_claim.calls, 0, "null slot skips SPU release");

    /* Non-null slot passes handle and marks it -1. */
    gReleaseSoundBankSpuHandle(2);
    FCK_EQ(s_spu_claim.calls, 1, "valid slot calls SPU release");
    FCK_EQ(s_spu_claim.last_handle, 0x12340, "handle forwarded");
    FCK_EQ(b2.handle, -1, "handle invalidated to -1");
    FCK_EQ(b0.handle, 0x1110, "other bank handle untouched");
}

/* ============================================================================
 * 19. 80066684 - gDrawMinimapMarkers
 * ============================================================================
 */
static void test_80066684(void)
{
    u8 obj[0xE0];
    s32 pos[2];
    s32 i;

    /* @leaf 80066684 source minimap marker null/mask fast-exits, 8-slot 3-bit flag walk, border clipping, unsigned *5>>12 scaling and color masking */
    LK_FUNC("func_80066684");

    for (i = 0; i < 32; i++) {
        gMarkerColorTable[i] = 0x00100000 + i * 0x101;
    }
    gFrameTime = 10; /* scale = 20, offset = 2060 */
    pos[0] = 1000;
    pos[1] = 2000;

    /* Null obj -> no calls. */
    memset(&s_disc, 0, sizeof(s_disc));
    gDrawMinimapMarkers(0, pos, 5000, 4000);
    FCK_EQ(s_disc.calls, 0, "null obj emits nothing");

    /* Non-null obj with no active bit-1 in any 3-bit slot -> fast exit. */
    memset(obj, 0, sizeof(obj));
    *(u32 *)(obj + 0xA4) = 0x00249249U; /* bits 0/2 set, never bit 1 */
    gDrawMinimapMarkers(obj, pos, 5000, 4000);
    FCK_EQ(s_disc.calls, 0, "no active slot bit emits nothing");

    /* 6 configured slots testing active/inactive, X/Y clipping and negative coords:
     * slot 0: active (2), coord (6, 8)   -> x=2072, y=2096 -> drawn (v.x=2, v.y=2)
     * slot 1: inactive (5)               -> skipped
     * slot 2: active (2), coord (-3, 8)  -> x=-2536, x+2060=-476<0 -> X low clip
     * slot 3: active (2), coord (16, 8)  -> x=7192 >= 7060 -> X high clip
     * slot 4: active (2), coord (6, 16)  -> y=6192 >= 6060 -> Y high clip
     * slot 5: active (2), coord (0, 3)   -> x=-1000, y=-464 -> drawn (unsigned shift -> -2, -1)
     */
    *(u32 *)(obj + 0xA4) = (2U << 0) | (5U << 3) | (2U << 6) | (2U << 9) | (2U << 12) | (2U << 15) | (2U << 24);
    obj[0xA8 + 0] = 0x23; /* & 31 == 3 */
    obj[0xA8 + 5] = 0x47; /* & 31 == 7 */
    *(s16 *)(obj + 0xB0 + 0 * 4) = 6;  *(s16 *)(obj + 0xB2 + 0 * 4) = 8;
    *(s16 *)(obj + 0xB0 + 1 * 4) = 6;  *(s16 *)(obj + 0xB2 + 1 * 4) = 8;
    *(s16 *)(obj + 0xB0 + 2 * 4) = -3; *(s16 *)(obj + 0xB2 + 2 * 4) = 8;
    *(s16 *)(obj + 0xB0 + 3 * 4) = 16; *(s16 *)(obj + 0xB2 + 3 * 4) = 8;
    *(s16 *)(obj + 0xB0 + 4 * 4) = 6;  *(s16 *)(obj + 0xB2 + 4 * 4) = 16;
    *(s16 *)(obj + 0xB0 + 5 * 4) = 0;  *(s16 *)(obj + 0xB2 + 5 * 4) = 3;
    *(s16 *)(obj + 0xB0 + 8 * 4) = 6;  *(s16 *)(obj + 0xB2 + 8 * 4) = 8; /* past 8-slot bound */

    gDrawMinimapMarkers(obj, pos, 5000, 4000);
    FCK_EQ(s_disc.calls, 2, "exactly slots 0 and 5 drawn");

    FCK_EQ(s_disc.pos[0].x, 2, "slot 0 v.x");
    FCK_EQ(s_disc.pos[0].y, 2, "slot 0 v.y");
    FCK_EQ(s_disc.pos[0].z, 0x1000, "slot 0 v.z");
    FCK_EQ(s_disc.color[0], gMarkerColorTable[3] << 8, "slot 0 color masked & shifted");
    FCK_EQ(s_disc.a2[0], 0, "slot 0 a2");
    FCK_EQ(s_disc.a3[0], 32, "slot 0 a3");
    FCK_EQ(s_disc.a4[0], 20, "slot 0 scale=gFrameTime*2");
    FCK_EQ(s_disc.a5[0], 0xE00, "slot 0 a5");
    FCK_EQ(s_disc.a6[0], 0, "slot 0 a6");

    FCK_EQ(s_disc.pos[1].x, -2, "slot 5 negative x logical >>12");
    FCK_EQ(s_disc.pos[1].y, -1, "slot 5 negative y logical >>12");
    FCK_EQ(s_disc.pos[1].z, 0x1000, "slot 5 v.z");
    FCK_EQ(s_disc.color[1], gMarkerColorTable[7] << 8, "slot 5 color masked & shifted");

    /* Exact lower clip boundary: x + offset == 0 and y + offset == 0 are included. */
    memset(&s_disc, 0, sizeof(s_disc));
    memset(obj, 0, sizeof(obj));
    *(u32 *)(obj + 0xA4) = 2U;
    pos[0] = 2060;
    pos[1] = 2060;
    gDrawMinimapMarkers(obj, pos, 5000, 4000);
    FCK_EQ(s_disc.calls, 1, "exact x+offset==0 boundary included");
    FCK_EQ(s_disc.pos[0].x, -3, "boundary x logical >>12");
    FCK_EQ(s_disc.pos[0].y, -3, "boundary y logical >>12");
}

/* ============================================================================
 * 20. 800825AC - gValidateAtlasSpriteId
 * ============================================================================
 */
struct AtlasHdr {
    u16 type;
    u16 count;
    u32 magic;
};
struct AtlasObj {
    struct AtlasHdr *header;
};

static void test_800825AC(void)
{
    struct AtlasHdr h1, h2, h_bad_type, h_bad_magic;
    struct AtlasObj o1, o2, o_bad_type, o_bad_magic;

    /* @leaf 800825AC source prior atlas free, type==2 & magic==0x35BFBC73 validation, and rejection teardown */
    LK_FUNC("func_800825AC");

    h1.type = 2; h1.count = 4; h1.magic = 0x35BFBC73U; o1.header = &h1;
    h2.type = 2; h2.count = 8; h2.magic = 0x35BFBC73U; o2.header = &h2;
    h_bad_type.type = 3; h_bad_type.count = 4; h_bad_type.magic = 0x35BFBC73U; o_bad_type.header = &h_bad_type;
    h_bad_magic.type = 2; h_bad_magic.count = 4; h_bad_magic.magic = 0x35BFBC72U; o_bad_magic.header = &h_bad_magic;

    /* First valid install with no prior atlas. */
    memset(&s_free_blk, 0, sizeof(s_free_blk));
    gSpriteAtlasBlock = 0;
    gAtlasTable = 0;
    gValidateAtlasSpriteId(&o1);
    FCK_EQ(s_free_blk.calls, 0, "valid first install frees nothing");
    FCK_CHECK(gSpriteAtlasBlock == &o1, "o1 installed");
    FCK_CHECK(gAtlasTable == &h1, "h1 installed");

    /* Replace with second valid atlas: frees o1 first. */
    gValidateAtlasSpriteId(&o2);
    FCK_EQ(s_free_blk.calls, 1, "replacement frees prior");
    FCK_CHECK(s_free_blk.ptrs[0] == &o1, "freed o1");
    FCK_CHECK(gSpriteAtlasBlock == &o2, "o2 installed");
    FCK_CHECK(gAtlasTable == &h2, "h2 installed");

    /* Bad type: frees prior o2 and rejected o_bad_type, clears globals. */
    memset(&s_free_blk, 0, sizeof(s_free_blk));
    gValidateAtlasSpriteId(&o_bad_type);
    FCK_EQ(s_free_blk.calls, 2, "bad type frees prior and rejected");
    FCK_CHECK(s_free_blk.ptrs[0] == &o2, "freed prior o2");
    FCK_CHECK(s_free_blk.ptrs[1] == &o_bad_type, "freed rejected bad_type");
    FCK_CHECK(gSpriteAtlasBlock == 0, "atlas block cleared");
    FCK_CHECK(gAtlasTable == 0, "atlas header cleared");

    /* Bad magic with no prior atlas: frees only rejected object. */
    memset(&s_free_blk, 0, sizeof(s_free_blk));
    gValidateAtlasSpriteId(&o_bad_magic);
    FCK_EQ(s_free_blk.calls, 1, "bad magic frees rejected");
    FCK_CHECK(s_free_blk.ptrs[0] == &o_bad_magic, "freed rejected bad_magic");
    FCK_CHECK(gSpriteAtlasBlock == 0, "atlas block stays null");
    FCK_CHECK(gAtlasTable == 0, "atlas header stays null");
}

/* ============================================================================
 * 21. 8005A828 - gEmitRadarTriangle
 * ============================================================================
 */
struct RadarPolyF3 {
    u32 tag;
    u32 code;
    s16 xy[6];
};

static void test_8005A828(void)
{
    u8 pool[0x44];
    struct RadarPolyF3 *p0;
    struct RadarPolyF3 *p1;

    /* @leaf 8005A828 source POLY_F3 radar triangle emission, 2D rotation around origin+Y bias 0x2C0000, and packet cursor limit guard */
    LK_FUNC("func_8005A828");

    lk_fill(pool, 0xA5, sizeof(pool));
    gRadiusTable[0] = 10;  gRadiusTable[1] = -4;
    gRadiusTable[2] = -6;  gRadiusTable[3] = 8;
    gRadiusTable[4] = 3;   gRadiusTable[5] = 5;

    memset(&s_sincos, 0, sizeof(s_sincos));
    s_sincos.cosine = 0x00010000; /* 1.0 */
    s_sincos.sine   = 0x00008000; /* 0.5 */

    gPacketCursor = pool + 4;
    gPacketLimit  = pool + 4 + 0x1C; /* room for 1 advance */

    gEmitRadarTriangle(0x00780000, 0x0400U, 0x00112233);
    p0 = (struct RadarPolyF3 *)(pool + 4);
    FCK_EQ(s_sincos.calls, 1, "calls gSinCos");
    FCK_EQ((s32)s_sincos.last_angle, 0x0400, "passes angle");
    FCK_CHECK(gPacketCursor == pool + 4 + 0x1C, "cursor advances by 0x1C");
    FCK_EQ((s32)p0->tag, 0x04000000, "POLY_F3 tag");
    FCK_EQ((s32)p0->code, 0x20112233, "code OR 0x20000000");
    FCK_EQ(p0->xy[0], 132, "v0.x");
    FCK_EQ(p0->xy[1], 45,  "v0.y");
    FCK_EQ(p0->xy[2], 110, "v1.x");
    FCK_EQ(p0->xy[3], 49,  "v1.y");
    FCK_EQ(p0->xy[4], 120, "v2.x");
    FCK_EQ(p0->xy[5], 50,  "v2.y");
    FCK_CHECK(lk_all(pool, 0, 4, 0xA5), "leading guard untouched");

    /* At cursor == limit: packet still written at cursor, cursor does NOT advance. */
    s_sincos.cosine = 0;
    s_sincos.sine   = 0x00010000;
    gEmitRadarTriangle(0x00280000, 0x0800U, 0x00445566);
    p1 = (struct RadarPolyF3 *)(pool + 4 + 0x1C);
    FCK_CHECK(gPacketCursor == pool + 4 + 0x1C, "cursor held at limit");
    FCK_EQ((s32)p1->tag, 0x04000000, "at-limit tag written");
    FCK_EQ((s32)p1->code, 0x20445566, "at-limit code written");
    /* x = (0x280000 - 0x10000*(-4)) >> 16 = 44; y = (0x10000*10 + 0x2C0000) >> 16 = 54 */
    FCK_EQ(p1->xy[0], 44, "at-limit v0.x");
    FCK_EQ(p1->xy[1], 54, "at-limit v0.y");
    FCK_CHECK(lk_all(pool, 4 + 0x1C + 20, sizeof(pool), 0xA5), "trailing guard untouched");
}

/* ============================================================================
 * 22. 80025D6C - gSetPlayerFadeTimersByDistance
 * ============================================================================
 */
static void test_80025D6C(void)
{
    s32 ref_pos[3];
    s32 ents[4][24];
    s32 i;

    /* @leaf 80025D6C source per-player distance fade curve: full 0x168 below 0x6000, linear decay >>8, zero clamp, and 0-player guard */
    LK_FUNC("func_80025D6C");

    for (i = 0; i < 4; i++) {
        gPlayerEntities[i] = ents[i];
        gPlayerFadeTimers[i] = 0x777;
    }

    /* 0 players: no calls, timers preserved. */
    memset(&s_dist, 0, sizeof(s_dist));
    gPlayerCount = 0;
    gSetPlayerFadeTimersByDistance(ref_pos);
    FCK_EQ(s_dist.calls, 0, "0 players makes no distance calls");
    FCK_EQ(gPlayerFadeTimers[0], 0x777, "timer untouched for 0 players");

    /* 4 players covering below/exact/mid/clamped distance regimes. */
    memset(&s_dist, 0, sizeof(s_dist));
    gPlayerCount = 4;
    s_dist.ret[0] = 0x5FFF;  /* < 0x6000 -> 360 */
    s_dist.ret[1] = 0x6000;  /* exact boundary -> 360 */
    s_dist.ret[2] = 0x6580;  /* excess 0x580 >> 8 = 5 -> 355 */
    s_dist.ret[3] = 0x20000; /* excess 0x1A000 >> 8 = 416 -> clamped 0 */
    gSetPlayerFadeTimersByDistance(ref_pos);

    FCK_EQ(s_dist.calls, 4, "4 distance calls");
    FCK_CHECK(s_dist.arg_a[0] == ref_pos && s_dist.arg_b[0] == &ents[0][20], "p0 passes ref and entity+0x50");
    FCK_CHECK(s_dist.arg_a[3] == ref_pos && s_dist.arg_b[3] == &ents[3][20], "p3 passes ref and entity+0x50");
    FCK_EQ(gPlayerFadeTimers[0], 360, "below threshold gives 0x168");
    FCK_EQ(gPlayerFadeTimers[1], 360, "at threshold gives 0x168");
    FCK_EQ(gPlayerFadeTimers[2], 355, "linear decay subtracts excess>>8");
    FCK_EQ(gPlayerFadeTimers[3], 0, "far distance clamps to 0");
}

/* ============================================================================
 * 23. 80020E68 - gFindFreeCardBlockRun
 * ============================================================================
 */
static void test_80020E68(void)
{
    s32 i;

    /* @leaf 80020E68 source memory-card block rounding up to 0x2000, directory refresh, first contiguous free run, tail boundary and miss/zero cases */
    LK_FUNC("func_80020E68");

    /* Case 1: 0x4001 -> 3 blocks; run of 2 at 0..1 skipped, run of 3 at 4..6 returned. */
    memset(&s_card, 0, sizeof(s_card));
    for (i = 0; i < 16; i++) s_card.states[i] = 1;
    s_card.states[0] = 2; s_card.states[1] = 2;
    s_card.states[4] = 2; s_card.states[5] = 2; s_card.states[6] = 2;
    FCK_EQ((s32)gFindFreeCardBlockRun(0x4001U), 4, "finds 3-block run at slot 4");
    FCK_EQ(s_card.parse_calls, 1, "refreshes card directory");

    /* Case 2: exact 2-block boundary 0x4000 matches slot 0. */
    FCK_EQ((s32)gFindFreeCardBlockRun(0x4000U), 0, "exact 2-block boundary hits slot 0");
    FCK_EQ(s_card.parse_calls, 2, "refreshes directory on each call");

    /* Case 3: tail boundary: 3 blocks at slots 13, 14, 15 (bound = 16 - 3 = 13). */
    for (i = 0; i < 16; i++) s_card.states[i] = 0;
    s_card.states[13] = 2; s_card.states[14] = 2; s_card.states[15] = 2;
    FCK_EQ((s32)gFindFreeCardBlockRun(0x6000U), 13, "finds run ending at slot 15");

    /* Case 4: no qualifying run -> returns 0. */
    s_card.states[14] = 1;
    FCK_EQ((s32)gFindFreeCardBlockRun(0x6000U), 0, "miss returns 0");

    /* Case 5: size 0 -> 0 blocks -> returns 0 without reading slot states. */
    s_card.state_calls = 0;
    FCK_EQ((s32)gFindFreeCardBlockRun(0U), 0, "zero size returns 0");
    FCK_EQ(s_card.state_calls, 0, "zero size skips slot reads");
}

/* ============================================================================
 * 24. 80055EE8 - gTestCameraSightLine
 * ============================================================================
 */
static void test_80055EE8(void)
{
    struct Vec3s32 from, to;
    s32 r;

    /* @leaf 80055EE8 source ceiling midpoint bisection when from.y<=0x7FFF, two-stage raycast with 0x8000 gating, and raw result fallback */
    LK_FUNC("func_80055EE8");

    /* Case 1: below ceiling, clear first ray (bit 0x8000 clear) -> single cast, returns raw result. */
    memset(&s_ray, 0, sizeof(s_ray));
    from.x = 10; from.y = 0x2000; from.z = -30;
    to.x   = 90; to.y   = 0x4000; to.z   = 50;
    s_ray.ret[0] = 0x1234;
    r = gTestCameraSightLine(&from, &to);
    FCK_EQ(r, 0x1234, "clear first ray returns raw value");
    FCK_EQ(s_ray.calls, 1, "single raycast when bit 0x8000 clear");
    FCK_EQ(s_ray.p0[0].x, 90, "cast 1 starts at end.x");
    FCK_EQ(s_ray.p0[0].y, 0x4000, "cast 1 starts at end.y");
    FCK_EQ(s_ray.p1[0].x, 10, "cast 1 targets start.x");
    FCK_EQ(s_ray.hit_nonnull[0], 1, "cast 1 passes hit out pointer");
    FCK_EQ(s_ray.a3[0], 0, "cast 1 arg3 == 0");
    FCK_EQ(s_ray.a4[0], 1, "cast 1 flag == 1");

    /* Case 2: from.y <= 0x7FFF with mid.y > 0x8000 bisects end once, then both casts set 0x8000 -> returns 0. */
    memset(&s_ray, 0, sizeof(s_ray));
    from.x = 0;   from.y = 0x4000;  from.z = 0;
    to.x   = 400; to.y   = 0x14000; to.z   = -200;
    s_ray.ret[0] = 0x8005;
    s_ray.write_hit[0].x = 50;
    s_ray.write_hit[0].y = 0x5000;
    s_ray.write_hit[0].z = -25;
    s_ray.ret[1] = 0x8001;
    r = gTestCameraSightLine(&from, &to);
    FCK_EQ(r, 0, "both casts with 0x8000 return 0");
    FCK_EQ(s_ray.calls, 2, "two raycasts performed");
    FCK_EQ(s_ray.p0[0].x, 200, "bisected end.x");
    FCK_EQ(s_ray.p0[0].y, 0xC000, "bisected end.y");
    FCK_EQ(s_ray.p0[0].z, -100, "bisected end.z");
    FCK_EQ(s_ray.p0[1].x, 0, "second cast from start.x");
    FCK_EQ(s_ray.p1[1].x, 50, "second cast to hit.x");
    FCK_EQ(s_ray.p1[1].y, 0x5000, "second cast to hit.y");
    FCK_EQ(s_ray.hit_nonnull[1], 0, "second cast passes null hit");

    /* Case 3: first cast sets 0x8000, second cast does NOT -> returns second raw result. */
    memset(&s_ray, 0, sizeof(s_ray));
    s_ray.ret[0] = 0x8005;
    s_ray.ret[1] = 0x0042;
    r = gTestCameraSightLine(&from, &to);
    FCK_EQ(r, 0x0042, "second cast without 0x8000 returns raw result");
    FCK_EQ(s_ray.calls, 2, "two casts on partial hit");

    /* Case 4: from.y == 0x8000 (> 0x7FFF) skips bisection even though mid.y > 0x8000. */
    memset(&s_ray, 0, sizeof(s_ray));
    from.x = 0;   from.y = 0x8000;  from.z = 0;
    to.x   = 400; to.y   = 0x18000; to.z   = -200;
    s_ray.ret[0] = 0x0007;
    r = gTestCameraSightLine(&from, &to);
    FCK_EQ(r, 7, "high-from return");
    FCK_EQ(s_ray.p0[0].x, 400, "from.y>0x7FFF skips bisection x");
    FCK_EQ(s_ray.p0[0].y, 0x18000, "from.y>0x7FFF skips bisection y");

    /* Case 5: exact boundary from.y == 0x7FFF still bisects when mid.y > 0x8000. */
    memset(&s_ray, 0, sizeof(s_ray));
    from.x = 0;   from.y = 0x7FFF; from.z = 0;
    to.x   = 400; to.y   = 0x8003; to.z   = -200;
    s_ray.ret[0] = 0x0009;
    r = gTestCameraSightLine(&from, &to);
    FCK_EQ(r, 9, "boundary from.y==0x7FFF return");
    FCK_EQ(s_ray.p0[0].x, 200, "from.y==0x7FFF bisects end.x");
    FCK_EQ(s_ray.p0[0].y, 0x8001, "from.y==0x7FFF bisects end.y");
}

/* ============================================================================
 * 25. 8007C80C - gHandleControllerSetupAction (uses shared sound_mock)
 * ============================================================================
 */
static void test_8007C80C(void)
{
    /* @leaf 8007C80C source controller setup port guard, preset 0..3 wraparound + sound 0x47, diagram toggle + sound 0x46, confirm preset/rumble apply */
    LK_FUNC("func_8007C80C");

    fcm_sound_reset();
    memset(&s_ctrl_apply, 0, sizeof(s_ctrl_apply));
    gStagedControllerPresets[0] = 1;
    gStagedControllerPresets[1] = 2;
    gControllerDiagramViews[0] = 0;
    gControllerDiagramViews[1] = 0;
    gRumbleEnabled[0] = 0;
    gRumbleEnabled[1] = 1;

    /* Active port != snapshot port -> ignored for every action. */
    gActivePlayerIndex = 1;
    gSnapshotResult = 0;
    gHandleControllerSetupAction(1);
    gHandleControllerSetupAction(3);
    gHandleControllerSetupAction(4);
    FCK_EQ(gStagedControllerPresets[1], 2, "mismatched port ignores action 1");
    FCK_EQ(gControllerDiagramViews[1], 0, "mismatched port ignores action 4");
    FCK_EQ(s_ctrl_apply.preset_calls, 0, "mismatched port ignores action 3");
    FCK_EQ(fcm_sound.sfx_calls, 0, "mismatched port plays no sound");

    /* Matching port 1: action 1 steps 2 -> 3 -> 0 (wrap) with sound 0x47. */
    gSnapshotResult = 1;
    gHandleControllerSetupAction(1);
    FCK_EQ(gStagedControllerPresets[1], 3, "action 1 increments 2->3");
    FCK_EQ(fcm_sound.sfx_calls, 1, "action 1 plays SFX");
    FCK_EQ(fcm_sound.sfx_last_id, 0x47, "action 1 SFX 0x47");
    FCK_EQ(fcm_sound.sfx_last_arg1, 0, "action 1 SFX arg1 0");

    gHandleControllerSetupAction(1);
    FCK_EQ(gStagedControllerPresets[1], 0, "action 1 wraps 3->0");
    FCK_EQ(gStagedControllerPresets[0], 1, "port 0 preset untouched");

    /* Action 2 steps 0 -> 3 (wrap) -> 2 with sound 0x47. */
    gHandleControllerSetupAction(2);
    FCK_EQ(gStagedControllerPresets[1], 3, "action 2 wraps 0->3");
    FCK_EQ(fcm_sound.sfx_last_id, 0x47, "action 2 SFX 0x47");

    gHandleControllerSetupAction(2);
    FCK_EQ(gStagedControllerPresets[1], 2, "action 2 decrements 3->2");

    /* Stepping down to 0 must stay at 0 (no wrap until < 0). */
    gStagedControllerPresets[1] = 1;
    gHandleControllerSetupAction(2);
    FCK_EQ(gStagedControllerPresets[1], 0, "action 2 decrements 1->0 without wrap");
    gStagedControllerPresets[1] = 2;

    /* Actions 4 and 5 toggle diagram view and play sound 0x46. */
    gHandleControllerSetupAction(4);
    FCK_EQ(gControllerDiagramViews[1], 1, "action 4 toggles 0->1");
    FCK_EQ(fcm_sound.sfx_last_id, 0x46, "action 4 SFX 0x46");

    gHandleControllerSetupAction(5);
    FCK_EQ(gControllerDiagramViews[1], 0, "action 5 toggles 1->0");
    FCK_EQ(fcm_sound.sfx_last_id, 0x46, "action 5 SFX 0x46");

    /* Action 3 applies preset and rumble without sound. */
    fcm_sound_reset();
    gHandleControllerSetupAction(3);
    FCK_EQ(s_ctrl_apply.preset_calls, 1, "action 3 applies preset");
    FCK_EQ(s_ctrl_apply.preset_port, 1, "preset port 1");
    FCK_EQ(s_ctrl_apply.preset_val, 2, "preset value 2");
    FCK_EQ(s_ctrl_apply.rumble_calls, 1, "action 3 applies rumble");
    FCK_EQ(s_ctrl_apply.rumble_port, 1, "rumble port 1");
    FCK_EQ(s_ctrl_apply.rumble_enable, 1, "rumble value 1");
    FCK_EQ(fcm_sound.sfx_calls, 0, "action 3 plays no sound");

    /* Unknown action does nothing. */
    gHandleControllerSetupAction(6);
    FCK_EQ(s_ctrl_apply.preset_calls, 1, "action 6 no preset apply");
    FCK_EQ(fcm_sound.sfx_calls, 0, "action 6 no sound");
}

int main(void)
{
    test_80091E8C();
    test_8007E8D8();
    test_800966B0();
    test_800867B0();
    test_80048978();
    test_80059E9C();
    test_800987B8();
    test_8007A450();
    test_8007E7A8();
    test_8001EA20();
    test_80021AD0();
    test_80021BD0();
    test_8008352C();
    test_8004DDF4();
    test_80025CFC();
    test_800217A8();
    test_800216D8();
    test_80021998();
    test_80066684();
    test_800825AC();
    test_8005A828();
    test_80025D6C();
    test_80020E68();
    test_80055EE8();
    test_8007C80C();
    return lk_done("leaf_hw_free_a");
}
