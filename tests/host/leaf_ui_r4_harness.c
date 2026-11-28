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

int main(void)
{
    test_8003526C();
    return lk_done("leaf_ui_r4");
}
