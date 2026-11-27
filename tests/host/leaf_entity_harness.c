/* Stream setup/mix, preview voices, results rendering, and entity stream
 * requests. All RAM and tables are synthetic; stand-ins observe the boundary
 * to other units, not another implementation of the function under test.
 * Results drawing and portrait audio call the real decoder and voice
 * acquisition units in this suite, respectively.
 *
 * Equivalent-mutant notes (tools/host_leaf_suite.py mutate):
 *  - 8002283C line 74 `channel >= 0` -> `>` (ge->gt) and `0` -> `1` (int+1):
 *    when channel == 0, both the true branch (clamped = channel) and the
 *    else branch (clamped = 0) yield clamped = 0, so both mutants are
 *    identical for all s32 inputs.
 *  - 80029A60 line 73 `(unk02 + unk00) & 7` -> `(unk02 - unk00) & 7`:
 *    guarded by line 58 (normal) and line 70 (priority), this branch only
 *    runs when count <= 0; for every reachable valid queue state count == 0,
 *    so +0 and -0 are identical. Line 59 `return 0;` -> `return 0;` is a
 *    verbatim no-op rewrite.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

s32 gConfigureAudioStreaming(s32 key, ...) __asm__("func_80022484");
void gApplyStreamChannelMix(s32 channel, s32 level, s32 pan) __asm__("func_8002283C");
s32 gAcquireTrackedSoundVoice(s32 mode, s32 bytes) __asm__("func_80024064");
void gEmitSplitScreenBlit(s32 link) __asm__("func_8002549C");
void gUpdateOptionsPreviewVoice(s32 track, s32 volume, s32 slot) __asm__("func_80025E18");
void gDrawResultsTextBlock(void *player, u8 *text, s32 x, s32 color) __asm__("func_80027F80");
void gDrawPlayerResultsScreen(void) __asm__("func_80028164");
s32 gStepSynchronizedPortrait(s32 *track) __asm__("func_80029748");
struct Unk41DC4 { s32 fixture_id; };
s32 gQueueEntityStreamRequest(s32 id, struct Unk41DC4 *entity, u8 priority) __asm__("func_80029A60");
void gServiceStreamRequestQueue(void) __asm__("func_80029C34");

struct Voice {
    s32 bank;
    s8 volume, base_volume, pan, extra;
    u8 pad08[5];
    s8 channel;
};
struct StreamChannel {
    s32 id;
    struct Voice *voice;
    s32 level, pan;
    u8 pad10[0x14];
};
/* Audio setup also writes offsets 0x88..0x97, beyond the channel array. */
struct {
    u32 mode;
    s32 active;
    u32 pad08[7];
    struct StreamChannel channels[3];
    u32 tail[5];
} mixer __asm__("D_800A0968");
u32 command_slot __asm__("D_8009F630");
u32 stream_callback __asm__("D_8009F634");
u32 stream_slot_a __asm__("D_8009F638");
u32 stream_slot_b __asm__("D_8009F63C");

struct VoiceResource {
    u8 pad00[13];
    s8 owner;
    u8 pad0e[2];
    s32 serial;
};
struct VoiceHandle {
    struct VoiceResource *resource;
    s32 serial, state, selector, deadline, delay;
    u8 has_selector;
};
struct VoiceHandle tracked_voice __asm__("D_800A0A80");
u8 cvk_active __asm__("D_8009F26C");
s32 slot_voice[2] __asm__("D_8009F678");
s32 slot_track[2] __asm__("D_8009FA78");
u8 *packet_cursor __asm__("D_8009FA90");
u8 *packet_limit __asm__("D_8009F9C0");

struct ResultsPlayer {
    u8 pad00[0x30];
    u32 buttons;
    u8 pad34[0x14];
    s32 still_displaying;
    u8 pad4c[0x2d4];
    s32 eligible;
};
struct PlayerEntity {
    u8 pad00[0xac];
    struct ResultsPlayer *player;
};
struct Session {
    u8 pad00[0x40];
    s32 first_team, second_team, reserved48, round;
};
struct PlayerEntity *player_entities[5] __asm__("D_8009FA58") = { 0 };
/* The results unit names slot 1 separately; preview indexes from slot 0. */
__asm__(".globl D_8009FA5C\n.set D_8009FA5C,D_8009FA58+4");
s32 single_player_entity __asm__("D_8009F9EC");
struct Session *session __asm__("D_8009FA94");
s32 player_count __asm__("D_8009FA0C");
s32 screen_state __asm__("D_8009FA00");
s32 frame_time __asm__("D_8009FAA4");
s32 *results_cache __asm__("D_8009F68C");
u8 **results_text __asm__("D_8009F2C0");
u8 decimal_format[3] __asm__("D_8009F2FC");
u8 time_format[5] __asm__("D_80010DCC");
u8 round_format[5] __asm__("D_8009F300");
u8 center_codes[2] __asm__("D_8009ABF0");
u8 outer_label_codes[2] __asm__("D_8009AC08");
u8 inner_label_codes[2] __asm__("D_8009AC14");
u8 outer_stat_codes[2] __asm__("D_8009AC20");
u8 middle_stat_codes[2] __asm__("D_8009AC2C");
u8 inner_stat_codes[2] __asm__("D_8009AC44");

struct Request {
    u16 id;
    u8 has_entity, reserved;
    struct Unk41DC4 *entity;
};
struct {
    s16 count, head;
    struct Request entries[8];
} requests __asm__("D_800A0BC0");
struct {
    s32 reserved00, reserved04, position, id;
    u8 pad10[0x10];
    u8 start_pending;
    u8 pad21[0x1b];
    u8 progress;
    u8 pad3d[0xf];
    s32 base;
} stream_buffers __asm__("D_800A0B70");
u8 stream_flags __asm__("D_8009F6B8");
s32 stream_id __asm__("D_8009F6BC");
u8 stream_latch __asm__("D_8009F6C0");
s32 stream_deadline __asm__("D_8009F6C4");
s32 portrait_start_tick __asm__("D_8009F6C8");
s32 stream_gate __asm__("D_8009F6DC");
u8 stream_guard __asm__("D_8009F69C");
u8 *resource_chunks __asm__("D_8009F698");
s32 frame_ticks __asm__("D_8009FA2C");
u8 stream_start_pending[1] __asm__("D_800A0B90");
s32 portrait_byte_count[1] __asm__("D_800A0C44");

