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

int main(void)
{
    test_8003526C();
    test_800491F8();
    return lk_done("leaf_ui_r4");
}
