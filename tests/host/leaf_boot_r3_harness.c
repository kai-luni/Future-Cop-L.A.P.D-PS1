/* Hardware-free boot/front-end tranche 3 leaf suite: 5 unchanged original MIPS
 * C units from src/game/boot-frontend/ running under qemu-mipsel.
 *
 * Nothing here is a device or hardware mock. Every global the units touch is a
 * `D_` label owned by this harness and set explicitly before each case, and
 * every callee is a recording stand-in *defined* under its retail label with
 * __asm__("func_XXXXXXXX"), so a wrong binding is a link error instead of a
 * quietly different function. No unit outside the five is linked, so nothing
 * recorded here comes from another implementation of the code under test.
 *
 *   80021E1C gFlushSoundChannelUpdates  guard + queued channel flush and masked
 *                                       SPU key release
 *   8003F79C gInitMissionSpawnState     spawn/occupancy table reset and three
 *                                       pool initialisations
 *   8004772C gPlayNextBankSample        rotating bank cursor and voice submit
 *   8004D3FC gAllocAndLinkEntity        tagged alloc, serial id, list append
 *   8001E930 gSetEntitySmallRumble      port index resolve, $a1 enable pass-through
 *
 * All five run under qemu-mipsel user mode: each one reaches its data only
 * through the `D_` labels above and its calls only through the stand-ins, so
 * there is no absolute kseg0 RAM or MMIO access to emulate. Nothing was
 * dropped from the assigned list.
 *
 * Expectations are literal values hand-worked from each unit's own body and
 * from the formulation it implements; none is produced by re-running the unit
 * or by a re-implementation. They pin the resulting counters, pointers, flag
 * bytes and the exact arguments each stand-in received.
 */
#include "leaf_kit.h"

/* ============================================================================
 * Shared helpers
 * ============================================================================ */

/* One name object per boundary so fck_record/fck_expect_order compare the same
 * pointer the stand-in stored. */
static const char key0_call[] = "spu key mode 0";
static const char key1_call[] = "spu key mode 1";
static const char attr_call[] = "apply voice attributes";
static const char tracer_call[] = "init tracer pool";
static const char effects_call[] = "init effect pools";
static const char pool_call[] = "create fixed pool";
static const char mix_call[] = "set voice mix";
static const char submit_call[] = "queue bank sample";
static const char alloc_call[] = "tagged alloc";
static const char append_call[] = "append entity";
static const char motor_call[] = "set small motor";

/* Presence plus the first three recorded arguments of the nth call to `name`.
 * Written out instead of using FCK_EQ because FCK_EQ folds its message into a
 * string literal, and these messages name the call at run time. */
static void expect_call(const char *what, const char *name, int nth,
                        s32 a0, s32 a1, s32 a2)
{
    struct FckCall c = fck_call_n(name, nth);
    s32 got[3];
    s32 want[3];
    int i;

    got[0] = c.a0; got[1] = c.a1; got[2] = c.a2;
    want[0] = a0;  want[1] = a1;  want[2] = a2;

    fck_checks++;
    if (c.name != name) {
        fck_write("FAIL: ");
        fck_write(what);
        fck_write(": call ");
        fck_write_int(nth);
        fck_write(" missing\n");
        fck_failures++;
        return;
    }
    for (i = 0; i < 3; i++) {
        fck_checks++;
        if (got[i] != want[i]) {
            fck_write("FAIL: ");
            fck_write(what);
            fck_write(": arg");
            fck_write_int(i);
            fck_write(" got ");
            fck_write_int(got[i]);
            fck_write(" want ");
            fck_write_int(want[i]);
            fck_write("\n");
            fck_failures++;
        }
    }
}

static int count_nonzero_words(const s32 *p, int n)
{
    int i, c = 0;
    for (i = 0; i < n; i++)
        if (p[i]) c++;
    return c;
}

/* ============================================================================
 * 1. 80021E1C - gFlushSoundChannelUpdates
 * ============================================================================
 */

struct BSoundChannel {
    void *unk00;
    u8 unk04[4];
    s32 unk08;
    s32 channel;
    s32 unk10;
    s32 unk14;
    s32 unk18;
};

/* Bank/channel/active/mask/queued-key words of D_800A07A8. The unit's unused
 * `gSoundChannelEnableMask` is a same-object alias of `mask` at +0x14
 * (D_800A07BC), so it is not defined separately: it would get its own address. */
struct BSoundSystem {
    s32 bankCount;
    void *banks;
    s32 channelCount;
    struct BSoundChannel *channels;
    s32 active;
    u32 mask;
    void *unk18;
};

struct BSoundSystem gSoundSystemFix __asm__("D_800A07A8");