/* Stand-ins record only the boundary data the tests use. Return values and
 * out-parameters (e.g. the signed effective mix) are set by each fixture.
 */
static const char fill_call[] = "fill";
static const char option_call[] = "stream option";
static const char buffers_call[] = "assign SPU buffers";
static const char reset_call[] = "reset stream controller";
static u32 options[11];
static u32 option_present;
static s32 buffers_result;

void record_fill(void *dst, s32 value, s32 size) __asm__("func_8001E4C0");
void record_fill(void *dst, s32 value, s32 size)
{
    fck_record(fill_call, (s32)dst, value, size);
    /* Bound writes to the synthetic mixer, including its trailing canary. */
    if (dst == &mixer && size >= 0 && size <= sizeof(mixer))
        lk_fill(dst, value, size);
}

s32 provide_stream_option(s32 *first_key, s32 key, u32 *out, s32 *index) __asm__("func_80022B84");
s32 provide_stream_option(s32 *first_key, s32 key, u32 *out, s32 *index)
{
    fck_record(option_call, key, *first_key, (s32)index);
    if (key < 0 || key >= 11 || !(option_present & (1u << key))) return 0;
    *out = options[key];
    return 1;
}

/* 80022BEC itself unconditionally dereferences an absolute kseg0 address. */
s32 provide_buffer_result(void) __asm__("func_80022BEC");
s32 provide_buffer_result(void)
{
    fck_record(buffers_call, mixer.mode, 0, 0);
    return buffers_result;
}

void record_stream_reset(void) __asm__("func_80022CA8");
void record_stream_reset(void) { fck_record(reset_call, 0, 0, 0); }

static const char store_mix_call[] = "store mix";
static const char voice_parameters_call[] = "set voice parameters";
static const char channel_mix_call[] = "submit channel mix";
static s8 effective_volume, effective_pan;
static s32 submitted_pitch;

void record_store_mix(s32 channel, s32 level, s32 pan) __asm__("func_800227BC");
void record_store_mix(s32 channel, s32 level, s32 pan)
{
    fck_record(store_mix_call, channel, level, pan);
    /* Stateful spy matching the valid-fixture contract of src/game/platform/gStoreStreamChannelMix.c:
     * update the selected channel's level/pan before 8002283C reads them back. */
    if (channel >= 0 && channel < 3) {
        mixer.channels[channel].level = level;
        mixer.channels[channel].pan = pan;
    }
}

void provide_voice_mix(struct Voice *voice, s32 pan, s32 volume, s32 extra) __asm__("func_800477D8");
void provide_voice_mix(struct Voice *voice, s32 pan, s32 volume, s32 extra)
{
    fck_record(voice_parameters_call, (s32)voice, pan, volume);
    voice->volume = effective_volume;
    voice->pan = effective_pan;
}

void record_channel_mix(s32 channel, s32 *mix) __asm__("func_800222CC");
void record_channel_mix(s32 channel, s32 *mix)
{
    submitted_pitch = mix[0];
    fck_record(channel_mix_call, channel, mix[1], mix[2]);
}

static const char playing_call[] = "tracked voice playing";
static const char stop_tracked_call[] = "stop tracked voice";
static const char allocate_priority_call[] = "allocate priority voice";
static const char bind_voice_call[] = "bind tracked handle";
static const char validate_id_call[] = "validate tracked id";
static const char start_tracked_call[] = "start tracked voice";
static const char upload_call[] = "upload portrait frame";
static struct VoiceResource voice_resource;
static struct VoiceResource *allocated_resource;
static s32 tracked_playing, valid_tracked_id;

s32 provide_tracked_playing(void) __asm__("func_80024170");
s32 provide_tracked_playing(void)
{
    fck_record(playing_call, 0, 0, 0);
    return tracked_playing;
}

void record_stop_tracked(void) __asm__("func_80023E84");
void record_stop_tracked(void) { fck_record(stop_tracked_call, 0, 0, 0); }

struct VoiceResource *provide_priority_voice(s32 volume, s32 priority, s32 flags, s32 owner) __asm__("func_80046B04");
struct VoiceResource *provide_priority_voice(s32 volume, s32 priority, s32 flags, s32 owner)
{
    fck_record(allocate_priority_call, volume, priority, flags);
    return allocated_resource;
}

void record_bind_voice(struct VoiceHandle *handle, struct VoiceResource *resource) __asm__("func_8004749C");
void record_bind_voice(struct VoiceHandle *handle, struct VoiceResource *resource)
{
    fck_record(bind_voice_call, (s32)handle, (s32)resource, 0);
}

s32 provide_tracked_validation(s32 id) __asm__("func_80045028");
s32 provide_tracked_validation(s32 id)
{
    fck_record(validate_id_call, id, 0, 0);
    return valid_tracked_id;
}

s32 record_start_tracked(void) __asm__("func_80023F08");
s32 record_start_tracked(void)
{
    fck_record(start_tracked_call, 0, 0, 0);
    return 1;
}

void record_portrait_upload(s32 mode, void *data, s32 x, s32 y, s32 flags) __asm__("func_8001BBEC");
void record_portrait_upload(s32 mode, void *data, s32 x, s32 y, s32 flags)
{
    fck_record(upload_call, (s32)data, x, y);
}

static const char stop_preview_call[] = "stop preview";
static const char allocate_preview_call[] = "allocate preview";
static const char play_preview_call[] = "play preview";
static const char update_preview_call[] = "update preview mix";
static s32 allocated_preview, preview_pitch, preview_pan, preview_extra;

void record_stop_preview(s32 handle) __asm__("func_80046A34");
void record_stop_preview(s32 handle) { fck_record(stop_preview_call, handle, 0, 0); }

s32 provide_preview_handle(s32 volume, s32 priority, s32 flags, s32 track) __asm__("func_80046A58");
s32 provide_preview_handle(s32 volume, s32 priority, s32 flags, s32 track)
{
    fck_record(allocate_preview_call, volume, track, priority);
    return allocated_preview;
}

void record_play_preview(s32 track, s32 volume, s32 pan, s32 pitch,
                         s32 handle, void *entity) __asm__("func_80046E88");
void record_play_preview(s32 track, s32 volume, s32 pan, s32 pitch, s32 handle, void *entity)
{
    preview_pitch = pitch;
    preview_pan = pan;
    fck_record(play_preview_call, track, handle, (s32)entity);
}

