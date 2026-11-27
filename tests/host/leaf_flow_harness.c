/* Gameplay/flow helpers that touch no hardware: audio voice bookkeeping,
 * front-end volume flow, sound-event dispatch and scene-pool lifecycle.
 *
 * Every fixture a check depends on is written before the call; nothing relies
 * on retail RAM or zero-initialised slots. Callees are stand-ins bound to
 * their retail label, so a wrong binding is a link error. A stand-in only
 * records its arguments and returns a value the test configured - it never
 * invents behaviour. Two notes on the boundaries:
 *
 *   - func_8001A578 is an identity rotation: the unit is handed the delta
 *     vector it is about to clamp, and the clamp arithmetic is the property
 *     under test here (the rotation has its own contract elsewhere).
 *   - units that are themselves under test are linked for real, so a few
 *     checks intentionally observe them through their own callees
 *     (800452D4 -> 8004530C -> 80047800, 800474F0 -> 80046A34).
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
/* @mock sound_mock */
/* func_800452C8 (voice-volume getter) and func_80046C84 (SE trigger / voice
 * lookup) come from the shared mock library: configure fcm_sound.voice_volume
 * and fcm_sound.sfx_result, read the SE calls back from fcm_sound.sfx_*. */
#include "sound_mock.h"

/* --------------------------------------------------------------- recorder */

#define LF_CAP_MAX 32

#define LF_SLOT       3   /* func_80045164 find tracked slot     */
#define LF_ATTEN      4   /* func_800458E8 spatial attenuation   */
#define LF_MIXER      6   /* func_80024730 apply channel volume  */
#define LF_SETMIX     9   /* func_800477D8 set voice mix fields  */
#define LF_MIXAPPLY  10   /* func_800243C4 apply mix to mixer    */
#define LF_RESETBANK 11   /* func_80044EF8 reset bank tables     */
#define LF_INITCHAN  12   /* func_8004582C init channel list     */
#define LF_RELEASE   14   /* func_80046698 release voice         */
#define LF_RESOLVE   15   /* func_80045494 resolve sound id      */
#define LF_ALLOCPRI  16   /* func_80046B04 alloc priority voice  */
#define LF_STOPSTREAM 18  /* func_800243F8 stop stream handle    */
#define LF_PLAYON    19   /* func_80046E88 play id on voice      */
#define LF_FREE      20   /* func_800181D4 free block            */
#define LF_FREEPOOL  21   /* func_80019584 free fixed pool       */
#define LF_ALLOC     22   /* func_80017EBC tagged allocation     */
#define LF_POOL      23   /* func_800194DC fixed pool creation   */
#define LF_QUAD      24   /* func_80082EF0 flat quad rect        */
#define LF_ROT       25   /* func_8001A578 rotate 2D vector      */

struct LfCall {
    int id;
    s32 a0, a1, a2, a3, a4, a5, a6, a7;
};

static struct LfCall lf_calls[LF_CAP_MAX];
static int lf_calls_n;

static void lf_reset(void)
{
    int i;
    s32 sfx_result = fcm_sound.sfx_result, voice_volume = fcm_sound.voice_volume;
    /* Clear the shared mock's call log but keep the configured return values,
     * as the private stand-ins' statics were kept before. */
    fcm_sound_reset();
    fcm_sound.sfx_result = sfx_result;
    fcm_sound.voice_volume = voice_volume;
    lf_calls_n = 0;
    for (i = 0; i < LF_CAP_MAX; i++) {
        lf_calls[i].id = 0;
        lf_calls[i].a0 = lf_calls[i].a1 = lf_calls[i].a2 = lf_calls[i].a3 = 0;
        lf_calls[i].a4 = lf_calls[i].a5 = lf_calls[i].a6 = lf_calls[i].a7 = 0;
    }
}

static void lf_rec(int id, s32 a0, s32 a1, s32 a2, s32 a3,
                   s32 a4, s32 a5, s32 a6, s32 a7)
{
    if (lf_calls_n >= LF_CAP_MAX) return;   /* overflow is a test bug */
    lf_calls[lf_calls_n].id = id;
    lf_calls[lf_calls_n].a0 = a0;
    lf_calls[lf_calls_n].a1 = a1;
    lf_calls[lf_calls_n].a2 = a2;
    lf_calls[lf_calls_n].a3 = a3;
    lf_calls[lf_calls_n].a4 = a4;
    lf_calls[lf_calls_n].a5 = a5;
    lf_calls[lf_calls_n].a6 = a6;
    lf_calls[lf_calls_n].a7 = a7;
    lf_calls_n++;
}

/* ------------------------------------------------------------------ state */

/* Pointers and scalars the units share. Types follow the strictest reader. */
u8  lf_volume_limit __asm__("D_8009F738");
s32 lf_sound_mode __asm__("D_8009F730");
s32 lf_music_volume __asm__("D_8009F334");
void *lf_priority_list __asm__("D_8009F744");
void *lf_priority_count __asm__("D_8009F748");
void *lf_voice_list __asm__("D_8009F74C");
void *lf_class0_count __asm__("D_8009F750");
void *lf_class2_list __asm__("D_8009F754");
void *lf_class2_count __asm__("D_8009F758");
void *lf_free_list __asm__("D_8009F73C");
s32 lf_free_count __asm__("D_8009F740");
s32 lf_pending_release __asm__("D_8009F32C");
s32 lf_spatial_mode __asm__("D_8009F734");
s32 lf_frame_tick __asm__("D_8009FA2C");
u8  lf_shade_table[8] __asm__("D_8009F3F8");
u8  lf_bank_low[8] __asm__("D_800A1830");
void *lf_mission_state __asm__("D_8009F934");
void *lf_bucket_grid __asm__("D_8009F938");
void *lf_mission_table __asm__("D_8009F930");
s32 lf_mission_clear __asm__("D_8009F944");

struct LfPosAngle {
    s32 position[3];
    s32 angle;
};
struct LfPosAngle lf_pos_angles[2] __asm__("D_800A1920");

/* 800453B8's voice node: 0x0D, index, 2, work word, 8, link, 0x0C = 0x2C. */
struct LfVoicePool {
    u8 unk00[0x0D];
    u8 index;
    u8 unk0E[2];
    s32 unk10;
    u8 unk14[8];
    struct LfVoicePool *next;
    u8 unk20[0x0C];
};
struct LfVoicePool lf_voice_pool[24] __asm__("D_800AACC8");

struct LfBank {
    s32 unk00;
    u8 unk04[0x10];
};
struct LfBank lf_banks[6] __asm__("D_800A10E0");

/* Shared by 800453B8 and 80047088: three words, playing/track/handle. */
struct LfStream {
    s32 playing;
    s32 track;
    s32 handle;
};
struct LfStream lf_streams[2] __asm__("D_800A1810");

/* ------------------------------------------------------------ stand-ins */