/* Extra detail the three-argument call log cannot hold. */
static struct {
    s32 calls;
    s32 unk18[8];      /* 4th argument of each func_80021AD0 call */
    s32 status[8];     /* chan->unk08 observed *inside* the call */
} s_attr;

static void reset_attr(void)
{
    memset(&s_attr, 0, sizeof(s_attr));
}

/* func_80098C68 gSpuSetVoiceKey(s32 mode, u32 value) - SPU key on/off. */
void mock_voice_key(s32 mode, u32 value) __asm__("func_80098C68");
void mock_voice_key(s32 mode, u32 value)
{
    fck_record(mode == 0 ? key0_call : key1_call, mode, (s32)value, 0);
}

/* func_80021AD0 gApplySoundVoiceAttributes(struct Arg*, s16, s16, s16) - the
 * retail prototype: three 16-bit attributes, so the stub reads the same
 * sign-extended low half the retail callee would. */
void mock_apply_attrs(struct BSoundChannel *chan, s16 a1, s16 a2, s16 a3)
    __asm__("func_80021AD0");
void mock_apply_attrs(struct BSoundChannel *chan, s16 a1, s16 a2, s16 a3)
{
    fck_record(attr_call, (s32)chan, a1, a2);
    if (s_attr.calls < 8) {
        s_attr.unk18[s_attr.calls] = a3;
        s_attr.status[s_attr.calls] = chan->unk08;
    }
    s_attr.calls++;
}

extern void gFlushSoundChannelUpdates(void) __asm__("func_80021E1C");

static void reset_sound_system(struct BSoundChannel *chans, s32 count)
{
    int i;
    lk_fill(&gSoundSystemFix, 0, sizeof(gSoundSystemFix));
    gSoundSystemFix.bankCount = 0;
    gSoundSystemFix.banks = 0;
    gSoundSystemFix.channelCount = count;
    gSoundSystemFix.channels = chans;
    for (i = 0; i < count; i++) {
        lk_fill(&chans[i], 0, sizeof(chans[i]));
        chans[i].unk08 = 0x77;
        chans[i].unk10 = 0x1000 + i;
        chans[i].unk14 = 0x2000 + i;
        chans[i].unk18 = 0x3000 + i;
    }
    fck_log_reset();
    reset_attr();
}

