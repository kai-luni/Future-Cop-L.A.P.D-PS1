/* Game/UI leaf suite (round 4): unchanged original MIPS C units run under
 * qemu-mipsel. Expectations are derived from the cited source, not sampled
 * from the implementation. All five entry points are void; we check their
 * observable state and their calls to the recording stand-ins.
 */
#include "leaf_kit.h"

/* @leaf 8003526C source gResetDroneApproach.c:4-39,62-75 counter restore, AI reset and ordered callbacks */
struct DroneState {
    u8 pad00[0x18];
    s16 counter;
    u8 pad1A[4];
    s16 threshold;
    u16 shortSpeed;
    u8 pad22[2];
    s32 speed;
};

struct Drone {
    u8 pad00[0x1C];
    s16 counter;
    s16 initialCounter;
    u8 active;
    u8 pad21[3];
    u32 flags;
    u8 pad28[0xCC];
    s32 speed;
    s16 shortSpeed;
    u8 phase;
    u8 padFB[0x39];
    struct DroneState *state;
};

struct Session {
    u8 pad00[0xE0];
    s32 mode;
};

struct Session *gGameSession __asm__("D_8009FA94");
extern void gResetDroneApproach(struct Drone *self) __asm__("func_8003526C");

static const char quartile_call[] = "func_8003BA50";
static const char random_call[] = "func_80019668";
static const char position_call[] = "func_800330E8";
static s32 random_result;

void mock_quartile(struct Drone *self, s32 oldCounter) __asm__("func_8003BA50");
void mock_quartile(struct Drone *self, s32 oldCounter)
{
    fck_record(quartile_call, (s32)self, oldCounter, self->counter);
}

s32 mock_random(void) __asm__("func_80019668");
s32 mock_random(void)
{
    fck_record(random_call, 0, 0, 0);
    return random_result;
}

void mock_position(struct Drone *self) __asm__("func_800330E8");
void mock_position(struct Drone *self)
{
    fck_record(position_call, (s32)self, self->phase, self->state->counter);
}

static void check_drone_reset(u8 active, u32 flags, s32 reports_quartile)
{
    static const char *with_quartile[] = {quartile_call, random_call, position_call};
    static const char *without_quartile[] = {random_call, position_call};
    struct Drone drone, expected_drone;
    struct DroneState state, expected_state;
    struct Session session, expected_session;
    struct FckCall call;

    lk_fill(&drone, 0xA5, sizeof(drone));
    lk_fill(&state, 0x5A, sizeof(state));
    lk_fill(&session, 0xC3, sizeof(session));
    drone.counter = -1234;
    drone.initialCounter = -21;
    drone.active = active;
    drone.flags = flags;
    drone.state = &state;
    state.counter = 17;
    state.shortSpeed = 0xFEDC;
    state.speed = 0x12345678;
    gGameSession = &session;
    random_result = 0x173;
    fck_log_reset();

    /* Lines 64-75: only these fields are written. -21 >> 1 is -11 on
     * MIPS, and 0x173 & 0x3F is 51, so the threshold becomes -62. */
    memcpy(&expected_drone, &drone, sizeof(drone));
    memcpy(&expected_state, &state, sizeof(state));
    memcpy(&expected_session, &session, sizeof(session));
    expected_drone.counter = -21;
    expected_drone.phase = 0;
    expected_drone.speed = 0x12345678;
    expected_drone.shortSpeed = (s16)0xFEDC;
    expected_state.threshold = -62;
    expected_state.counter = 0;
    expected_session.mode = 4;

    gResetDroneApproach(&drone);

    /* Lines 65-72: quartile sees the restored counter and the signed old
     * value; position sees the cleared phase but the not-yet-cleared AI counter. */
    if (reports_quartile) {
        fck_expect_order(with_quartile, 3, "drone active call order");
        call = fck_call_n(quartile_call, 0);
        FCK_EQ(call.a0, (s32)&drone, "quartile self");
        FCK_EQ(call.a1, -1234, "quartile old signed counter");
        FCK_EQ(call.a2, -21, "counter restored before quartile");
    } else {
        fck_expect_order(without_quartile, 2, "drone suppressed-quartile call order");
    }
    call = fck_call_n(position_call, 0);
    FCK_EQ(call.a0, (s32)&drone, "position self");
    FCK_EQ(call.a1, 0, "phase cleared before position");
    FCK_EQ(call.a2, 17, "AI counter not yet cleared at position");
    FCK_EQ(state.threshold, -62, "signed half-counter minus masked random bits");
    FCK_EQ(memcmp(&drone, &expected_drone, sizeof(drone)), 0, "drone exact writes");
    FCK_EQ(memcmp(&state, &expected_state, sizeof(state)), 0, "AI state exact writes");
    FCK_EQ(memcmp(&session, &expected_session, sizeof(session)), 0, "session exact writes");
    FCK_EQ((s32)gGameSession, (s32)&session, "session pointer untouched");
}