static s32 lf_atten_ok, lf_atten_vol;
static s32 lf_resolve_slot;
static s32 lf_alloc_base, lf_alloc_seq;
static s32 lf_pending_at_stream;
static void *lf_pool_ret;
static void *lf_alloc_ptr_ret;
static s32 lf_table_buffer[0x41];

struct LfSlot450A4 {
    u8 unk00[0x50];
    s32 unk50;
    u8 unk54[4];
    s32 unk58;
};
static struct LfSlot450A4 lf_slot_450A4;
static struct LfSlot450A4 *lf_slot_ptr;

struct LfSlot450A4 *lf_stub_45164(s32 id) __asm__("func_80045164");
struct LfSlot450A4 *lf_stub_45164(s32 id)
{
    lf_rec(LF_SLOT, id, 0, 0, 0, 0, 0, 0, 0);
    return lf_slot_ptr;
}

s32 lf_stub_458E8(s32 a0, s32 a1, s8 *mix, s32 a3) __asm__("func_800458E8");
s32 lf_stub_458E8(s32 a0, s32 a1, s8 *mix, s32 a3)
{
    lf_rec(LF_ATTEN, a0, a1, (s32)mix, a3, 0, 0, 0, 0);
    if (lf_atten_ok) mix[0] = (s8)lf_atten_vol;
    return lf_atten_ok;
}

void lf_stub_24730(s32 volume) __asm__("func_80024730");
void lf_stub_24730(s32 volume)
{
    lf_rec(LF_MIXER, volume, 0, 0, 0, 0, 0, 0, 0);
}

void lf_stub_477D8(void *voice, s32 pan, s32 volume, s32 extra) __asm__("func_800477D8");
void lf_stub_477D8(void *voice, s32 pan, s32 volume, s32 extra)
{
    lf_rec(LF_SETMIX, (s32)voice, pan, volume, extra, 0, 0, 0, 0);
}

void lf_stub_243C4(void *voice, s32 pan, s32 volume, s32 extra) __asm__("func_800243C4");
void lf_stub_243C4(void *voice, s32 pan, s32 volume, s32 extra)
{
    lf_rec(LF_MIXAPPLY, (s32)voice, pan, volume, extra, 0, 0, 0, 0);
}

void lf_stub_44EF8(void) __asm__("func_80044EF8");
void lf_stub_44EF8(void)
{
    lf_rec(LF_RESETBANK, 0, 0, 0, 0, 0, 0, 0, 0);
}

void lf_stub_4582C(void) __asm__("func_8004582C");
void lf_stub_4582C(void)
{
    lf_rec(LF_INITCHAN, 0, 0, 0, 0, 0, 0, 0, 0);
}

void lf_stub_46698(s32 handle) __asm__("func_80046698");
void lf_stub_46698(s32 handle)
{
    lf_rec(LF_RELEASE, handle, 0, 0, 0, 0, 0, 0, 0);
}

s32 lf_stub_45494(s32 id, u8 *table, s32 limit) __asm__("func_80045494");
s32 lf_stub_45494(s32 id, u8 *table, s32 limit)
{
    lf_rec(LF_RESOLVE, id, (s32)table, limit, 0, 0, 0, 0, 0);
    return lf_resolve_slot;
}

s32 lf_stub_46B04(s32 a0, s32 a1, s32 a2, s32 slot) __asm__("func_80046B04");
s32 lf_stub_46B04(s32 a0, s32 a1, s32 a2, s32 slot)
{
    lf_rec(LF_ALLOCPRI, a0, a1, a2, slot, 0, 0, 0, 0);
    lf_pending_at_stream = lf_pending_release;
    return lf_alloc_base + lf_alloc_seq++;
}

void lf_stub_243F8(s8 *handle) __asm__("func_800243F8");
void lf_stub_243F8(s8 *handle)
{
    lf_rec(LF_STOPSTREAM, (s32)handle, 0, 0, 0, 0, 0, 0, fcm_sound.sfx_calls);
}

void lf_stub_46E88(s32 track, s32 a1, s32 a2, s32 a3, s32 handle,
                   void *object) __asm__("func_80046E88");
void lf_stub_46E88(s32 track, s32 a1, s32 a2, s32 a3, s32 handle, void *object)
{
    lf_rec(LF_PLAYON, track, a1, a2, a3, handle, (s32)object, 0, fcm_sound.sfx_calls);
}

void lf_stub_181D4(void *block) __asm__("func_800181D4");
void lf_stub_181D4(void *block)
{
    lf_rec(LF_FREE, (s32)block, 0, 0, 0, 0, 0, 0, 0);
}

void lf_stub_19584(s32 pool) __asm__("func_80019584");
void lf_stub_19584(s32 pool)
{
    lf_rec(LF_FREEPOOL, pool, 0, 0, 0, 0, 0, 0, 0);
}

void *lf_stub_17EBC(s32 size, s32 tag) __asm__("func_80017EBC");
void *lf_stub_17EBC(s32 size, s32 tag)
{
    lf_rec(LF_ALLOC, size, tag, 0, 0, 0, 0, 0, 0);
    return lf_alloc_ptr_ret;
}

void *lf_stub_194DC(s32 count, s32 size, s32 tag) __asm__("func_800194DC");
void *lf_stub_194DC(s32 count, s32 size, s32 tag)
{
    lf_rec(LF_POOL, count, size, tag, 0, 0, 0, 0, 0);
    return lf_pool_ret;
}

void lf_stub_82EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g,
                   s32 b) __asm__("func_80082EF0");
void lf_stub_82EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b)
{
    lf_rec(LF_QUAD, x, y, w, h, (s32)link, r, g, b);
}

/* Identity: the unit is handed the delta vector it is about to clamp, and the
 * clamp arithmetic is what this suite checks. The rotation itself belongs to
 * func_8001A578's own contract. */
void lf_stub_1A578(u32 angle, s32 *vec) __asm__("func_8001A578");
void lf_stub_1A578(u32 angle, s32 *vec)
{
    lf_rec(LF_ROT, (s32)angle, (s32)vec, 0, 0, 0, 0, 0, 0);
}

/* -------------------------------------------------------- tested prototypes */

s8 gComputeTrackedVoiceVolume(void) __asm__("func_80045050");
s32 gComputeTrackedSoundMix(s32 id, s8 *mix) __asm__("func_800450A4");
void gSetStereoSoundMode(s32 enable) __asm__("func_800451B4");
void gSetVagmVolumeSetting(s32 volume) __asm__("func_80045238");
void gEnableHalfVolumeForSoundVoices(void) __asm__("func_800452D4");
void gClearVolumeLimitAndRefreshVoices(void) __asm__("func_80045370");
void gInitSoundPools(void) __asm__("func_800453B8");
void gComputeSpatialSoundPan(s8 *mix) __asm__("func_800460D4");
void gStopSoundVoiceHandle(s32 handle) __asm__("func_80046A34");
s32 gAllocVoiceForSoundId(s32 arg0, s32 arg1, s32 arg2, s32 id) __asm__("func_80046A58");
s32 gAllocReservedStreamVoice(void) __asm__("func_80046AD8");
void gApplyAnimationSoundEvent(s32 arg0, void *object) __asm__("func_80047088");
s32 gReleaseValidatedSoundVoiceHandle(void *handle) __asm__("func_800474F0");
void gUpdateSoundVoiceMix(void *voice, s32 pan, s32 volume, s32 extra) __asm__("func_80047800");
void gDrawBlinkSelectFrame(s32 x, s32 y, s32 w, s32 h) __asm__("func_80083280");
void gFreeSceneSubsystemBuffers(void) __asm__("func_80083428");
void gInitSceneNodeAndVoicePools(void) __asm__("func_8008347C");