static void test_80021E1C(void)
{
    struct BSoundChannel chans[3];

    /* @leaf 80021E1C source two-phase active guard, channelCount-long forward record walk masked by (1<<channel), unconditional status=2, masked key release and mask/guard clear */
    LK_FUNC("func_80021E1C");

    /* Busy guard: active != 0 returns before anything, and leaves every field
     * including the queued key and the mask untouched. */
    reset_sound_system(chans, 2);
    gSoundSystemFix.active = 5;
    gSoundSystemFix.mask = 0x00000009;
    gSoundSystemFix.unk18 = (void *)0x1111;
    gFlushSoundChannelUpdates();
    FCK_EQ(fck_log_count, 0, "busy guard makes no calls");
    FCK_EQ(gSoundSystemFix.active, 5, "busy guard preserves active");
    FCK_EQ(gSoundSystemFix.mask, 9, "busy guard preserves mask");
    FCK_EQ((u32)gSoundSystemFix.unk18, 0x1111U, "busy guard preserves queued key");
    FCK_EQ(chans[0].unk08, 0x77, "busy guard leaves channel 0 status");
    FCK_EQ(chans[1].unk08, 0x77, "busy guard leaves channel 1 status");

    /* Idle with nothing queued and an empty mask: no calls at all, and the
     * records keep their canary status bytes (the loop is mask-gated). */
    reset_sound_system(chans, 2);
    gFlushSoundChannelUpdates();
    FCK_EQ(fck_log_count, 0, "empty flush makes no calls");
    FCK_EQ(gSoundSystemFix.active, 0, "empty flush leaves active clear");
    FCK_EQ(gSoundSystemFix.mask, 0, "empty flush leaves mask clear");
    FCK_EQ(chans[0].unk08, 0x77, "empty flush leaves channel 0 status");
    FCK_EQ(chans[1].unk08, 0x77, "empty flush leaves channel 1 status");

    /* Queued key only: mode 0 key with the queued pointer, word cleared, no
     * attribute update and no channel scan. */
    reset_sound_system(chans, 2);
    gSoundSystemFix.unk18 = (void *)0x1234;
    gFlushSoundChannelUpdates();
    FCK_EQ(fck_log_count, 1, "queued key makes exactly one call");
    expect_call("queued key sent in mode 0", key0_call, 0, 0, 0x1234, 0);
    FCK_EQ((u32)gSoundSystemFix.unk18, 0U, "queued key word cleared");
    FCK_EQ(gSoundSystemFix.active, 0, "queued key leaves guard clear");
    FCK_EQ(chans[0].unk08, 0x77, "queued key does not scan channels");

    /* Full flush: mask bits 0 and 3, three records on channels 0, 3 and 2.
     * The countdown-guarded walk starts at record 0, so record 0 is updated
     * before record 1; record 2 misses the mask. The masked key release happens
     * after the walk. */
    reset_sound_system(chans, 3);
    chans[0].channel = 0;  chans[0].unk10 = 0x111; chans[0].unk14 = 0x222; chans[0].unk18 = 0x333;
    chans[1].channel = 3;  chans[1].unk10 = -2;    chans[1].unk14 = 0x444; chans[1].unk18 = 0x555;
    chans[2].channel = 2;  chans[2].unk10 = 0x666; chans[2].unk14 = 0x777; chans[2].unk18 = 0x888;
    gSoundSystemFix.mask = 0x00000009;
    gSoundSystemFix.unk18 = (void *)0xABCD;
    gFlushSoundChannelUpdates();
    {
        static const char *order[] = { key0_call, attr_call, attr_call, key1_call };
        fck_expect_order(order, 4, "key 0, walk, masked key 1");
    }
    expect_call("queued key precedes the walk", key0_call, 0, 0, 0xABCD, 0);
    expect_call("record 0 (bit 0) updated first", attr_call, 0,
                (s32)&chans[0], 0x111, 0x222);
    FCK_EQ(s_attr.unk18[0], 0x333, "record 0 forwards its +0x18 word");
    expect_call("record 1 (bit 3) updated second", attr_call, 1,
                (s32)&chans[1], -2, 0x444);
    FCK_EQ(s_attr.unk18[1], 0x555, "record 1 forwards its +0x18 word");
    FCK_EQ(s_attr.calls, 2, "record 2 (bit 2) is not updated");
    FCK_EQ(s_attr.status[0], 0x77, "status written after the attribute call");
    FCK_EQ(s_attr.status[1], 0x77, "status written after the attribute call");
    expect_call("mask released in mode 1 after the walk", key1_call, 0, 1, 9, 0);
    FCK_EQ(chans[0].unk08, 2, "visited status = 2");
    FCK_EQ(chans[1].unk08, 2, "non-matching status still = 2");
    FCK_EQ(chans[2].unk08, 2, "missed record status still = 2");
    FCK_EQ(gSoundSystemFix.mask, 0, "mask cleared after flush");
    FCK_EQ(gSoundSystemFix.active, 0, "guard cleared after flush");
    FCK_EQ((u32)gSoundSystemFix.unk18, 0U, "queued key cleared after flush");

    /* Mask with no matching channel bit: no attribute call, but the masked
     * release and the status write still happen. */
    reset_sound_system(chans, 2);
    chans[0].channel = 0;
    chans[1].channel = 2;
    gSoundSystemFix.mask = 0x00000010;
    gFlushSoundChannelUpdates();
    FCK_EQ(fck_log_count, 1, "non-matching mask makes one call");
    expect_call("mask released even when no bit matched", key1_call, 0, 1, 0x10, 0);
    FCK_EQ(chans[0].unk08, 2, "status written without a match");
    FCK_EQ(chans[1].unk08, 2, "status written without a match");

    /* Zero channel count with a live mask: the countdown loop never runs, the
     * masked release still issues. */
    reset_sound_system(chans, 3);
    chans[0].unk08 = 0x55;
    gSoundSystemFix.channelCount = 0;
    gSoundSystemFix.mask = 0x00000040;
    gFlushSoundChannelUpdates();
    FCK_EQ(fck_log_count, 1, "zero channel count makes one call");
    expect_call("mask released with zero records", key1_call, 0, 1, 0x40, 0);
    FCK_EQ(chans[0].unk08, 0x55, "zero channel count leaves records alone");
    FCK_EQ(gSoundSystemFix.mask, 0, "zero channel count clears mask");
}

/* ============================================================================
 * 2. 8003F79C - gInitMissionSpawnState
 * ============================================================================
 */

/* D_800A0C70 is a 256-entry snapshot array and D_800A7040 a 2x256 map. Each
 * fixture carries one guard word directly past the last entry the unit may
 * touch, so an off-by-one in either clear loop is visible here instead of
 * landing in unrelated RAM. */
struct BSpawnSnapshotBlock {
    s32 descs[256];
    s32 guard;
} gSpawnSnapshotFix __asm__("D_800A0C70");

struct BOccupancyMapBlock {
    s32 rows[2][256];
    s32 guard;
} gSpawnMapFix __asm__("D_800A7040");
/* The unit clears D_800A1070[0..6]; index 7 is the overrun canary. */
s32 gListHeadsFix[8] __asm__("D_800A1070");
s32 gSpawnTableRowFix __asm__("D_8009F310");
s32 gPendingSpawnListFix __asm__("D_8009F314");
void *gDeferredRegPoolFix __asm__("D_8009F70C");