void record_preview_mix(struct Voice *voice, s32 pan, s32 volume, s32 extra) __asm__("func_80047800");
void record_preview_mix(struct Voice *voice, s32 pan, s32 volume, s32 extra)
{
    preview_extra = extra;
    fck_record(update_preview_call, (s32)voice, pan, volume);
}

static const char stat_call[] = "get player stat";
static const char format_call[] = "format results text";
static const char draw_call[] = "draw results text";
static const char pad_call[] = "translate pad";
static const char tile_call[] = "emit translucent tile";
static const char cache_call[] = "allocate results cache";
static s32 stat_values[16];
static s32 cache_ram[50];
static s32 drawn_colors[32];
static s32 drawn_count;

s32 provide_stat(void *player, s32 id) __asm__("func_80059E9C");
s32 provide_stat(void *player, s32 id)
{
    fck_record(stat_call, (s32)player, id, 0);
    return id >= 0 && id < 16 ? stat_values[id] : -999;
}

/* The format call carries its first argument in a2 and, only for the time
 * format, its second in a3. The fake formatter emits distinct glyphs so we
 * can see which string the drawing code measured and passed on.
 */
void record_format(u8 *out, u8 *format, s32 first, s32 second) __asm__("func_8008CD30");
void record_format(u8 *out, u8 *format, s32 first, s32 second)
{
    s32 is_time = format == time_format;
    fck_record(format_call, (s32)format, first, is_time ? second : 0);
    out[0] = is_time ? 'T' : (format == round_format ? 'R' : 'N');
    out[1] = 0;
}

/* First character identifies the synthetic string; each character is 8 px. */
s32 measure_fixture_text(u8 *text) __asm__("func_80049340");
s32 measure_fixture_text(u8 *text)
{
    s32 width = 0;
    while (*text++) width += 8;
    return width;
}