static void test_8003526C(void)
{
    LK_FUNC("func_8003526C");
    /* Line 66: either inactive or flag 0x40000 independently suppresses quartile. */
    check_drone_reset(2, 0x80000, 1);
    check_drone_reset(0, 0, 0);
    check_drone_reset(2, 0x40000, 0);
}

/* @leaf 800491F8 source gDrawFormattedHudNumber.c:4-8,29-35 formatter-to-draw handoff and low-halfword color */
char gFormatStringB[8] __asm__("D_8009F368");
extern void gDrawFormattedHudNumber(s32 value, s32 x, s32 y, s32 color)
    __asm__("func_800491F8");

static const char format_call[] = "func_8008CD30";
static const char draw_call[] = "func_80049100";
static const char format_fixture[8] = "%d";
static const char *format_text;
static s32 format_result;
static s32 draw_color;
static s32 draw_has_text;

s32 mock_format_number(char *buffer, char *format, s32 value) __asm__("func_8008CD30");
s32 mock_format_number(char *buffer, char *format, s32 value)
{
    fck_record(format_call, (s32)buffer, (s32)format, value);
    if (buffer)
        memcpy(buffer, format_text, 8);
    return format_result;
}

void mock_draw_number(char *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
void mock_draw_number(char *text, s32 x, s32 y, s32 color)
{
    struct FckCall format = fck_call_n(format_call, 0);
    fck_record(draw_call, (s32)text, x, y);
    draw_color = color;
    draw_has_text = text && (s32)text == format.a0 && memcmp(text, format_text, 8) == 0;
}

static void check_hud_number(s32 value, s32 input_color, s32 expected_color,
                             const char *text, s32 result)
{
    static const char *order[] = {format_call, draw_call};
    struct FckCall format, draw;

    memcpy(gFormatStringB, format_fixture, sizeof(gFormatStringB));
    format_text = text;
    format_result = result;
    draw_color = -1;
    draw_has_text = 0;
    fck_log_reset();

    gDrawFormattedHudNumber(value, -37, 149, input_color);

    /* Lines 31-35: format first; the exact same populated local buffer is
     * sent to draw at the caller's coordinates, regardless of format result.
     * The format global is passed by address and the wrapper does not write it. */
    fck_expect_order(order, 2, "HUD number call order");
    format = fck_call_n(format_call, 0);
    draw = fck_call_n(draw_call, 0);
    FCK_CHECK(format.a0 != 0 && format.a0 != (s32)gFormatStringB,
              "formatter gets a separate output buffer");
    FCK_EQ(format.a1, (s32)gFormatStringB, "formatter gets shared format pointer");
    FCK_EQ(format.a2, value, "formatter gets signed number");
    FCK_EQ(draw.a0, format.a0, "draw gets the formatter buffer");
    FCK_EQ(draw_has_text, 1, "draw sees the formatter output");
    FCK_EQ(draw.a1, -37, "draw gets signed x");
    FCK_EQ(draw.a2, 149, "draw gets y");
    FCK_EQ(draw_color, expected_color, "draw color is low unsigned halfword");
    FCK_EQ(memcmp(gFormatStringB, format_fixture, sizeof(gFormatStringB)), 0,
           "shared HUD format bytes untouched");
}

static void test_800491F8(void)
{
    static const char negative_text[8] = "-1234";
    static const char zero_text[8] = "0";

    LK_FUNC("func_800491F8");
    check_hud_number(-1234, (s32)0xBADC8123U, 0x8123, negative_text, -1);
    check_hud_number(0, (s32)0xCAFE0000U, 0, zero_text, 0);
}

/* @leaf 80027D70 source gRefreshSoloFrameHud.c:4-7,30-35 single-player equality gate and HUD argument */
s32 gPlayerCount __asm__("D_8009FA0C");
extern void gRefreshSoloFrameHud(void) __asm__("func_80027D70");

static const char close_hud_call[] = "func_800486FC";

void mock_close_hud(s32 arg) __asm__("func_800486FC");
void mock_close_hud(s32 arg)
{
    fck_record(close_hud_call, arg, 0, 0);
}

static void test_80027D70(void)
{
    static const s32 counts[] = {1, 0, 2, -1, 0x10001};
    static const char *order[] = {close_hud_call};
    struct FckCall call;
    int i;

    LK_FUNC("func_80027D70");
    /* Lines 32-34: only exact equality of the full player-count word calls
     * the HUD update, with constant 1. No path writes the count. */
    for (i = 0; i < (int)(sizeof(counts) / sizeof(counts[0])); i++) {
        gPlayerCount = counts[i];
        fck_log_reset();

        gRefreshSoloFrameHud();

        if (i == 0) {
            fck_expect_order(order, 1, "solo frame calls the HUD once");
            call = fck_call_n(close_hud_call, 0);
            FCK_EQ(call.a0, 1, "solo HUD refresh argument");
        } else {
            fck_expect_order(0, 0, "non-solo frame does not call the HUD");
        }
        FCK_EQ(gPlayerCount, counts[i], "player count untouched");
    }
}

/* @leaf 8005A154 source gPlayGaugeEdgeSound.c:4-17,44-47 unsigned effect byte and player-slot word forwarding */
struct GaugeOwner {
    u8 pad00[0xC1];
    s8 side;
    u8 guard[4];
};

struct Gauge {
    u8 pad00[0x13];
    u8 sound;
    u8 guard[4];
};

s32 gPlayerEntities[2] __asm__("D_8009FA58");
extern void gPlayGaugeEdgeSound(struct GaugeOwner *owner, struct Gauge *gauge)
    __asm__("func_8005A154");

static const char gauge_sound_call[] = "func_80046C84";
static s32 gauge_sound_result;

s32 mock_gauge_sound(s32 id, s32 player) __asm__("func_80046C84");
s32 mock_gauge_sound(s32 id, s32 player)
{
    fck_record(gauge_sound_call, id, player, 0);
    return gauge_sound_result;
}

static void check_gauge_sound(s8 side, u8 sound, s32 slot_word, s32 result)
{
    static const s32 players[] = {(s32)0x8F123456U, 0x23456789};
    static const char *order[] = {gauge_sound_call};
    struct GaugeOwner owner, before_owner;
    struct Gauge gauge, before_gauge;
    struct FckCall call;

    lk_fill(&owner, 0xA5, sizeof(owner));
    lk_fill(&gauge, 0x5A, sizeof(gauge));
    owner.side = side;
    gauge.sound = sound;
    memcpy(&before_owner, &owner, sizeof(owner));
    memcpy(&before_gauge, &gauge, sizeof(gauge));
    memcpy(gPlayerEntities, players, sizeof(gPlayerEntities));
    gauge_sound_result = result;
    fck_log_reset();

    gPlayGaugeEdgeSound(&owner, &gauge);

    /* Line 46: one sound call forwards an unsigned byte at +0x13 and
     * the whole selected player word. Neither input record nor the slot
     * table is modified, and the callee result is discarded. */
    fck_expect_order(order, 1, "gauge calls sound once");
    call = fck_call_n(gauge_sound_call, 0);
    FCK_EQ(call.a0, sound, "gauge sound id is unsigned byte");
    FCK_EQ(call.a1, slot_word, "gauge owner selects the player-table word");
    FCK_EQ(memcmp(&owner, &before_owner, sizeof(owner)), 0, "gauge owner untouched");
    FCK_EQ(memcmp(&gauge, &before_gauge, sizeof(gauge)), 0, "gauge record untouched");
    FCK_EQ(memcmp(gPlayerEntities, players, sizeof(gPlayerEntities)), 0,
           "gauge player table untouched");
}

static void test_8005A154(void)
{
    LK_FUNC("func_8005A154");
    check_gauge_sound(0, 0xE3, (s32)0x8F123456U, 0);
    check_gauge_sound(1, 0x34, 0x23456789, 17);
}

int main(void)
{
    test_8003526C();
    test_800491F8();
    test_80027D70();
    test_8005A154();
    return lk_done("leaf_ui_r4");
}