static struct {
    s32 pool_calls;
    s32 row_seen;          /* D_8009F310 inside the tracer call */
    s32 row_seen_effects;  /* D_8009F310 inside the effect-pool call */
    s32 pending_seen;      /* D_8009F314 inside the pool allocation call */
    void *result[4];
} s_spawn;

static void reset_spawn(void)
{
    memset(&s_spawn, 0, sizeof(s_spawn));
}

void mock_init_tracer(void) __asm__("func_8006EDC4");
void mock_init_tracer(void)
{
    s_spawn.row_seen = gSpawnTableRowFix;
    fck_record(tracer_call, 1, 0, 0);
}

void mock_init_effects(void) __asm__("func_8005E1BC");
void mock_init_effects(void)
{
    s_spawn.row_seen_effects = gSpawnTableRowFix;
    fck_record(effects_call, 2, 0, 0);
}

/* func_800194DC gCreateFixedPool(s32 count, s32 size, s32 tag). */
void *mock_create_pool(s32 count, s32 size, s32 tag) __asm__("func_800194DC");
void *mock_create_pool(s32 count, s32 size, s32 tag)
{
    void *ret = 0;
    s_spawn.pending_seen = gPendingSpawnListFix;
    if (s_spawn.pool_calls < 4)
        ret = s_spawn.result[s_spawn.pool_calls];
    s_spawn.pool_calls++;
    fck_record(pool_call, count, size, tag);
    return ret;
}

extern void gInitMissionSpawnState(void) __asm__("func_8003F79C");

static void dirty_spawn_tables(void)
{
    int i, j;

    for (i = 0; i < 256; i++) {
        gSpawnSnapshotFix.descs[i] = 0x5A5A5A5A;
        for (j = 0; j < 2; j++)
            gSpawnMapFix.rows[j][i] = 0x6B6B6B6B;
    }
    gSpawnSnapshotFix.guard = 0x7E7E7E7E;
    gSpawnMapFix.guard = 0x7D7D7D7D;
    for (i = 0; i < 8; i++)
        gListHeadsFix[i] = 0xA0 + i;
    gSpawnTableRowFix = 0x99;
    gPendingSpawnListFix = 0x1234;
    gDeferredRegPoolFix = (void *)0xDEADBEEF;
}

static void test_8003F79C(void)
{
    /* @leaf 8003F79C source reverse clear of the 256-entry snapshot and 2x256 occupancy map, row cleared before the tracer call, tag-1 0x100x8 pool stored before the list clear, 7 list heads cleared */
    LK_FUNC("func_8003F79C");

    dirty_spawn_tables();
    fck_log_reset();
    reset_spawn();
    s_spawn.result[0] = (void *)0x8001E100;

    gInitMissionSpawnState();

    FCK_EQ(count_nonzero_words(gSpawnSnapshotFix.descs, 256), 0,
           "all 256 spawn snapshot entries cleared");
    FCK_EQ(gSpawnSnapshotFix.guard, 0x7E7E7E7E,
           "no write past the 256 snapshot entries");
    FCK_EQ(count_nonzero_words(gSpawnMapFix.rows[0], 256), 0,
           "occupancy row 0 cleared");
    FCK_EQ(count_nonzero_words(gSpawnMapFix.rows[1], 256), 0,
           "occupancy row 1 cleared");
    FCK_EQ(gSpawnMapFix.guard, 0x7D7D7D7D,
           "no write past the 2x256 occupancy map");
    FCK_EQ(gSpawnTableRowFix, 0, "spawn table row index cleared");
    FCK_EQ(gPendingSpawnListFix, 0, "pending spawn list cleared");
    FCK_EQ((u32)gDeferredRegPoolFix, 0x8001E100U, "deferred pool receives the allocation");
    FCK_EQ(gListHeadsFix[0], 0, "list head 0 cleared");
    FCK_EQ(gListHeadsFix[6], 0, "list head 6 cleared");
    FCK_EQ(gListHeadsFix[7], 0xA7, "list head 7 outside the seven is untouched");

    FCK_EQ(fck_log_count, 3, "three pool/table callees run");
    {
        static const char *order[] = { tracer_call, effects_call, pool_call };
        fck_expect_order(order, 3, "tracer, effects, then the deferred pool");
    }
    FCK_EQ(s_spawn.row_seen, 0, "row index cleared before the tracer call");
    FCK_EQ(s_spawn.row_seen_effects, 0, "row index still clear at the effect pools");
    expect_call("deferred pool is 0x100 x 8 tag 1", pool_call, 0, 0x100, 8, 1);
    FCK_EQ(s_spawn.pending_seen, 0x1234,
           "pending list cleared only after the pool is allocated");

    /* Re-run on freshly dirtied tables: the clears are unconditional and the
     * pool pointer is replaced by the new allocation, not preserved. */
    dirty_spawn_tables();
    gSpawnSnapshotFix.descs[255] = 7;
    gSpawnMapFix.rows[0][255] = 9;
    gSpawnMapFix.rows[1][0] = 11;
    gListHeadsFix[3] = 13;
    gSpawnTableRowFix = 21;
    gPendingSpawnListFix = 22;
    fck_log_reset();
    reset_spawn();
    s_spawn.result[0] = (void *)0x8001E200;

    gInitMissionSpawnState();

    FCK_EQ(gSpawnSnapshotFix.descs[255], 0, "tail snapshot entry re-cleared");
    FCK_EQ(gSpawnMapFix.rows[0][255], 0, "occupancy row 0 tail re-cleared");
    FCK_EQ(gSpawnMapFix.rows[1][0], 0, "occupancy row 1 head re-cleared");
    FCK_EQ(gSpawnSnapshotFix.guard, 0x7E7E7E7E,
           "second call stays inside the snapshot array");
    FCK_EQ(gSpawnMapFix.guard, 0x7D7D7D7D,
           "second call stays inside the occupancy map");
    FCK_EQ(gListHeadsFix[3], 0, "list head 3 re-cleared");
    FCK_EQ(gSpawnTableRowFix, 0, "row index re-cleared");
    FCK_EQ(gPendingSpawnListFix, 0, "pending list re-cleared");
    FCK_EQ((u32)gDeferredRegPoolFix, 0x8001E200U, "deferred pool replaced");
    FCK_EQ(fck_log_count, 3, "second call runs the same three callees");
    FCK_EQ(s_spawn.pool_calls, 1, "second call allocates the pool once");
}