/* 8004530C walks this node: note at +5, program +6, velocity +7, next +0x1C. */
struct LfVoice4530C {
    u8 unk00[5];
    s8 note;
    s8 program;
    s8 velocity;
    u8 unk08[0x14];
    struct LfVoice4530C *next;
    u8 unk20[0x0C];
};
void gUpdateSoundVoiceListMix(struct LfVoice4530C *voice) __asm__("func_8004530C");

/* 800450A4 / 800460D4 mix buffer: flag at +2, first 2D delta at +4, second at
 * +0xC. s32 members keep the unit's word loads aligned. */
struct LfMix {
    s8 volume;
    s8 pan;
    s8 flag;
    s8 pad;
    s32 d0, d1;
    s32 e0, e1;
};

/* 80047088 object: info record pointer at +0xAC, stream index at info+0xC1. */
struct LfInfo47088 {
    u8 unk00[0xC1];
    s8 stream;
};
struct LfObject47088 {
    u8 unk00[0xAC];
    struct LfInfo47088 *info;
};

/* 800474F0 handle: resource at +0, serial at +4; resource serial at +0x10. */
struct LfResource4749C {
    u8 unk00[0x10];
    s32 serial;
};
struct LfHandle4749C {
    struct LfResource4749C *resource;
    s32 serial;
};