void record_draw(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
void record_draw(u8 *text, s32 x, s32 y, s32 color)
{
    fck_record(draw_call, *text, x, y);
    if (drawn_count < 32) drawn_colors[drawn_count++] = color;
}

void record_pad(void *out, s32 port) __asm__("func_800576A8");
void record_pad(void *out, s32 port) { fck_record(pad_call, (s32)out, port, 0); }

void record_tile(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");
void record_tile(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend)
{
    fck_record(tile_call, color, w, h);
}

void *provide_cache(s32 size, s32 tag) __asm__("func_80017EBC");
void *provide_cache(s32 size, s32 tag)
{
    fck_record(cache_call, size, tag, 0);
    return cache_ram;
}

static const char preempt_call[] = "preempt portrait stream";
static const char mark_call[] = "mark entity stream request";
static const char mode_call[] = "set volume mode";
static const char ready_call[] = "check stream readiness";
static const char drain_vagm_call[] = "drain VAGM ring";
static const char drain_callbacks_call[] = "drain callback slot ring";
static const char start_transition_call[] = "flush deferred releases";
static s32 is_ready;

void record_preempt(void) __asm__("func_80029DD8");
void record_preempt(void) { fck_record(preempt_call, 0, 0, 0); }

void record_mark(struct Unk41DC4 *entity, s32 id) __asm__("func_80041DC4");
void record_mark(struct Unk41DC4 *entity, s32 id) { fck_record(mark_call, (s32)entity, id, 0); }

void record_mode(s32 mode, void *entity) __asm__("func_80044FF8");
void record_mode(s32 mode, void *entity) { fck_record(mode_call, mode, (s32)entity, 0); }

s32 provide_readiness(void) __asm__("func_80029BE4");
s32 provide_readiness(void)
{
    fck_record(ready_call, 0, 0, 0);
    return is_ready;
}

void record_drain_vagm(void) __asm__("func_8002A49C");
void record_drain_vagm(void) { fck_record(drain_vagm_call, 0, 0, 0); }

void record_drain_callbacks(void) __asm__("func_8002A408");
void record_drain_callbacks(void) { fck_record(drain_callbacks_call, 0, 0, 0); }

void record_start_transition(void) __asm__("func_80029520");
void record_start_transition(void) { fck_record(start_transition_call, stream_id, stream_flags, 0); }

static void expect_call(const char *name, int nth, s32 a0, s32 a1, s32 a2)
{
    struct FckCall call = fck_call_n(name, nth);
    if (call.name != name || call.a0 != a0 || call.a1 != a1 || call.a2 != a2) {
        fck_write("Unexpected arguments or missing call: ");
        fck_write(name);
        fck_write("\n");
    }
    FCK_CHECK(call.name == name && call.a0 == a0 && call.a1 == a1 && call.a2 == a2,
              "recorded call and arguments");
}

static void reset_configuration(s32 result)
{
    lk_fill(&mixer, 0xa5, sizeof(mixer));
    command_slot = stream_callback = stream_slot_a = stream_slot_b = 0x12345678;
    lk_fill(options, 0, sizeof(options));
    option_present = 0;
    buffers_result = result;
    fck_log_reset();
}

static void test_stream_configuration(void)
{
    u32 *words = (u32 *)&mixer;
    static const char *order[] = {
        fill_call, option_call, option_call, option_call, option_call,
        option_call, buffers_call, reset_call
    };

    /* @leaf 80022484 semantic 800244A4 consumes the allocator status and supplies rate, block size/count, channel count and callback as keyed options. */
    LK_FUNC("func_80022484");
    reset_configuration(-17);
    FCK_EQ(gConfigureAudioStreaming(0), -17, "propagates SPU buffer failure to caller");
    fck_expect_order(order, 8, "parse options before allocating and resetting");
    FCK_EQ(words[0x18 / 4], 4096, "nominal rate has unity Q12 pitch");
    FCK_EQ(stream_callback, 0, "no callback without an override");
    FCK_EQ(command_slot | stream_slot_a | stream_slot_b, 0, "old stream state cleared");
    FCK_CHECK(lk_all(&mixer, 0x98, sizeof(mixer), 0xa5), "mixer end canary");

    reset_configuration(9);
    /* The option provider supplies half the nominal sample rate, so
     * unity pitch becomes 0x800 in Q12. */
    option_present = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 6) | (1 << 10);
    options[1] = 22050;
    options[2] = 2048;
    options[3] = 7;
    options[6] = 0x123400;
    options[10] = 2;
    FCK_EQ(gConfigureAudioStreaming(3, 7, 10, 2, 6, 0x123400, 2, 2048, 1, 22050, 0),
           9, "allocator result returned on success");
    expect_call(option_call, 0, 6, 3, 0);
    FCK_EQ(stream_callback, 0x123400, "callback selected by key");
    FCK_EQ(words[0x14 / 4], 2, "two channels configured");
    FCK_EQ(words[0x18 / 4], 2048, "half-rate pitch");
    FCK_EQ(words[0x1c / 4], 22050, "sample rate persisted");
    FCK_EQ(words[0x6c / 4], 2048, "block size persisted");
    FCK_EQ(words[0x70 / 4], 7, "block capacity persisted");
    FCK_EQ(words[0x40 / 4], -1, "first channel starts unassigned");
    FCK_EQ(words[0x64 / 4], -1, "second channel starts unassigned");
    FCK_EQ(mixer.mode, 1, "initialized controller published");
}

static void test_stream_mix(void)
{
    struct Voice voice;
    static const char *live[] = { store_mix_call, voice_parameters_call, channel_mix_call };

    /* @leaf 8002283C semantic 80024730 sets level/pan for each VAGM channel; only live voices should submit an SPU mix without a pitch change (800222CC). */
    LK_FUNC("func_8002283C");
    lk_fill(&mixer, 0, sizeof(mixer));
    fck_log_reset();
    gApplyStreamChannelMix(-5, 41, 17);
    FCK_EQ(fck_log_count, 1, "inactive stream only stores channel mix");
    expect_call(store_mix_call, 0, 0, 41, 17);

    lk_fill(&mixer, 0, sizeof(mixer));
    mixer.active = 1;
    fck_log_reset();
    gApplyStreamChannelMix(1, 85, 26);
    FCK_EQ(fck_log_count, 1, "unbound channel has no SPU update");
    expect_call(store_mix_call, 0, 1, 85, 26);

    lk_fill(&mixer, 0, sizeof(mixer));
    fck_log_reset();
    gApplyStreamChannelMix(3, 77, 15);
    FCK_EQ(fck_log_count, 1, "upper-bound channel 3 clamps to 2 on inactive stream");
    expect_call(store_mix_call, 0, 2, 77, 15);

    lk_fill(&mixer, 0, sizeof(mixer));
    lk_fill(&voice, 0, sizeof(voice));
    mixer.active = 1;
    voice.channel = 12;
    mixer.channels[2].voice = &voice;
    mixer.channels[2].level = 53;
    mixer.channels[2].pan = 31;
    effective_volume = -7;
    effective_pan = -3;
    submitted_pitch = 0;
    fck_log_reset();
    gApplyStreamChannelMix(19, 90, 21);
    fck_expect_order(live, 3, "store, apply effective voice mix, submit");
    expect_call(store_mix_call, 0, 2, 90, 21);
    expect_call(voice_parameters_call, 0, (s32)&voice, 21, 90);
    expect_call(channel_mix_call, 0, 12, -7, -3);
    FCK_EQ(submitted_pitch, -1, "leave existing channel pitch alone");
}

static void reset_tracked_voice(s32 active, s32 playing, s32 valid, struct VoiceResource *resource)
{
    lk_fill(&tracked_voice, 0x55, sizeof(tracked_voice));
    lk_fill(&voice_resource, 0, sizeof(voice_resource));
    cvk_active = active;
    tracked_playing = playing;
    valid_tracked_id = valid;
    allocated_resource = resource;
    fck_log_reset();
}

static void test_tracked_voice(void)
{
    static const char *replacement[] = {
        playing_call, stop_tracked_call, allocate_priority_call, bind_voice_call, validate_id_call
    };

    /* @leaf 80024064 semantic Prepare the tracked voice handle and its delay for the subsequent playback in 80023F08; do not replace it when CVK is off or binding fails. */
    LK_FUNC("func_80024064");
    reset_tracked_voice(0, 1, 1, &voice_resource);
    FCK_EQ(gAcquireTrackedSoundVoice(17, 210), 0, "disabled CVK cannot acquire");
    FCK_EQ(fck_log_count, 0, "disabled CVK touches no voice");
    FCK_CHECK(lk_all(&tracked_voice, 0, sizeof(tracked_voice), 0x55), "disabled handle unchanged");

    reset_tracked_voice(1, 0, 1, 0);
    FCK_EQ(gAcquireTrackedSoundVoice(17, 210), 0, "voice pool exhausted");
    FCK_EQ(fck_calls_to(bind_voice_call), 0, "failed allocation cannot bind an empty handle");
    FCK_EQ(fck_calls_to(validate_id_call), 0, "no selector needed without a voice");
    FCK_EQ(tracked_voice.delay, 0x55555555, "failed allocation did not install a delay");

    reset_tracked_voice(1, 1, 1, &voice_resource);
    FCK_EQ(gAcquireTrackedSoundVoice(17, 210), 1, "acquired replacement voice");
    fck_expect_order(replacement, 5, "release previous voice then allocate and bind replacement");
    expect_call(bind_voice_call, 0, (s32)&tracked_voice, (s32)&voice_resource, 0);
    expect_call(validate_id_call, 0, 17, 0, 0);
    FCK_EQ(tracked_voice.has_selector, 1, "valid mix selector enabled");
    FCK_EQ(tracked_voice.selector, 17, "caller track forwarded to playback");
    FCK_EQ(tracked_voice.deadline, 0, "start will set a fresh deadline");
    FCK_EQ(tracked_voice.delay, 10, "210 bytes represent ten delay ticks at 21 bytes per tick");

    reset_tracked_voice(1, 0, 0, &voice_resource);
    FCK_EQ(gAcquireTrackedSoundVoice(27, 63), 1, "unknown selector can use the default mix");
    FCK_EQ(tracked_voice.has_selector, 0, "unknown selector uses default");
    FCK_EQ(tracked_voice.selector, -1, "invalid selector is not persisted");
    FCK_EQ(tracked_voice.delay, 3, "63 bytes represent three delay ticks");
}

static void test_split_screen(void)
{
    u32 packets[5][7];
    int i;
    /* docs/environment-render-path.md: loading-screen uploads are at VRAM
     * (320,0)/(320,256); cover the screen without a seam or overlap.
     * Packet words below are little-endian GP0/DMA fields: size:8/link:24,
     * y:s16/x:s16, v:u8/u:u8, and height:u16/width:u16.
     */
    static const u32 expected_pages[] = { 0xe1000105, 0xe1000115 };
    static const u32 expected_xy[] = { 0, 160 };

    /* @leaf 8002549C semantic docs/environment-render-path.md loading-screen uploads occupy two 15-bit texture pages; two neutral sprites tile the 320x240 destination. */
    LK_FUNC("func_8002549C");
    lk_fill(packets, 0xa5, sizeof(packets));
    packet_cursor = (u8 *)packets[0];
    packet_limit = (u8 *)packets[4];
    gEmitSplitScreenBlit(0xff123456u);
    FCK_EQ(packet_cursor, packets[4], "four packet slots consumed");
    for (i = 0; i < 2; i++) {
        u32 *page = packets[2 * i];
        u32 *sprite = packets[2 * i + 1];
        FCK_EQ(page[0], 0x01123456, "texpage tag has one word and a 24-bit link");
        FCK_EQ(page[1], expected_pages[i], "15-bit texture page matches VRAM upload");
        FCK_CHECK(lk_all(page, 8, 28, 0xa5), "texpage does not write packet padding");
        FCK_EQ(sprite[0], 0x04123456, "sprite tag has four words and a 24-bit link");
        FCK_EQ(sprite[1], 0x64808080, "variable-size textured sprite with neutral RGB");
        FCK_EQ(sprite[2], expected_xy[i], "two horizontal halves start at x=0 and x=160");
        FCK_EQ(sprite[3] & 0xffff, 0, "starts at the texpage UV origin");
        FCK_EQ(sprite[4], 0x00f000a0, "sprite is 160 by 240");
        FCK_CHECK(lk_all(sprite, 20, 28, 0xa5), "sprite does not write packet padding");
    }
    FCK_CHECK(lk_all(packets[4], 0, 28, 0xa5), "full allocation leaves sentinel slot unused");

    lk_fill(packets, 0xa5, sizeof(packets));
    packet_cursor = (u8 *)packets[0];
    packet_limit = (u8 *)packets[0];
    gEmitSplitScreenBlit(7);
    FCK_EQ(packet_cursor, packets[0], "saturated packet cursor cannot advance");
    FCK_EQ(packets[0][0], 0x04000007, "last primitive reuses fallback slot");
    FCK_EQ(packets[0][2], 160, "fallback contains right sprite");
    FCK_CHECK(lk_all(packets, 28, sizeof(packets), 0xa5), "no write past the fallback slot");
}

static void reset_preview(s32 handle, s32 track, s32 allocated, struct PlayerEntity *entity)
{
    lk_fill(player_entities, 0, sizeof(player_entities));
    player_entities[1] = entity;
    slot_track[0] = 91;
    slot_voice[0] = 92;
    slot_track[1] = track;
    slot_voice[1] = handle;
    allocated_preview = allocated;
    preview_pitch = preview_pan = preview_extra = 100;
    fck_log_reset();
}

static void test_options_preview(void)
{
    struct Voice voice;
    struct PlayerEntity entity;
    static const char *replacement[] = { stop_preview_call, allocate_preview_call, play_preview_call };

    /* @leaf 80025E18 semantic Options volume preview (800260B8) is per player slot; changing track replaces its sound, changing volume alone reuses it, and mute stops it. */
    LK_FUNC("func_80025E18");
    lk_fill(&voice, 0, sizeof(voice));
    lk_fill(&entity, 0, sizeof(entity));
    voice.pan = -11;
    reset_preview((s32)&voice, 8, 23, &entity);
    gUpdateOptionsPreviewVoice(8, 42, 1);
    FCK_EQ(fck_log_count, 1, "same track updates rather than restarts");
    expect_call(update_preview_call, 0, (s32)&voice, -11, 42);
    FCK_EQ(preview_extra, -1, "mix update leaves the other parameter alone");
    FCK_EQ(slot_voice[1], &voice, "retain current voice");

    reset_preview(17, 8, 23, &entity);
    gUpdateOptionsPreviewVoice(11, 51, 1);
    fck_expect_order(replacement, 3, "stop the old sound before starting the new one");
    expect_call(stop_preview_call, 0, 17, 0, 0);
    expect_call(allocate_preview_call, 0, 51, 11, 10);
    expect_call(play_preview_call, 0, 11, 23, (s32)&entity);
    FCK_EQ(preview_pitch, -1, "preview uses the sound's default pitch");
    FCK_EQ(preview_pan, 0, "no preview pan override");
    FCK_EQ(slot_track[1], 11, "new track stored in requested slot");
    FCK_EQ(slot_voice[1], 23, "replacement handle stored in requested slot");
    FCK_EQ(slot_track[0], 91, "other slot track unchanged");
    FCK_EQ(slot_voice[0], 92, "other slot voice unchanged");

    reset_preview(23, 11, 0, &entity);
    gUpdateOptionsPreviewVoice(11, 0, 1);
    expect_call(stop_preview_call, 0, 23, 0, 0);
    FCK_EQ(fck_log_count, 1, "mute cannot allocate or start another voice");
    FCK_EQ(slot_track[1], 0, "mute clears remembered track");
    FCK_EQ(slot_voice[1], 0, "mute clears live handle");

    reset_preview(0, 11, 0, &entity);
    gUpdateOptionsPreviewVoice(12, 51, 1);
    FCK_EQ(fck_log_count, 1, "failed allocation cannot start playback");
    expect_call(allocate_preview_call, 0, 51, 12, 10);
    FCK_EQ(slot_track[1], 0, "failed preview does not claim a playing track");
}

static void reset_results(u8 **table)
{
    results_text = table;
    memcpy(decimal_format, "%d", sizeof(decimal_format));
    memcpy(time_format, "time", sizeof(time_format));
    memcpy(round_format, "%02d", sizeof(round_format));
    lk_fill(cache_ram, 0xa5, sizeof(cache_ram));
    lk_fill(stat_values, 0, sizeof(stat_values));
    lk_fill(drawn_colors, 0xa5, sizeof(drawn_colors));
    drawn_count = 0;
    fck_log_reset();
}

static void test_results_decoder(void)
{
    struct ResultsPlayer player;
    u8 *table[64];
    /* The final stat after the terminator must not be drawn. */
    u8 codes[] = { 2, 0x19, 0x1a, 0x40, 0x4f, 0x1b, 0x40 };
    lk_fill(&player, 0, sizeof(player));
    lk_fill(table, 0, sizeof(table));
    table[2] = (u8 *)"Label";

    /* @leaf 80027F80 semantic Results columns decode labels/control codes and cache player stats for redraw (80028164 and docs/ram-globals.md); derive positions from synthetic glyph widths. */
    LK_FUNC("func_80027F80");
    reset_results(table);
    stat_values[0] = 27;
    stat_values[15] = 18450;  /* 61.5 seconds at 300 ticks/s -> 1:02 */
    results_cache = cache_ram;
    cache_ram[0] = 0;
    cache_ram[1] = 3;
    gDrawResultsTextBlock(&player, codes, 100, 0x12340008);
    expect_call(stat_call, 0, (s32)&player, 0, 0);
    expect_call(stat_call, 1, (s32)&player, 15, 0);
    expect_call(format_call, 0, (s32)decimal_format, 27, 0);
    expect_call(format_call, 1, (s32)time_format, 1, 2);
    /* Widths 40/8/8; an empty row and tightened row consume 9+5 pixels. */
    expect_call(draw_call, 0, 'L', 90, 33);
    expect_call(draw_call, 1, 'N', 98, 56);
    expect_call(draw_call, 2, 'T', 98, 65);
    FCK_EQ(drawn_count, 3, "only label and two stats rendered");
    FCK_EQ(drawn_colors[0], 8, "draw color truncated to its low halfword");
    FCK_EQ(cache_ram[1], 5, "two stats consume two cache words");
    FCK_EQ(cache_ram[3], 27, "raw decimal statistic cached");
    FCK_EQ(cache_ram[4], 18450, "raw timing statistic cached");

    reset_results(table);
    results_cache = cache_ram;
    cache_ram[0] = 1;
    cache_ram[1] = 3;
    cache_ram[3] = 71;
    cache_ram[4] = 18000;  /* exactly one minute */
    gDrawResultsTextBlock(0, codes, 100, 2);
    FCK_EQ(fck_calls_to(stat_call), 0, "a cached redraw does not query the player");
    expect_call(format_call, 0, (s32)decimal_format, 71, 0);
    expect_call(format_call, 1, (s32)time_format, 1, 0);
    FCK_EQ(cache_ram[1], 5, "redraw walks same cache slots");
}

static void reset_results_screen(struct Session *round, struct PlayerEntity *entities,
                                 struct ResultsPlayer *players, u8 **table, u8 *label)
{
    int i;

    reset_results(table);
    lk_fill(table, 0, 64 * sizeof(*table));
    lk_fill(round, 0, sizeof(*round));
    lk_fill(entities, 0, 2 * sizeof(*entities));
    lk_fill(players, 0, 2 * sizeof(*players));
    lk_fill(player_entities, 0, sizeof(player_entities));
    memcpy(label, "Round **", 9);
    table[0] = (u8 *)"Alpha";
    table[1] = (u8 *)"Beta";
    table[2] = label;
    table[3] = (u8 *)"C";
    table[4] = (u8 *)"O";
    table[5] = (u8 *)"I";
    table[0x5c / 4] = (u8 *)"Continue";
    table[0x60 / 4] = (u8 *)"Solo";
    center_codes[0] = 3;
    outer_label_codes[0] = 4;
    inner_label_codes[0] = 5;
    outer_stat_codes[0] = 0x40;
    middle_stat_codes[0] = 0x41;
    inner_stat_codes[0] = 0x42;
    center_codes[1] = outer_label_codes[1] = inner_label_codes[1] = 0x1b;
    outer_stat_codes[1] = middle_stat_codes[1] = inner_stat_codes[1] = 0x1b;
    session = round;
    round->first_team = 2;
    round->second_team = 7;
    round->round = 5;
    for (i = 0; i < 2; i++) {
        player_entities[i] = &entities[i];
        entities[i].player = &players[i];
        players[i].eligible = 1;
    }
    screen_state = 0xd;
    frame_time = 0x20;
    player_count = 2;
    single_player_entity = 0;
    results_cache = 0;
    stat_values[0] = 21;
    stat_values[1] = 32;
    stat_values[2] = 43;
}

static void test_results_screen(void)
{
    struct Session round;
    struct PlayerEntity entities[2];
    struct ResultsPlayer players[2];
    u8 *table[64];
    u8 label[12];

    /* @leaf 80028164 semantic docs/ram-globals.md documents mirrored results columns, the solo round counter and reusable stats; use real decoding with synthetic labels and stat codes. */
    LK_FUNC("func_80028164");
    reset_results_screen(&round, entities, players, table, label);
    gDrawPlayerResultsScreen();
    expect_call(cache_call, 0, 200, 3, 0);
    expect_call(pad_call, 0, (s32)&players[0].buttons, 0, 0);
    expect_call(pad_call, 1, (s32)&players[1].buttons, 1, 0);
    FCK_EQ(fck_calls_to(tile_call), 4, "panel background, both headers, and footer");
    FCK_EQ(drawn_count, 14, "two headers, eleven body lines, footer");
    expect_call(draw_call, 0, 'A', 75, 20);
    expect_call(draw_call, 1, 'B', 227, 20);
    expect_call(draw_call, 2, 'C', 158, 33);
    expect_call(draw_call, 3, 'O', 38, 33);
    expect_call(draw_call, 4, 'O', 278, 33);
    expect_call(stat_call, 0, (s32)&players[0], 0, 0);
    expect_call(stat_call, 3, (s32)&players[1], 0, 0);
    expect_call(draw_call, 13, 'C', 144, 210);
    FCK_EQ(results_cache, cache_ram, "first frame initializes cache");
    FCK_EQ(cache_ram[0], 1, "completed frame publishes cache");
    FCK_EQ(cache_ram[1], 9, "six stats occupy cache after its three-word header");
    FCK_EQ(screen_state, 5, "eligible pair exits results");

    reset_results_screen(&round, entities, players, table, label);
    single_player_entity = 1;
    player_count = 1;
    players[0].eligible = 0;
    gDrawPlayerResultsScreen();
    expect_call(format_call, 0, (s32)round_format, 6, 0);
    FCK_EQ(round.round, 6, "solo round incremented on first frame");
    FCK_EQ(label[6], 'R', "round display gets formatted suffix");
    FCK_EQ(drawn_colors[2], 8, "solo round is bright on the selected blink phase");
    FCK_EQ(fck_calls_to(pad_call), 1, "solo results poll only one pad");
    expect_call(draw_call, 14, 'S', 152, 210);
    FCK_EQ(screen_state, 0xe, "alternate exit when first player not eligible");

    reset_results_screen(&round, entities, players, table, label);
    results_cache = cache_ram;
    cache_ram[0] = 1;
    cache_ram[1] = 9;
    cache_ram[2] = 1;
    lk_fill(&cache_ram[3], 0, 6 * sizeof(s32));
    players[0].still_displaying = players[1].still_displaying = 1;
    gDrawPlayerResultsScreen();
    FCK_EQ(fck_calls_to(cache_call), 0, "subsequent frame reuses results cache");
    FCK_EQ(fck_calls_to(stat_call), 0, "subsequent frame reuses stats");
    expect_call(draw_call, 0, 'B', 77, 20);
    expect_call(draw_call, 1, 'A', 225, 20);
    FCK_EQ(screen_state, 0xd, "two still-displaying players keep results visible");
}

static void reset_portrait(u8 flags)
{
    reset_tracked_voice(1, 0, 1, &voice_resource);
    stream_flags = flags;
    stream_id = 44;
    frame_time = 1710;
    portrait_start_tick = 1600;
    portrait_byte_count[0] = 126;
}

static void test_portrait(void)
{
    s32 track;

    /* @leaf 80029748 semantic HUD caller 800486FC gets the selected track; a portrait with audio only starts its companion voice once without scheduling a VRAM upload. */
    LK_FUNC("func_80029748");
    /* The active animation branch uses the absolute address 0x800A0C30;
     * QEMU user mode cannot load RAM there. Exercise audio without bit 1.
     */
    reset_portrait(0);
    track = -1;
    FCK_EQ(gStepSynchronizedPortrait(&track), 0, "inactive portrait has no animation");
    FCK_EQ(track, 44, "track published even when inactive");
    FCK_EQ(fck_log_count, 0, "inactive portrait does not touch sound or VRAM");

    reset_portrait(0x0c);
    track = -1;
    FCK_EQ(gStepSynchronizedPortrait(&track), 0, "finished portrait is idle");
    FCK_EQ(track, 44, "finished portrait still reports the selected track");
    FCK_EQ(fck_log_count, 0, "finished portrait cannot restart sound");

    reset_portrait(4);
    track = -1;
    FCK_EQ(gStepSynchronizedPortrait(&track), 0, "audio-only portrait has no animation");
    expect_call(validate_id_call, 0, 44, 0, 0);
    FCK_EQ(tracked_voice.delay, 6, "126-byte fixture becomes six voice-delay ticks");
    FCK_EQ(fck_calls_to(start_tracked_call), 1, "companion sound starts on first step");
    FCK_EQ(fck_calls_to(upload_call), 0, "audio-only portrait schedules no upload");
    FCK_EQ(portrait_start_tick, 1600, "audio-only portrait leaves animation clock untouched");
    FCK_EQ(stream_flags, 0x14, "companion sound marked started but no animation");

    reset_portrait(0x14);
    track = -1;
    FCK_EQ(gStepSynchronizedPortrait(&track), 0, "subsequent audio-only step has no animation");
    FCK_EQ(track, 44, "selected track reported on subsequent step");
    FCK_EQ(fck_log_count, 0, "subsequent step does not restart the voice");
}

static void reset_requests(s16 count, s16 head, u8 flags)
{
    lk_fill(&requests, 0x5a, sizeof(requests));
    requests.count = count;
    requests.head = head;
    stream_flags = flags;
    screen_state = 4;
    fck_log_reset();
}

static void test_enqueue(void)
{
    struct Unk41DC4 entity = { 100 };
    static const char *preempt[] = { preempt_call, mark_call };

    /* @leaf 80029A60 semantic 80069D68 supplies normal and priority requests; priority goes ahead of the old head with wraparound, but the eight-slot queue cannot grow past capacity. */
    LK_FUNC("func_80029A60");
    reset_requests(0, 7, 0);
    FCK_EQ(gQueueEntityStreamRequest(0x1234, &entity, 0), 1, "idle normal request accepted");
    expect_call(mark_call, 0, (s32)&entity, 0x1234, 0);
    FCK_EQ(fck_calls_to(preempt_call), 0, "empty queue needs no preemption");
    FCK_EQ(requests.head, 7, "first request becomes head");
    FCK_EQ(requests.count, 1, "one new request");
    FCK_EQ(requests.entries[7].id, 0x1234, "slot has requested ID");
    FCK_EQ(requests.entries[7].entity, &entity, "slot has the requesting entity");
    FCK_EQ(requests.entries[7].has_entity, 1, "slot marked as entity request");
    FCK_CHECK(lk_all(requests.entries, 0, 7 * sizeof(struct Request), 0x5a),
              "enqueue only writes the selected slot");

    reset_requests(1, 0, 0);
    FCK_EQ(gQueueEntityStreamRequest(66, &entity, 0), 0, "normal request cannot displace pending work");
    FCK_EQ(fck_log_count, 0, "rejection does not mark entity or preempt");
    FCK_EQ(requests.count, 1, "pending request still queued");

    reset_requests(0, 2, 0x40);
    FCK_EQ(gQueueEntityStreamRequest(66, &entity, 0), 0, "normal request cannot interrupt live stream");
    FCK_EQ(fck_log_count, 0, "busy stream does not mark entity");

    reset_requests(1, 0, 0x40);
    screen_state = 5;
    FCK_EQ(gQueueEntityStreamRequest(66, &entity, 1), 0, "priority cannot bypass screen-state guard");
    FCK_EQ(fck_log_count, 0, "guard does not preempt existing request");

    reset_requests(0, 3, 0x40);
    FCK_EQ(gQueueEntityStreamRequest(77, &entity, 1), 1, "priority on empty queue accepted without preemption");
    FCK_EQ(fck_calls_to(preempt_call), 0, "empty queue priority request does not preempt");
    expect_call(mark_call, 0, (s32)&entity, 77, 0);
    FCK_EQ(requests.count, 1, "empty priority request increments count to one");
    FCK_EQ(requests.head, 3, "empty priority request keeps head slot");
    FCK_EQ(requests.entries[3].id, 77, "empty priority request stored at head slot");
    FCK_EQ(requests.entries[3].entity, &entity, "empty priority request carries entity");
    FCK_EQ(requests.entries[3].has_entity, 1, "empty priority request marked as entity request");
    FCK_CHECK(lk_all(requests.entries, 0, 3 * sizeof(struct Request), 0x5a),
              "empty priority request leaves earlier slots untouched");

    reset_requests(3, 1, 0x40);
    FCK_EQ(gQueueEntityStreamRequest(88, &entity, 1), 1, "priority on partially full queue accepted");
    fck_expect_order(preempt, 2, "partially full priority preempts before marking");
    expect_call(mark_call, 0, (s32)&entity, 88, 0);
    FCK_EQ(requests.count, 4, "partially full priority increments count");
    FCK_EQ(requests.head, 0, "priority from head 1 steps back to slot 0 without wrapping");
    FCK_EQ(requests.entries[0].id, 88, "slot 0 holds prepended priority request");
    FCK_EQ(requests.entries[0].entity, &entity, "slot 0 carries requesting entity");
    FCK_EQ(requests.entries[0].has_entity, 1, "slot 0 marked as entity request");
    FCK_CHECK(lk_all(requests.entries, sizeof(struct Request), 8 * sizeof(struct Request), 0x5a),
              "partially full priority leaves slots 1..7 untouched");

    reset_requests(8, 0, 0x40);
    FCK_EQ(gQueueEntityStreamRequest(66, &entity, 1), 1, "priority accepted even when full");
    fck_expect_order(preempt, 2, "stop old stream before marking new request");
    expect_call(mark_call, 0, (s32)&entity, 66, 0);
    FCK_EQ(requests.count, 8, "capacity stays eight");
    FCK_EQ(requests.head, 7, "priority wraps to head slot seven");
    FCK_EQ(requests.entries[7].id, 66, "new head contains priority request");
    FCK_EQ(requests.entries[7].entity, &entity, "new head carries requesting entity");
    FCK_CHECK(lk_all(requests.entries, 0, 7 * sizeof(struct Request), 0x5a),
              "priority only replaces the previous tail slot");
}

static s32 chunks[4][3];

static void reset_service(s16 count, s32 current, u8 flags, s32 readiness, s32 gate, void *entity)
{
    reset_requests(count, 7, flags);
    lk_fill(&stream_buffers, 0x55, sizeof(stream_buffers));
    lk_fill(chunks, 0, sizeof(chunks));
    chunks[2][1] = 432;
    resource_chunks = (u8 *)chunks;
    stream_buffers.base = 1000;
    requests.entries[7].id = 2;
    requests.entries[7].has_entity = 1;
    requests.entries[7].entity = entity;
    stream_id = current;
    stream_gate = gate;
    stream_latch = 7;
    stream_deadline = 111;
    stream_guard = 1;
    stream_start_pending[0] = 0;
    frame_ticks = 7000;
    is_ready = readiness;
}

static void test_service(void)
{
    struct Unk41DC4 entity = { 101 };
    static const char *transition[] = {
        mode_call, ready_call, drain_vagm_call, drain_callbacks_call, start_transition_call
    };

    /* @leaf 80029C34 semantic Stream service consumes one head only when it can proceed; a new stream resets progress and gets a deadline consumed by 8002A26C. */
    LK_FUNC("func_80029C34");
    reset_service(0, 3, 0, 1, 1, &entity);
    gServiceStreamRequestQueue();
    FCK_EQ(fck_log_count, 0, "empty queue never consults the stream");
    FCK_EQ(requests.count, 0, "empty queue cannot underflow");

    reset_service(1, 3, 0x40, 1, 1, &entity);
    gServiceStreamRequestQueue();
    FCK_EQ(fck_log_count, 0, "busy stream defers the request");
    FCK_EQ(requests.count, 1, "busy request remains queued");

    reset_service(1, 3, 0, 0, 1, &entity);
    gServiceStreamRequestQueue();
    FCK_EQ(fck_log_count, 2, "not-ready stream only selects mode and checks readiness");
    expect_call(mode_call, 0, 2, (s32)&entity, 0);
    FCK_EQ(requests.count, 1, "unready request remains queued");
    FCK_EQ(requests.head, 7, "unready request keeps head");
    FCK_EQ(stream_id, 3, "current stream remains selected");

    reset_service(2, 2, 0x08, 0, 3, &entity);
    requests.entries[7].has_entity = 0;
    gServiceStreamRequestQueue();
    expect_call(mode_call, 0, -1, 0, 0);
    FCK_EQ(fck_log_count, 1, "same stream does not wait or request a transition");
    FCK_EQ(stream_flags, 0, "same stream acknowledges finished flag");
    FCK_EQ(requests.count, 1, "only one matching request consumed");
    FCK_EQ(requests.head, 0, "dequeue wraps past slot seven");
    FCK_EQ(stream_deadline, 111, "same stream does not refresh timeout");

    reset_service(2, 3, 0, 1, 1, &entity);
    gServiceStreamRequestQueue();
    fck_expect_order(transition, 5, "drain previous stream queues before transition");
    expect_call(start_transition_call, 0, 2, 0x40, 0);
    FCK_EQ(stream_gate, 2, "gate enters transition state");
    FCK_EQ(stream_start_pending[0], 1, "transition flags a pending start");
    FCK_EQ(stream_id, 2, "request becomes current stream");
    FCK_EQ(stream_flags, 0x40, "new stream is busy");
    FCK_EQ(stream_latch, 0, "new stream clears old latch");
    FCK_EQ(stream_guard, 0, "next teardown is enabled");
    FCK_EQ(stream_deadline, 10000, "3000-tick deadline relative to tick 7000");
    FCK_EQ(requests.head, 0, "transition advances queue by one");
    FCK_EQ(requests.count, 1, "one request remains after transition");
    FCK_EQ(stream_buffers.id, 2, "buffer controller takes new stream ID");
    FCK_EQ(stream_buffers.position, 1432, "file base 1000 plus chunk offset 432");
    FCK_EQ(stream_buffers.start_pending | stream_buffers.progress, 0, "stream position state reset");
}

int main(void)
{
    test_stream_configuration();
    test_stream_mix();
    test_tracked_voice();
    test_split_screen();
    test_options_preview();
    test_results_decoder();
    test_results_screen();
    test_portrait();
    test_enqueue();
    test_service();
    return lk_done("leaf_entity");
}