/* ============================================================================
 * 3. 8004772C - gPlayNextBankSample
 * ============================================================================
 */

struct BSoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
};

struct BSoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
    s32 priority;
    u8 unk0C[8];
    s32 *sample;
};

static struct {
    s32 calls;
    s32 extra[4];         /* 4th argument of each func_80024358 call */
    s32 volume[4];
    s32 sample_seen[4];   /* voice->sample inside the mix call */
    s8 out_pan;
    s8 out_volume;
    s8 out_extra;
} s_voice;

static struct {
    s32 calls;
    s32 result;
} s_submit;

static void reset_voice_path(s32 result, s8 pan, s8 volume, s8 extra)
{
    memset(&s_voice, 0, sizeof(s_voice));
    s_voice.out_pan = pan;
    s_voice.out_volume = volume;
    s_voice.out_extra = extra;
    memset(&s_submit, 0, sizeof(s_submit));
    s_submit.result = result;
    fck_log_reset();
}

/* func_800477D8 gSetSoundVoiceMixParameters(voice, pan, volume, extra) - writes
 * the resolved mix into the voice; the stub writes the values the test chose so
 * the follow-up submit can be shown to read the *stored* fields. */
void mock_set_mix(struct BSoundVoice *voice, s32 pan, s32 volume, s32 extra)
    __asm__("func_800477D8");
void mock_set_mix(struct BSoundVoice *voice, s32 pan, s32 volume, s32 extra)
{
    fck_record(mix_call, (s32)voice, pan, volume);
    if (s_voice.calls < 4)
        s_voice.sample_seen[s_voice.calls] = (s32)voice->sample;
    s_voice.calls++;
    (void)extra;
    voice->pan = s_voice.out_pan;
    voice->volume = s_voice.out_volume;
    voice->extra = s_voice.out_extra;
}

/* func_80024358 gQueueVoiceBankSample(bank, voice, pan, volume, extra) -> s32.
 * The unit declares a u8 return, so a word result above 0xFF is truncated by
 * the unit, not by this stand-in. */
s32 mock_queue_bank_sample(struct BSoundBank *bank, struct BSoundVoice *voice,
                           s32 pan, s32 volume, s32 extra)
    __asm__("func_80024358");
s32 mock_queue_bank_sample(struct BSoundBank *bank, struct BSoundVoice *voice,
                           s32 pan, s32 volume, s32 extra)
{
    s32 n = s_voice.calls - 1;
    fck_record(submit_call, (s32)bank, (s32)voice, pan);
    if (n >= 0 && n < 4) {
        s_voice.volume[n] = volume;
        s_voice.extra[n] = extra;
    }
    return s_submit.result;
}

extern u8 gPlayNextBankSample(struct BSoundBank *bank, struct BSoundVoice *voice,
                              s32 pan, s32 volume, s32 extra)
    __asm__("func_8004772C");