/* 80047800 voice: volume +4, base +5, pan +6, extra +7. */
struct LfVoice47800 {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

/* ---------------------------------------------------------------- tests */

static void test_voice_volume(void)
{
    /* @leaf 80045050 source src/game/platform/gComputeTrackedVoiceVolume.c: 1.5x boost with 0x7F saturation, no work at zero volume */
    LK_FUNC("func_80045050");
    lf_reset();
    lf_volume_limit = 5;
    lf_voice_list = 0;
    lf_class2_list = 0;
    fcm_sound.voice_volume = 0;
    FCK_EQ(gComputeTrackedVoiceVolume(), 0, "zero volume returns zero");
    FCK_EQ(lf_volume_limit, 5, "zero volume leaves the limit selector alone");
    FCK_CHECK(lf_calls_n == 0, "zero volume makes no voice refresh call");

    fcm_sound.voice_volume = 84;                 /* 84 + 42 = 126, below the cap */
    FCK_EQ(gComputeTrackedVoiceVolume(), 126, "boost is volume plus half");
    FCK_EQ(lf_volume_limit, 1, "non-zero volume switches the limit selector on");

    fcm_sound.voice_volume = 86;                 /* 86 + 43 = 129 saturates */
    FCK_EQ(gComputeTrackedVoiceVolume(), 127, "boost saturates at 0x7F");

    fcm_sound.voice_volume = -4;                 /* -4 + (-2) = -6 */
    FCK_EQ(gComputeTrackedVoiceVolume(), -6, "negative volume keeps its sign");

    fcm_sound.voice_volume = 4096;
    FCK_EQ(gComputeTrackedVoiceVolume(), 127, "large volume saturates");
}

static void test_tracked_mix(void)
{
    struct LfMix mix;

    /* @leaf 800450A4 source src/game/boot-frontend/gComputeTrackedSoundMix.c: 1.25x mix boost, selector picks the 0x3F or 0x7F cap, silent mix on failed attenuation */
    LK_FUNC("func_800450A4");
    lf_reset();
    lf_spatial_mode = 1;      /* the real pan stage reads D_8009F734 */
    lf_volume_limit = 0;
    lf_slot_450A4.unk50 = 0x1234;
    lf_slot_450A4.unk58 = -0x2345;
    lf_slot_ptr = 0;
    lk_fill(&mix, 0x11, sizeof(mix));
    mix.volume = 0x11;
    mix.pan = 0x22;
    mix.flag = 0x33;
    FCK_EQ(gComputeTrackedSoundMix(7, (s8 *)&mix), 0, "missing slot reports failure");
    FCK_CHECK(mix.volume == 0x11 && mix.pan == 0x22 && mix.flag == 0x33,
              "missing slot leaves the mix untouched");
    FCK_EQ(lf_calls_n, 1, "missing slot skips the mix calculation");

    lf_reset();
    lf_slot_ptr = &lf_slot_450A4;
    lf_atten_ok = 0;
    mix.volume = 0x11;
    mix.pan = 0x22;
    mix.flag = 0;
    FCK_EQ(gComputeTrackedSoundMix(9, (s8 *)&mix), 1, "found slot reports success");
    FCK_CHECK(lf_calls[0].id == LF_SLOT && lf_calls[0].a0 == 9, "lookup receives the requested id");
    FCK_CHECK(lf_calls[1].id == LF_ATTEN && lf_calls[1].a0 == 0x1234 &&
              lf_calls[1].a1 == -0x2345 && lf_calls[1].a3 == 0,
              "attenuation receives the slot words and a zero flag");
    FCK_CHECK(mix.volume == 0 && mix.pan == 0x40, "failed attenuation yields a silent centred mix");
    FCK_EQ(mix.flag, 0, "failed attenuation never reaches the pan stage");
    FCK_EQ(lf_calls_n, 2, "failed attenuation makes no pan call");

    lf_reset();
    lf_atten_ok = 1;
    lf_atten_vol = 48;                    /* 48 + 24 + 12 = 84 */
    mix.volume = 0;
    mix.pan = 0;
    mix.flag = 0;
    mix.d0 = 0;
    mix.d1 = 0;
    mix.e0 = 0;
    mix.e1 = 0;
    FCK_EQ(gComputeTrackedSoundMix(1, (s8 *)&mix), 1, "successful mix returns success");
    FCK_EQ(mix.volume, 84, "volume is boosted by one half plus one quarter");
    FCK_EQ(mix.flag, 1, "successful attenuation is followed by the pan stage");
    FCK_EQ(mix.pan, 0x40, "pan stage writes the centred value for a zero delta");

    lf_volume_limit = 1;
    mix.volume = 0;
    mix.pan = 0;
    mix.flag = 0;
    gComputeTrackedSoundMix(1, (s8 *)&mix);
    FCK_EQ(mix.volume, 63, "half-volume selector caps the boost at 0x3F");

    lf_atten_vol = 37;                    /* 37 + 18 + 9 = 64, exactly the cap */
    mix.volume = 0;
    mix.pan = 0;
    mix.flag = 0;
    gComputeTrackedSoundMix(1, (s8 *)&mix);
    FCK_EQ(mix.volume, 63, "a boost that lands exactly on 0x40 is capped");

    lf_volume_limit = 0;
    lf_atten_vol = -4;                    /* -4 - 2 - 1 = -7 */
    mix.volume = 0;
    mix.flag = 0;
    gComputeTrackedSoundMix(1, (s8 *)&mix);
    FCK_EQ(mix.volume, -7, "negative mix keeps its sign");
}

static void test_stereo_mode(void)
{
    /* @leaf 800451B4 source src/game/platform/gSetStereoSoundMode.c: enable flag normalised to 0/1, effect volume re-applied */
    LK_FUNC("func_800451B4");
    lf_reset();
    lf_sound_mode = 5;
    lf_music_volume = 0x33;
    gSetStereoSoundMode(0);
    FCK_EQ(lf_sound_mode, 0, "zero disables the flag");
    FCK_EQ(lf_calls_n, 1, "one mixer call");
    FCK_CHECK(lf_calls[0].id == LF_MIXER && lf_calls[0].a0 == 0x33, "mixer receives the stored volume");

    gSetStereoSoundMode(-7);
    FCK_EQ(lf_sound_mode, 1, "any non-zero value enables the flag");
    lf_music_volume = -2;
    gSetStereoSoundMode(0);
    FCK_EQ(lf_calls[2].a0, -2, "mixer follows the current volume word");
}

static void test_vagm_volume(void)
{
    /* @leaf 80045238 source src/game/platform/gSetVagmVolumeSetting.c: clamp to 0..0x7F, store, forward the stored value */
    LK_FUNC("func_80045238");
    lf_reset();
    lf_music_volume = 0x55;
    gSetVagmVolumeSetting(-1);
    FCK_EQ(lf_music_volume, 0, "negative request clamps to zero");
    FCK_EQ(lf_calls[0].a0, 0, "mixer receives the clamped zero");

    gSetVagmVolumeSetting(0x40);
    FCK_EQ(lf_music_volume, 0x40, "in-range request stored unchanged");
    FCK_EQ(lf_calls[1].a0, 0x40, "mixer receives the stored value");

    gSetVagmVolumeSetting(0x80);
    FCK_EQ(lf_music_volume, 0x7F, "0x80 saturates at 0x7F");

    gSetVagmVolumeSetting(0x7F);
    FCK_EQ(lf_music_volume, 0x7F, "0x7F is the top of the range");
    FCK_EQ(lf_calls_n, 4, "every call re-applies the volume");
}

static void test_half_volume(void)
{
    struct LfVoice4530C a, b;

    /* @leaf 800452D4 source src/game/boot-frontend/gEnableHalfVolumeForSoundVoices.c: raise the selector then walk both stored lists in pointer order */
    LK_FUNC("func_800452D4");
    lf_reset();
    lk_fill(&a, 0, sizeof(a));
    lk_fill(&b, 0, sizeof(b));
    a.note = 0;
    a.program = 0x11;
    b.note = 0;
    b.program = 0x22;
    lf_volume_limit = 0;
    lf_voice_list = &a;
    lf_class2_list = &b;
    gEnableHalfVolumeForSoundVoices();
    FCK_EQ(lf_volume_limit, 1, "selector is raised");
    FCK_EQ(lf_calls_n, 4, "each one-voice list is dispatched once");
    FCK_CHECK(lf_calls[0].id == LF_SETMIX && lf_calls[0].a0 == (s32)&a &&
              lf_calls[2].a0 == (s32)&b,
              "voice list is walked before the class-2 list");
    FCK_CHECK(lf_calls[0].a1 == 0x11 && lf_calls[2].a1 == 0x22,
              "each list's own program reaches the voice call");

    lf_reset();
    lf_voice_list = 0;
    lf_class2_list = 0;
    lf_volume_limit = 9;
    gEnableHalfVolumeForSoundVoices();
    FCK_CHECK(lf_volume_limit == 1 && lf_calls_n == 0,
              "null lists are tolerated and the selector is still rewritten to 1");
}

static void test_list_mix(void)
{
    struct LfVoice4530C v[3];

    /* @leaf 8004530C source src/game/boot-frontend/gUpdateSoundVoiceListMix.c: follow next, skip note -1, forward program/note/velocity */
    LK_FUNC("func_8004530C");
    lf_reset();
    lk_fill(v, 0xA5, sizeof(v));
    v[0].note = 10;
    v[0].program = 3;
    v[0].velocity = 100;
    v[0].next = &v[1];
    v[1].note = -1;                       /* skipped */
    v[1].program = 4;
    v[1].velocity = 90;
    v[1].next = &v[2];
    v[2].note = -5;
    v[2].program = 5;
    v[2].velocity = 80;
    v[2].next = 0;
    gUpdateSoundVoiceListMix(v);
    FCK_EQ(lf_calls_n, 4, "two eligible voices, each dispatched once");
    FCK_CHECK(lf_calls[0].a0 == (s32)&v[0] && lf_calls[0].a1 == 3 &&
              lf_calls[0].a2 == 10 && lf_calls[0].a3 == 100,
              "first voice forwards program, note and velocity");
    FCK_CHECK(lf_calls[2].a0 == (s32)&v[2] && lf_calls[2].a1 == 5 &&
              lf_calls[2].a2 == -5 && lf_calls[2].a3 == 80,
              "traversal skips the -1 note and reaches the tail");
    FCK_EQ(v[1].program, 4, "skipped voice keeps its record");
    FCK_EQ(v[2].next, 0, "null tail stays null");

    lf_reset();
    gUpdateSoundVoiceListMix(0);
    FCK_EQ(lf_calls_n, 0, "empty list dispatches nothing");
}

static void test_clear_limit(void)
{
    struct LfVoice4530C p, q, r;

    /* @leaf 80045370 source src/game/boot-frontend/gClearVolumeLimitAndRefreshVoices.c: flag gates clearing and three list walks */
    LK_FUNC("func_80045370");
    lf_reset();
    lk_fill(&p, 0, sizeof(p));
    lk_fill(&q, 0, sizeof(q));
    lk_fill(&r, 0, sizeof(r));
    p.note = q.note = r.note = 0;
    p.program = 1;
    q.program = 2;
    r.program = 3;
    lf_volume_limit = 0;
    lf_priority_list = &p;
    lf_voice_list = &q;
    lf_class2_list = &r;
    gClearVolumeLimitAndRefreshVoices();
    FCK_EQ(lf_volume_limit, 0, "clear flag stays clear");
    FCK_EQ(lf_calls_n, 0, "clear flag walks nothing");

    lf_volume_limit = 7;
    gClearVolumeLimitAndRefreshVoices();
    FCK_EQ(lf_volume_limit, 0, "set flag is cleared");
    FCK_EQ(lf_calls_n, 6, "all three lists are walked");
    FCK_CHECK(lf_calls[0].a0 == (s32)&p && lf_calls[0].a1 == 1,
              "priority list is walked first");
    FCK_CHECK(lf_calls[2].a0 == (s32)&q && lf_calls[2].a1 == 2,
              "general voice list is walked second");
    FCK_CHECK(lf_calls[4].a0 == (s32)&r && lf_calls[4].a1 == 3,
              "class-2 list is walked third");

    lf_reset();
    lf_volume_limit = 1;
    lf_priority_list = 0;
    lf_voice_list = 0;
    lf_class2_list = 0;
    gClearVolumeLimitAndRefreshVoices();
    FCK_CHECK(lf_volume_limit == 0 && lf_calls_n == 0,
              "null pointers are tolerated after the flag is cleared");
}

static void test_sound_pools(void)
{
    int i, banks_ok, voices_ok;

    /* @leaf 800453B8 source src/game/platform/gInitSoundPools.c: 24-node free list, six cleared bank markers, two stream records */
    LK_FUNC("func_800453B8");
    lf_reset();
    lk_fill(lf_voice_pool, 0xA5, sizeof(lf_voice_pool));
    lk_fill(lf_banks, 0x5C, sizeof(lf_banks));
    lk_fill(lf_streams, 0x3D, sizeof(lf_streams));
    lf_free_list = (void *)0x1111;
    lf_free_count = -1;
    lf_priority_list = (void *)1;
    lf_priority_count = (void *)1;
    lf_voice_list = (void *)1;
    lf_class0_count = (void *)1;
    lf_class2_list = (void *)1;
    lf_class2_count = (void *)1;
    lf_pending_release = 77;
    lf_volume_limit = 9;
    lf_alloc_base = 1000;
    lf_alloc_seq = 0;
    lf_pending_at_stream = -1;
    gInitSoundPools();
    FCK_CHECK(lf_free_list == lf_voice_pool, "free list head is the first voice");
    FCK_EQ(lf_free_count, 24, "24 free voices");
    FCK_CHECK(lf_priority_list == 0 && lf_priority_count == 0 && lf_voice_list == 0 &&
              lf_class0_count == 0 && lf_class2_list == 0 && lf_class2_count == 0,
              "six list heads and counts cleared");
    voices_ok = 1;
    for (i = 0; i < 24; i++) {
        if (lf_voice_pool[i].index != i) voices_ok = 0;
        if (lf_voice_pool[i].unk10 != 0) voices_ok = 0;
        if (lf_voice_pool[i].next != (i == 23 ? (struct LfVoicePool *)0 : &lf_voice_pool[i + 1]))
            voices_ok = 0;
    }
    FCK_CHECK(voices_ok, "every voice gets its index, a cleared work word and the next link");
    FCK_CHECK(lf_voice_pool[23].next == 0, "last voice terminates the list");
    FCK_CHECK(lk_all(&lf_voice_pool[0], 0, 0x0D, 0xA5) && lk_all(&lf_voice_pool[0], 0x0E, 0x10, 0xA5) &&
              lk_all(&lf_voice_pool[0], 0x14, 0x1C, 0xA5) && lk_all(&lf_voice_pool[0], 0x20, 0x2C, 0xA5),
              "unrelated voice bytes untouched");
    FCK_CHECK(lk_all(&lf_voice_pool[1], 0, 0x0D, 0xA5) && lk_all(&lf_voice_pool[23], 0x14, 0x1C, 0xA5),
              "later voices keep their unrelated bytes too");
    FCK_CHECK(lf_calls[0].id == LF_RESETBANK && lf_calls[1].id == LF_INITCHAN,
              "bank reset and channel init run first, in that order");
    FCK_CHECK(lf_calls_n == 4 && lf_calls[2].id == LF_ALLOCPRI &&
              lf_calls[3].id == LF_ALLOCPRI,
              "the two reserved stream voices are requested after the init calls");
    banks_ok = 1;
    for (i = 0; i < 6; i++) {
        if (lf_banks[i].unk00 != 0) banks_ok = 0;
        if (!lk_all(&lf_banks[i], 4, 0x14, 0x5C)) banks_ok = 0;
    }
    FCK_CHECK(banks_ok, "six bank markers cleared, their payload untouched");
    FCK_EQ(lf_pending_release, 0, "pending release count cleared");
    FCK_EQ(lf_pending_at_stream, 0, "release count is cleared before the stream loop");
    FCK_CHECK(lf_streams[0].playing == 0 && lf_streams[1].playing == 0,
              "stream playing words cleared");
    FCK_CHECK(lf_streams[0].handle == 1000 && lf_streams[1].handle == 1001,
              "each stream stores its own reserved voice handle");
    FCK_CHECK(lf_streams[0].track == 0x3D3D3D3D && lf_streams[1].track == 0x3D3D3D3D,
              "stream track words untouched");
    FCK_EQ(lf_volume_limit, 0, "volume limit selector cleared last");
    FCK_EQ(lf_alloc_seq, 2, "two reserved voices requested");
}

static void test_spatial_pan(void)
{
    struct LfMix mix;

    /* @leaf 800460D4 source src/game/platform/gComputeSpatialSoundPan.c: single-listener >>10 and dual-listener >>11 halves, both clamped */
    LK_FUNC("func_800460D4");
    lf_reset();
    lf_pos_angles[0].angle = 0x111;
    lf_pos_angles[1].angle = 0x222;
    lf_spatial_mode = 1;
    lk_fill(&mix, 0x55, sizeof(mix));
    mix.flag = 0;
    mix.d0 = 0x4000;                      /* 16384 >> 10 = 16; 16 + 64 = 80 */
    mix.e0 = 0x77;
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_EQ(lf_calls_n, 1, "single-listener mode rotates one delta");
    FCK_CHECK(lf_calls[0].id == LF_ROT && lf_calls[0].a0 == 0x111 &&
              lf_calls[0].a1 == (s32)&mix.d0,
              "rotation uses listener 0's angle on the first delta");
    FCK_EQ(mix.pan, 80, "single-listener pan is delta>>10 plus 0x40");
    FCK_EQ(mix.flag, 1, "computed flag is set");
    FCK_EQ(mix.e0, 0x77, "second delta untouched in single-listener mode");

    lf_reset();
    mix.flag = 0;
    mix.d0 = -0x20000;                    /* -131072 >> 10 = -128; -128 + 64 = -64 clamps to 0 */
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_EQ(mix.pan, 0, "negative single-listener pan clamps to 0");

    lf_reset();
    mix.flag = 0;
    mix.d0 = 0x200000;                    /* 2048 + 64 clamps to 0x7F */
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_EQ(mix.pan, 127, "single-listener pan clamps to 0x7F");

    lf_reset();
    lf_spatial_mode = 2;
    mix.flag = 0;
    mix.d0 = 0x2000;                      /* 4 + 32 = 36 */
    mix.e0 = 0x4000;                      /* 8 + 32 = 40 */
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_EQ(lf_calls_n, 2, "dual-listener mode rotates both deltas");
    FCK_CHECK(lf_calls[1].a0 == 0x222 && lf_calls[1].a1 == (s32)&mix.e0,
              "second rotation uses listener 1's angle on the second delta");
    FCK_EQ(mix.pan, 76, "dual-listener pan is the sum of both clamped halves");
    FCK_EQ(mix.flag, 1, "dual-listener pan marks the mix computed");

    lf_reset();
    mix.flag = 0;
    mix.d0 = -0x20000;                    /* -64 + 32 clamps to 0 */
    mix.e0 = 0x100000;                    /* 512 + 32 clamps to 0x40 */
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_EQ(mix.pan, 64, "each dual-listener half is clamped to 0..0x40 before summing");

    lf_reset();
    mix.flag = 3;
    mix.pan = 0x12;
    mix.d0 = 0x4000;
    gComputeSpatialSoundPan((s8 *)&mix);
    FCK_CHECK(lf_calls_n == 0 && mix.pan == 0x12 && mix.flag == 3,
              "already computed mix is not recomputed");
}

static void test_stop_handle(void)
{
    /* @leaf 80046A34 source src/game/platform/gStopSoundVoiceHandle.c: null handle is a silent no-op, everything else is released */
    LK_FUNC("func_80046A34");
    lf_reset();
    gStopSoundVoiceHandle(0);
    FCK_EQ(lf_calls_n, 0, "null handle releases nothing");
    gStopSoundVoiceHandle(5);
    FCK_EQ(lf_calls_n, 1, "non-null handle is released once");
    FCK_CHECK(lf_calls[0].id == LF_RELEASE && lf_calls[0].a0 == 5, "release receives the handle");
    gStopSoundVoiceHandle(-1);
    FCK_CHECK(lf_calls_n == 2 && lf_calls[1].a0 == -1, "negative handle is released too");
}

static void test_alloc_for_id(void)
{
    /* @leaf 80046A58 source src/game/platform/gAllocVoiceForSoundId.c: bank table with limit 0x47, 0xFF slot suppresses allocation */
    LK_FUNC("func_80046A58");
    lf_reset();
    lf_resolve_slot = 0xFF;
    FCK_EQ(gAllocVoiceForSoundId(0x40, 2, 1, 0x30), 0, "unmapped id returns no handle");
    FCK_EQ(lf_calls_n, 1, "unmapped id stops after the lookup");
    FCK_CHECK(lf_calls[0].id == LF_RESOLVE && lf_calls[0].a0 == 0x30 &&
              lf_calls[0].a1 == (s32)lf_bank_low && lf_calls[0].a2 == 0x47,
              "lookup receives id, the shared bank table and limit 0x47");

    lf_reset();
    lf_resolve_slot = 3;
    lf_alloc_base = 0x4242;
    lf_alloc_seq = 0;
    FCK_EQ(gAllocVoiceForSoundId(0x40, 2, 1, 0x30), 0x4242, "resolved slot returns the voice handle");
    FCK_EQ(lf_calls_n, 2, "resolved slot allocates once");
    FCK_CHECK(lf_calls[1].id == LF_ALLOCPRI && lf_calls[1].a0 == 0x40 &&
              lf_calls[1].a1 == 2 && lf_calls[1].a2 == 1 && lf_calls[1].a3 == 3,
              "allocation forwards the three arguments and the resolved slot");
}

static void test_reserved_voice(void)
{
    /* @leaf 80046AD8 source src/game/boot-frontend/gAllocReservedStreamVoice.c: fixed reserved-stream request (0x7F, 0xA, 2, slot 0xFF) */
    LK_FUNC("func_80046AD8");
    lf_reset();
    lf_alloc_base = 500;
    lf_alloc_seq = 0;
    FCK_EQ(gAllocReservedStreamVoice(), 500, "returns the allocator's handle");
    FCK_EQ(lf_calls_n, 1, "one allocation");
    FCK_CHECK(lf_calls[0].id == LF_ALLOCPRI && lf_calls[0].a0 == 0x7F &&
              lf_calls[0].a1 == 0xA && lf_calls[0].a2 == 2 && lf_calls[0].a3 == 0xFF,
              "fixed volume, priority, class and the 0xFF slot are forwarded");
    lf_alloc_base = -7;
    lf_alloc_seq = 0;
    FCK_EQ(gAllocReservedStreamVoice(), -7, "negative handle is passed through unchanged");
}

static void test_animation_event(void)
{
    struct LfObject47088 object;
    struct LfInfo47088 info;
    s32 voice[4];

    /* @leaf 80047088 source src/game/platform/gApplyAnimationSoundEvent.c: 0x20 gate, same-track volume dip, restart, one-shot and track 7 no-op */
    LK_FUNC("func_80047088");
    lf_reset();
    lk_fill(&info, 0, sizeof(info));
    lk_fill(&object, 0, sizeof(object));
    info.stream = 1;
    object.info = &info;
    lk_fill(voice, 0x11, sizeof(voice));
    voice[2] = 0x300;
    fcm_sound.sfx_result = (s32)voice;
    lk_fill(lf_streams, 0x66, sizeof(lf_streams));
    lf_streams[1].playing = 1;
    lf_streams[1].track = 2;
    lf_streams[1].handle = 0x555;

    gApplyAnimationSoundEvent(0x10, &object);
    FCK_CHECK(lf_streams[1].playing == 1 && lf_streams[1].track == 2 &&
              lf_streams[1].handle == 0x555 && lf_calls_n == 0 && fcm_sound.sfx_calls == 0,
              "request without bit 0x20 does nothing");

    lf_reset();
    gApplyAnimationSoundEvent(0x22, &object);       /* same track 2 */
    FCK_CHECK(lf_calls_n == 0 && fcm_sound.sfx_calls == 1, "same track only looks the voice up");
    FCK_CHECK(fcm_sound.sfx_ids[0] == 0x3A && fcm_sound.sfx_last_arg1 == (s32)&object,
              "lookup id is track|0x38 and carries the object");
    FCK_EQ(voice[2], 0x29C, "same track lowers the voice volume by 0x64");
    FCK_CHECK(voice[0] == 0x11111111 && voice[1] == 0x11111111 && voice[3] == 0x11111111,
              "other voice words untouched");
    FCK_CHECK(lf_streams[1].playing == 1 && lf_streams[1].track == 2,
              "same track keeps the stream playing");

    lf_reset();
    voice[2] = 0x300;
    fcm_sound.sfx_result = 0;
    gApplyAnimationSoundEvent(0x22, &object);
    FCK_CHECK(lf_calls_n == 0 && fcm_sound.sfx_calls == 1 && voice[2] == 0x300,
              "missing voice skips the volume dip");

    lf_reset();
    voice[2] = 0x300;
    fcm_sound.sfx_result = (s32)voice;
    gApplyAnimationSoundEvent(0x23, &object);       /* switch to track 3 */
    FCK_CHECK(lf_calls_n == 2 && fcm_sound.sfx_calls == 1, "switch stops, releases and starts");
    FCK_CHECK(fcm_sound.sfx_ids[0] == 0x34 && fcm_sound.sfx_last_arg1 == (s32)&object,
              "old track is stopped with track+0x32");
    FCK_CHECK(lf_calls[0].id == LF_STOPSTREAM && lf_calls[0].a0 == 0x555 &&
              lf_calls[0].a7 == 1,
              "stream handle is released after the stop sound");
    FCK_CHECK(lf_calls[1].id == LF_PLAYON && lf_calls[1].a0 == 0x25 &&
              lf_calls[1].a1 == 0 && lf_calls[1].a2 == 0 && lf_calls[1].a3 == -1 &&
              lf_calls[1].a4 == 0x555 && lf_calls[1].a5 == (s32)&object,
              "new loop starts with track+0x22 on the retained handle");
    FCK_EQ(lf_streams[1].track, 3, "stream track is updated");
    FCK_EQ(lf_streams[1].playing, 1, "stream is marked playing");
    FCK_EQ(lf_streams[1].handle, 0x555, "start reuses the stored handle");

    lf_reset();
    lf_streams[1].playing = 0;
    lf_streams[1].track = 0x77;
    voice[2] = 0x300;
    gApplyAnimationSoundEvent(0x26, &object);       /* one-shot track 6 */
    FCK_CHECK(lf_calls_n == 0 && fcm_sound.sfx_calls == 1, "track 6 makes a single call");
    FCK_CHECK(fcm_sound.sfx_ids[0] == 0x3D,
              "one-shot uses the fixed id 0x3D");
    FCK_EQ(voice[2], 0x29C, "one-shot also dips the voice volume");
    FCK_CHECK(lf_streams[1].track == 6 && lf_streams[1].playing == 0,
              "one-shot records the track but stays stopped");

    lf_reset();
    lf_streams[1].playing = 0;
    lf_streams[1].track = 0x77;
    gApplyAnimationSoundEvent(0x27, &object);       /* track 7 */
    FCK_CHECK(lf_calls_n == 0 && fcm_sound.sfx_calls == 0 && lf_streams[1].track == 0x77,
              "track 7 is a no-op");

    lf_reset();
    lf_streams[1].playing = 0;
    lf_streams[1].track = 0x77;
    gApplyAnimationSoundEvent(0x20, &object);       /* track 0 */
    FCK_CHECK(lf_calls_n == 1 && fcm_sound.sfx_calls == 0 &&
              lf_calls[0].id == LF_PLAYON && lf_calls[0].a0 == 0x22,
              "track 0 starts the lowest loop id");
    FCK_CHECK(lf_streams[1].track == 0 && lf_streams[1].playing == 1,
              "track 0 is stored and marked playing");
    FCK_CHECK(lf_streams[0].playing == 0x66666666 && lf_streams[0].track == 0x66666666,
              "other stream entry untouched");
}

static void test_validated_handle(void)
{
    struct LfHandle4749C handle;
    struct LfResource4749C resource;

    /* @leaf 800474F0 source src/game/platform/gReleaseValidatedSoundVoiceHandle.c: release only on serial match, clear the slot, idempotent */
    LK_FUNC("func_800474F0");
    lf_reset();
    FCK_EQ(gReleaseValidatedSoundVoiceHandle(0), 0, "null handle reports nothing released");
    FCK_EQ(lf_calls_n, 0, "null handle calls nothing");

    lk_fill(&handle, 0x99, sizeof(handle));
    handle.resource = 0;
    handle.serial = 0x1234;
    FCK_EQ(gReleaseValidatedSoundVoiceHandle(&handle), 0, "null resource reports nothing released");
    FCK_EQ(lf_calls_n, 0, "null resource calls nothing");
    FCK_EQ(handle.serial, 0x1234, "null resource leaves the serial alone");

    lk_fill(&resource, 0x5A, sizeof(resource));
    resource.serial = 0x2200;
    handle.resource = &resource;
    handle.serial = 0x1234;
    FCK_EQ(gReleaseValidatedSoundVoiceHandle(&handle), 0, "serial mismatch reports nothing released");
    FCK_EQ(lf_calls_n, 0, "serial mismatch does not release");
    FCK_CHECK(handle.resource == &resource, "stale handle keeps its resource pointer");

    handle.serial = 0x2200;
    FCK_EQ(gReleaseValidatedSoundVoiceHandle(&handle), 1, "serial match reports a release");
    FCK_EQ(lf_calls_n, 1, "serial match releases once");
    FCK_CHECK(lf_calls[0].id == LF_RELEASE && lf_calls[0].a0 == (s32)&resource,
              "release receives the resource pointer");
    FCK_EQ(handle.resource, 0, "released handle is cleared");

    FCK_EQ(gReleaseValidatedSoundVoiceHandle(&handle), 0, "second release is a no-op");
    FCK_EQ(lf_calls_n, 1, "second release does not call again");
}

static void test_voice_mix(void)
{
    struct LfVoice47800 voice;

    /* @leaf 80047800 source src/game/platform/gUpdateSoundVoiceMix.c: set fields first, then forward the record's adopted values */
    LK_FUNC("func_80047800");
    lf_reset();
    voice.bank = 0x1234;
    voice.volume = 0x40;
    voice.baseVolume = 0x11;
    voice.pan = 6;
    voice.extra = 0x2A;
    gUpdateSoundVoiceMix(&voice, -5, 100, 7);
    FCK_EQ(lf_calls_n, 2, "field update then mixer update");
    FCK_CHECK(lf_calls[0].id == LF_SETMIX && lf_calls[0].a0 == (s32)&voice &&
              lf_calls[0].a1 == -5 && lf_calls[0].a2 == 100 && lf_calls[0].a3 == 7,
              "first call carries the requested pan, volume and extra");
    FCK_CHECK(lf_calls[1].id == LF_MIXAPPLY && lf_calls[1].a0 == (s32)&voice &&
              lf_calls[1].a1 == 6 && lf_calls[1].a2 == 0x40 && lf_calls[1].a3 == 0x2A,
              "second call carries the values read back from the record");
    FCK_CHECK(voice.bank == 0x1234 && voice.baseVolume == 0x11,
              "bank and base volume are not part of the mix");

    lf_reset();
    gUpdateSoundVoiceMix(&voice, 1, 2, 3);
    FCK_CHECK(lf_calls[1].a1 == 6 && lf_calls[1].a2 == 0x40 && lf_calls[1].a3 == 0x2A,
              "mixer values come from the record, not from the arguments");
}

static void test_blink_frame(void)
{
    /* @leaf 80083280 source src/game/gpu-2d-hud/gDrawBlinkSelectFrame.c: palette index (tick>>4)&7, frame origin x-(w+1)/2, y-h/2, four 2px borders */
    LK_FUNC("func_80083280");
    lf_reset();
    lf_shade_table[0] = 0x10;
    lf_shade_table[1] = 0x11;
    lf_shade_table[2] = 0x12;
    lf_shade_table[3] = 0x13;
    lf_shade_table[4] = 0x14;
    lf_shade_table[5] = 0x15;
    lf_shade_table[6] = 0x16;
    lf_shade_table[7] = 0x17;
    lf_frame_tick = 0x57;                 /* 0x57 >> 4 = 5 */
    gDrawBlinkSelectFrame(100, 50, 9, 7);
    FCK_EQ(lf_calls_n, 4, "four border rectangles");
    FCK_EQ(lf_calls[0].id, LF_QUAD, "first rectangle via the quad emitter");
    FCK_CHECK(lf_calls[0].a0 == 95 && lf_calls[0].a1 == 45 &&
              lf_calls[0].a2 == 10 && lf_calls[0].a3 == 2,
              "top border spans w+1 at x-(w+1)/2, two rows above y-h/2");
    FCK_CHECK(lf_calls[1].a0 == 95 && lf_calls[1].a1 == 47 &&
              lf_calls[1].a2 == 2 && lf_calls[1].a3 == 7,
              "left border is two pixels wide and h tall");
    FCK_CHECK(lf_calls[2].a0 == 103 && lf_calls[2].a1 == 47 &&
              lf_calls[2].a2 == 2 && lf_calls[2].a3 == 7,
              "right border sits at the far edge of the frame");
    FCK_CHECK(lf_calls[3].a0 == 95 && lf_calls[3].a1 == 54 &&
              lf_calls[3].a2 == 10 && lf_calls[3].a3 == 2,
              "bottom border starts at y-h/2+h");
    FCK_CHECK(lf_calls[0].a4 == 0x12C && lf_calls[3].a4 == 0x12C,
              "all borders use ordering link 0x12C");
    FCK_EQ(lf_calls[0].a5, 0x95, "shade value is the table entry plus 0x80");
    FCK_CHECK(lf_calls[0].a6 == 0x95 && lf_calls[0].a7 == 0x95,
              "rectangle is drawn in grey (r == g == b)");

    lf_reset();
    lf_frame_tick = 0x0F;                 /* index 0 */
    gDrawBlinkSelectFrame(0, 0, 1, 1);
    FCK_EQ(lf_calls[0].a5, 0x90, "tick 0x0F selects the first palette entry");
    FCK_CHECK(lf_calls[0].a0 == -1 && lf_calls[0].a1 == -2 && lf_calls[0].a2 == 2,
              "one-pixel inner box centres the two-pixel frame around the origin");

    lf_reset();
    lf_frame_tick = -1;                   /* -1 >> 4 = -1, & 7 = 7 */
    gDrawBlinkSelectFrame(0, 0, 0, 0);
    FCK_EQ(lf_calls[0].a5, 0x97, "negative tick masks to the last palette entry");
}

static void test_scene_free(void)
{
    /* @leaf 80083428 source src/game/renderer-scene/gFreeSceneSubsystemBuffers.c: pool freed unconditionally, grid and table only when non-null, table pointer retained */
    LK_FUNC("func_80083428");
    lf_reset();
    lf_mission_state = (void *)0x1000;
    lf_bucket_grid = (void *)0x2000;
    lf_mission_table = (void *)0x3000;
    gFreeSceneSubsystemBuffers();
    FCK_EQ(lf_calls_n, 3, "three allocations released");
    FCK_CHECK(lf_calls[0].id == LF_FREEPOOL && lf_calls[0].a0 == 0x1000,
              "node pool freed through the pool helper");
    FCK_CHECK(lf_calls[1].id == LF_FREE && lf_calls[1].a0 == 0x2000 &&
              lf_calls[2].id == LF_FREE && lf_calls[2].a0 == 0x3000,
              "grid then table freed as plain blocks");
    FCK_EQ(lf_mission_state, 0, "pool state cleared");
    FCK_EQ(lf_bucket_grid, 0, "grid pointer cleared");
    FCK_EQ(lf_mission_table, 0x3000, "table pointer is not cleared");

    lf_reset();
    lf_mission_state = (void *)0x4000;
    lf_bucket_grid = 0;
    lf_mission_table = 0;
    gFreeSceneSubsystemBuffers();
    FCK_CHECK(lf_calls_n == 1 && lf_calls[0].id == LF_FREEPOOL && lf_calls[0].a0 == 0x4000,
              "null grid and table are skipped while the pool is still freed");
    FCK_EQ(lf_bucket_grid, 0, "already-null grid stays null");

    lf_reset();
    lf_mission_state = 0;
    lf_bucket_grid = (void *)0x2100;
    lf_mission_table = 0;
    gFreeSceneSubsystemBuffers();
    FCK_CHECK(lf_calls_n == 2 && lf_calls[0].a0 == 0 &&
              lf_calls[1].id == LF_FREE && lf_calls[1].a0 == 0x2100,
              "pool is freed even when its recorded state is null");
}

static void test_scene_init(void)
{
    int i;

    /* @leaf 8008347C source src/game/renderer-scene/gInitSceneNodeAndVoicePools.c: 250x0x5C pool, 0x104 emitter table with an all-ones free mask */
    LK_FUNC("func_8008347C");
    lf_reset();
    for (i = 0; i < 0x41; i++) lf_table_buffer[i] = (s32)0xCCCCCCCC;
    lf_pool_ret = (void *)0x7000;
    lf_alloc_ptr_ret = lf_table_buffer;
    lf_mission_clear = 5;
    lf_bucket_grid = (void *)0x9999;
    lf_mission_state = (void *)0x8888;
    lf_mission_table = (void *)0x7777;
    gInitSceneNodeAndVoicePools();
    FCK_EQ(lf_calls_n, 2, "one pool creation and one plain allocation");
    FCK_CHECK(lf_calls[0].id == LF_POOL && lf_calls[0].a0 == 0xFA &&
              lf_calls[0].a1 == 0x5C && lf_calls[0].a2 == 2,
              "node pool is 250 blocks of 0x5C bytes with tag 2");
    FCK_CHECK(lf_calls[1].id == LF_ALLOC && lf_calls[1].a0 == 0x104 &&
              lf_calls[1].a1 == 2,
              "emitter table is a 0x104-byte tagged allocation");
    FCK_EQ(lf_mission_state, 0x7000, "pool state published");
    FCK_EQ(lf_mission_table, (s32)lf_table_buffer, "table pointer published");
    FCK_EQ(lf_mission_clear, 0, "mission clear flag cleared");
    FCK_EQ(lf_bucket_grid, 0, "bucket grid pointer cleared");
    FCK_EQ(lf_table_buffer[0], -1, "free mask starts with every slot free");
    FCK_CHECK(lk_all(lf_table_buffer, 4, 0x104, 0xCC), "rest of the table untouched");
}

int main(void)
{
    test_voice_volume();
    test_tracked_mix();
    test_stereo_mode();
    test_vagm_volume();
    test_half_volume();
    test_list_mix();
    test_clear_limit();
    test_sound_pools();
    test_spatial_pan();
    test_stop_handle();
    test_alloc_for_id();
    test_reserved_voice();
    test_animation_event();
    test_validated_handle();
    test_voice_mix();
    test_blink_frame();
    test_scene_free();
    test_scene_init();
    return lk_done("leaf_flow");
}