static void fill_bank(struct BSoundBank *bank, u8 count, u8 cursor)
{
    bank->samples[0] = 0x0A0A0A0A;
    bank->samples[1] = 0x0B0B0B0B;
    bank->samples[2] = 0x0C0C0C0C;
    bank->samples[3] = 0x0D0D0D0D;
    bank->count = count;
    bank->cursor = cursor;
}

static void test_8004772C(void)
{
    struct BSoundBank bank;
    struct BSoundVoice voice;
    u8 result;

    /* @leaf 8004772C source empty-bank reject, cursor advance/wrap at the count, sample pointer from the cursor, mix before submit on the stored voice fields, u8 result */
    LK_FUNC("func_8004772C");

    /* Empty bank: no cursor move, no mix, no submit, result 0, voice untouched. */
    lk_fill(&bank, 0, sizeof(bank));
    lk_fill(&voice, 0, sizeof(voice));
    fill_bank(&bank, 0, 2);
    voice.pan = 0x11;
    voice.volume = 0x22;
    voice.extra = 0x33;
    voice.sample = (s32 *)0x1234;
    reset_voice_path(0x7777, -3, 100, 7);
    result = gPlayNextBankSample(&bank, &voice, 0x41, 0x42, 0x43);
    FCK_EQ(result, 0, "empty bank returns 0");
    FCK_EQ(fck_log_count, 0, "empty bank calls nothing");
    FCK_EQ(bank.cursor, 2, "empty bank keeps the cursor");
    FCK_EQ((u32)voice.sample, 0x1234U, "empty bank keeps the voice sample");
    FCK_EQ(voice.pan, 0x11, "empty bank keeps the voice pan");

    /* Middle of a 3-sample bank: cursor 1 -> 2, sample[2], mix gets the caller
     * arguments, submit gets the values the mix stored in the voice. */
    lk_fill(&voice, 0, sizeof(voice));
    fill_bank(&bank, 3, 1);
    voice.bank = 0x33;
    voice.priority = 0x44;
    voice.pan = 0x55;
    voice.volume = 0x66;
    voice.extra = 0x77;
    voice.sample = (s32 *)0x1234;
    reset_voice_path(0x1234, -3, 100, 7);
    result = gPlayNextBankSample(&bank, &voice, 0x11, 0x22, 0x33);
    FCK_EQ(result, 0x34, "result is the low byte of the submit return");
    FCK_EQ(bank.cursor, 2, "cursor advances to 2");
    FCK_EQ((u32)voice.sample, (u32)&bank.samples[2], "sample points at samples[2]");
    FCK_EQ(voice.pan, -3, "mix result stored in the voice pan");
    FCK_EQ(voice.volume, 100, "mix result stored in the voice volume");
    FCK_EQ(voice.extra, 7, "mix result stored in the voice extra");
    FCK_EQ(voice.bank, 0x33, "voice bank untouched");
    FCK_EQ(voice.priority, 0x44, "voice priority untouched");
    {
        static const char *order[] = { mix_call, submit_call };
        fck_expect_order(order, 2, "mix then submit");
    }
    expect_call("mix receives the caller arguments", mix_call, 0,
                (s32)&voice, 0x11, 0x22);
    FCK_EQ(s_voice.sample_seen[0], (s32)&bank.samples[2],
           "sample stored before the mix call");
    expect_call("submit receives the bank and voice", submit_call, 0,
                (s32)&bank, (s32)&voice, -3);
    FCK_EQ(s_voice.volume[0], 100, "submit volume is the stored voice volume");
    FCK_EQ(s_voice.extra[0], 7, "submit extra is the stored voice extra");

    /* Cursor 2 of 3 wraps to 0 before the sample pointer is taken. */
    lk_fill(&voice, 0, sizeof(voice));
    fill_bank(&bank, 3, 2);
    reset_voice_path(0, -1, -128, -3);
    result = gPlayNextBankSample(&bank, &voice, 5, 6, 7);
    FCK_EQ(result, 0, "zero submit result stays 0");
    FCK_EQ(bank.cursor, 0, "cursor wraps to 0 at the count");
    FCK_EQ((u32)voice.sample, (u32)&bank.samples[0], "wrapped sample is samples[0]");
    expect_call("submit pan is the stored negative pan", submit_call, 0,
                (s32)&bank, (s32)&voice, -1);
    FCK_EQ(s_voice.volume[0], -128, "submit volume keeps the s8 minimum");
    FCK_EQ(s_voice.extra[0], -3, "submit extra keeps the negative value");

    /* Single-sample bank at cursor 0: 1 >= 1 wraps immediately. */
    lk_fill(&voice, 0, sizeof(voice));
    fill_bank(&bank, 1, 0);
    reset_voice_path(0x00FF, 0, 0, 0);
    result = gPlayNextBankSample(&bank, &voice, 0, 0, 0);
    FCK_EQ(result, 0xFF, "0x00FF submit result survives the u8 return");
    FCK_EQ(bank.cursor, 0, "single-sample cursor stays 0");
    FCK_EQ((u32)voice.sample, (u32)&bank.samples[0], "single-sample pointer is samples[0]");

    /* Cursor 254 of 255: 255 >= 255 wraps (equality is inside the wrap). */
    lk_fill(&voice, 0, sizeof(voice));
    fill_bank(&bank, 255, 254);
    reset_voice_path(0x100, 1, 1, 1);
    result = gPlayNextBankSample(&bank, &voice, 0, 0, 0);
    FCK_EQ(result, 0, "0x100 truncates to 0 through the u8 return");
    FCK_EQ(bank.cursor, 0, "cursor 255 of 255 wraps to 0");
    FCK_EQ((u32)voice.sample, (u32)&bank.samples[0], "wrapped sample is samples[0]");
}

/* ============================================================================
 * 4. 8004D3FC - gAllocAndLinkEntity
 * ============================================================================
 */

struct BEntity {
    struct BEntity *next;
    s32 (*update)(s32 message, struct BEntity *entity, s32 arg2, s32 arg3);
    s32 id;
};

s32 gEntityIdCounterFix __asm__("D_8009F7A0");

static struct {
    s32 calls;
    void *result[4];
} s_entity;

static void reset_entity_path(void)
{
    memset(&s_entity, 0, sizeof(s_entity));
    fck_log_reset();
}

/* func_80017EBC gAllocMemory(s32 size, s32 tag) - tagged RAM allocator. */
void *mock_alloc_memory(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc_memory(s32 size, s32 tag)
{
    void *ret = 0;
    if (s_entity.calls < 4)
        ret = s_entity.result[s_entity.calls];
    fck_record(alloc_call, size, tag, 0);
    s_entity.calls++;
    return ret;
}

/* func_8004D3D4 gAppendEntityToList(struct Entity*) - records the pointer and
 * the id already stamped in the block when it was linked. */
void mock_append_entity(struct BEntity *entity) __asm__("func_8004D3D4");
void mock_append_entity(struct BEntity *entity)
{
    fck_record(append_call, (s32)entity, entity->id, 0);
}

extern struct BEntity *gAllocAndLinkEntity(s32 size) __asm__("func_8004D3FC");

static void test_8004D3FC(void)
{
    struct BEntity block;
    struct BEntity block2;
    struct BEntity *ret;

    /* @leaf 8004D3FC source size and tag 3 forwarded unchanged, id = pre-incremented serial stored at +0x8 before the append, block returned */
    LK_FUNC("func_8004D3FC");

    /* First allocation: size 0xD4, serial 0x41 -> id 0x42, linked after the id. */
    lk_fill(&block, 0, sizeof(block));
    block.next = (struct BEntity *)0xCAFE;
    gEntityIdCounterFix = 0x41;
    reset_entity_path();
    s_entity.result[0] = &block;

    ret = gAllocAndLinkEntity(0xD4);

    FCK_EQ((u32)ret, (u32)&block, "returns the allocated block");
    FCK_EQ(block.id, 0x42, "id is the pre-incremented serial");
    FCK_EQ(gEntityIdCounterFix, 0x42, "counter advanced to 0x42");
    FCK_EQ((u32)block.next, 0xCAFEU, "unit does not touch the block's next word");
    FCK_EQ(fck_log_count, 2, "allocation then append");
    {
        static const char *order[] = { alloc_call, append_call };
        fck_expect_order(order, 2, "allocate before linking");
    }
    expect_call("allocator gets the size unchanged and tag 3", alloc_call, 0,
                0xD4, 3, 0);
    expect_call("append gets the block with its id already stored", append_call, 0,
                (s32)&block, 0x42, 0);

    /* Negative size is forwarded bit-for-bit; the serial wraps from 0xFFFFFFFF
     * to 0, so the appended id is 0. */
    lk_fill(&block, 0, sizeof(block));
    gEntityIdCounterFix = 0xFFFFFFFE;
    reset_entity_path();
    s_entity.result[0] = &block;

    ret = gAllocAndLinkEntity(-1);

    FCK_EQ((u32)ret, (u32)&block, "wrapped-serial block returned");
    FCK_EQ(block.id, 0xFFFFFFFF, "id 0xFFFFFFFF on the boundary");
    FCK_EQ(gEntityIdCounterFix, 0xFFFFFFFF, "counter holds 0xFFFFFFFF");
    expect_call("allocator gets -1 unchanged", alloc_call, 0, -1, 3, 0);
    expect_call("append sees the boundary id", append_call, 0,
                (s32)&block, 0xFFFFFFFF, 0);

    /* Two calls in a row: distinct returned blocks, distinct serials, append
     * called once per allocation with the matching block. */
    lk_fill(&block, 0, sizeof(block));
    lk_fill(&block2, 0, sizeof(block2));
    gEntityIdCounterFix = 6;
    reset_entity_path();
    s_entity.result[0] = &block;
    s_entity.result[1] = &block2;

    ret = gAllocAndLinkEntity(0x10);
    FCK_EQ((u32)ret, (u32)&block, "first call returns block 1");
    ret = gAllocAndLinkEntity(0x20);
    FCK_EQ((u32)ret, (u32)&block2, "second call returns block 2");

    FCK_EQ(block.id, 7, "first id 7");
    FCK_EQ(block2.id, 8, "second id 8");
    FCK_EQ(gEntityIdCounterFix, 8, "counter ends at 8");
    FCK_EQ(fck_log_count, 4, "two allocations and two appends");
    expect_call("first allocation size 0x10", alloc_call, 0, 0x10, 3, 0);
    expect_call("second allocation size 0x20", alloc_call, 1, 0x20, 3, 0);
    expect_call("first append", append_call, 0, (s32)&block, 7, 0);
    expect_call("second append", append_call, 1, (s32)&block2, 8, 0);
}

/* ============================================================================
 * 5. 8001E930 - gSetEntitySmallRumble
 * ============================================================================
 */

struct BInnerState {
    u8 pad[0xC1];
    s8 index;
};

struct BOuterState {
    u8 pad[0xAC];
    struct BInnerState *inner;
};

/* func_8001E9AC gSetSmallMotor(s32 port, s32 enable). The unit passes only the
 * resolved port; $a1 is the enable word the caller supplied, which the unit's
 * code never writes (confirmed against its emitted body: only a0 is assigned
 * before the call). The two-argument prototype is the retail one, so recording
 * a1 measures the pass-through instead of assuming it. */
void mock_small_motor(s32 port, s32 enable) __asm__("func_8001E9AC");
void mock_small_motor(s32 port, s32 enable)
{
    fck_record(motor_call, port, enable, 0);
}

extern void gSetEntitySmallRumble(struct BOuterState *state) __asm__("func_8001E930");

/* The unit takes one argument; calling through this typed pointer lets the test
 * control the otherwise-unused $a1 word. */
typedef void (*RumbleFn)(struct BOuterState *state, s32 enable);

static void test_8001E930(void)
{
    struct BOuterState outer;
    struct BInnerState inner;
    RumbleFn call = (RumbleFn)(void (*)(struct BOuterState *))gSetEntitySmallRumble;

    /* @leaf 8001E930 source signed port index from inner+0xC1 else 0, single motor call with the caller's $a1 enable word unchanged */
    LK_FUNC("func_8001E930");

    /* Port 3 with the small actuator enabled. */
    lk_fill(&outer, 0, sizeof(outer));
    lk_fill(&inner, 0, sizeof(inner));
    outer.inner = &inner;
    inner.index = 3;
    fck_log_reset();
    call(&outer, 1);
    FCK_EQ(fck_log_count, 1, "one motor call per invocation");
    expect_call("port 3 enabled", motor_call, 0, 3, 1, 0);

    /* Negative (sign-extended) port index and a disabled actuator. */
    inner.index = -2;
    fck_log_reset();
    call(&outer, 0);
    FCK_EQ(fck_log_count, 1, "disabled call still reaches the motor");
    expect_call("signed port -2 disabled", motor_call, 0, -2, 0, 0);

    /* Largest positive port index. */
    inner.index = 127;
    fck_log_reset();
    call(&outer, 0x1234);
    expect_call("port 127 with the enable word 0x1234", motor_call, 0, 127, 0x1234, 0);

    /* Null state falls back to port 0 and still forwards a negative enable. */
    fck_log_reset();
    call(0, -1);
    FCK_EQ(fck_log_count, 1, "null state still calls the motor");
    expect_call("null state uses port 0", motor_call, 0, 0, -1, 0);

    /* The inner state is only read: its index must survive the call. */
    lk_fill(&inner, 0, sizeof(inner));
    inner.index = 9;
    fck_log_reset();
    call(&outer, 1);
    FCK_EQ(inner.index, 9, "inner index is not modified");
    expect_call("port 9 read from the inner state", motor_call, 0, 9, 1, 0);
}

int main(void)
{
    test_80021E1C();
    test_8003F79C();
    test_8004772C();
    test_8004D3FC();
    test_8001E930();
    return lk_done("leaf_boot_r3");
}
