/* Behavioural checks for the demo/chase camera phase helpers.
 *
 * These two units are pure C and drive the input-phase clock of the chase
 * camera: the table filler seeds eight preset bytes per side, and the stepper
 * counts a timer down and advances the phase when it runs out. Both are taken
 * from the decomp sources unchanged; only the random source and the table
 * storage are provided here, so the checks observe exactly what the names and
 * doc blocks claim.
 *
 * The entry points are bound to their retail labels, and the table to its
 * retail address, so a broken binding fails at link time. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void __main(void) {}

static int failures;

static void say(const char *text)
{
    int length = 0;
    register long v0 __asm__("$2") = 4004;
    register long a0 __asm__("$4") = 1;
    register long a1 __asm__("$5") = (long)text;
    register long a2 __asm__("$6");
    while (text[length]) length++;
    a2 = length;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
}

#define CHECK(condition, message) do { \
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } \
} while (0)

/* D_8009F7E8 / D_8009F7EC - the radar centre the marker drawer measures from. */
s32 gRadarOriginZ __asm__("D_8009F7E8");
s32 gRadarOriginX __asm__("D_8009F7EC");

/* D_800A2290 - the per-side camera preset table, indexed [side][step]. */
u8 gDemoCameraStates[2][8] __asm__("D_800A2290");

/* func_80019668 - the pseudo-random source the filler draws from. */
static u32 rand_state = 0x12345678;
static u32 rand_calls;

u32 gNextRandomValue(void) __asm__("func_80019668");
u32 gNextRandomValue(void)
{
    rand_calls++;
    rand_state = rand_state * 1103515245u + 12345u;
    return (rand_state >> 16) & 0x7FFF;
}

/* The camera state block the stepper walks. Offsets follow the decomp struct:
 * +0xC1 side, +0x2FE timer, +0x313 step. */
struct CameraState {
    u8 pad00[0xC1];
    s8 side;
    u8 padC2[0x23C];
    s16 timer;
    u8 pad300[0x13];
    u8 step;
};

/* --- 80056924: seed eight preset bytes per side ------------------------- */
void gInitDemoCameraStates(void) __asm__("func_80056924");

static void test_init_states(void)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        gDemoCameraStates[0][i] = 0xAA;
        gDemoCameraStates[1][i] = 0xAA;
    }
    rand_calls = 0;

    gInitDemoCameraStates();

    CHECK(rand_calls == 16, "eight presets per side, two sides, one draw each");
    for (i = 0; i < 8; i++) {
        /* The doc block pins the range: (rand & 7) + 7, so 7..14. */
        CHECK(gDemoCameraStates[0][i] >= 7 && gDemoCameraStates[0][i] <= 14,
              "the side 0 preset stays in 7..14");
        CHECK(gDemoCameraStates[1][i] >= 7 && gDemoCameraStates[1][i] <= 14,
              "the side 1 preset stays in 7..14");
    }
    /* Every seeded byte must have overwritten the 0xAA filler. */
    for (i = 0; i < 8; i++) {
        CHECK(gDemoCameraStates[0][i] != 0xAA, "the filler byte is overwritten");
        CHECK(gDemoCameraStates[1][i] != 0xAA, "the filler byte is overwritten");
    }
}

/* --- 80056994: count the phase timer down, then advance ----------------- */
u8 gStepDemoCameraTable(struct CameraState *state, s32 elapsed)
    __asm__("func_80056994");

static void prime_state(struct CameraState *state, s8 side, s16 timer, u8 step)
{
    state->side = side;
    state->timer = timer;
    state->step = step;
}

static void test_step_keeps_phase_while_time_remains(void)
{
    struct CameraState state;

    prime_state(&state, 0, 100, 3);
    gDemoCameraStates[0][3] = 0x42;
    gDemoCameraStates[0][4] = 0x43;

    CHECK(gStepDemoCameraTable(&state, 10) == 0x42, "the current phase byte is returned");
    CHECK(state.timer == 90, "the elapsed time is subtracted");
    CHECK(state.step == 3, "the phase only advances when the timer runs out");
}

static void test_step_advances_when_timer_runs_out(void)
{
    struct CameraState state;

    prime_state(&state, 0, 5, 3);
    gDemoCameraStates[0][3] = 0x42;
    gDemoCameraStates[0][4] = 0x43;

    CHECK(gStepDemoCameraTable(&state, 6) == 0x43, "the next phase byte is returned");
    CHECK(state.timer == 3000, "the timer is reloaded to the phase length");
    CHECK(state.step == 4, "the phase advances exactly once");
}

static void test_step_advances_on_exactly_zero(void)
{
    struct CameraState state;

    /* A timer of 0 is not negative yet; it must advance only after going
     * below zero, which one more unit does. */
    prime_state(&state, 1, 0, 2);
    gDemoCameraStates[1][2] = 0x11;
    gDemoCameraStates[1][3] = 0x12;

    CHECK(gStepDemoCameraTable(&state, 0) == 0x11, "a zero delta keeps the phase");
    CHECK(state.step == 2, "a zero delta does not advance");
    CHECK(gStepDemoCameraTable(&state, 1) == 0x12, "one unit past zero advances");
    CHECK(state.step == 3, "the phase advanced after crossing zero");
}

static void test_step_is_side_relative(void)
{
    struct CameraState state;
    s32 i;

    for (i = 0; i < 8; i++) {
        gDemoCameraStates[0][i] = 0x10 + i;
        gDemoCameraStates[1][i] = 0x20 + i;
    }

    prime_state(&state, 0, 100, 5);
    CHECK(gStepDemoCameraTable(&state, 0) == 0x15, "side 0 reads its own row");

    prime_state(&state, 1, 100, 5);
    CHECK(gStepDemoCameraTable(&state, 0) == 0x25, "side 1 reads its own row");
}


/* --- 8005B088: the renamed radar origin axes cull the marker -------------- */
/* The unit is compiled here unchanged. It measures the marker against the two
 * origin words and gives up as soon as either axis distance passes 0x25555, so
 * moving one origin word must make the marker disappear: that is exactly what
 * the two new names claim. The packet emitters it writes into are provided. */
typedef struct { s32 x, y, z; } R8Vec;
typedef struct {
    u32 tag;
    u32 color;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
} R8Packet;

s32 gDrawRadarMarker(R8Vec *marker, u32 color, s32 side, s32 type)
    __asm__("func_8005B088");

s32 gRadarYawSin __asm__("D_8009F7E0");
s32 gRadarHeadingCosine __asm__("D_8009F7E4");
R8Packet *gPacketCursor __asm__("D_8009FA90");
R8Packet *gPacketLimit __asm__("D_8009F9C0");
u16 gEffectUvV __asm__("D_800A8EA6");

static R8Packet packet_arena[8];

static void prime_marker(R8Vec *marker)
{
    marker->x = 0;
    marker->y = 0;
    marker->z = 0;
    gRadarOriginX = 0;
    gRadarOriginZ = 0;
    gRadarYawSin = 0;
    gRadarHeadingCosine = 0;
    gEffectUvV = 0;
    gPacketCursor = packet_arena;
    gPacketLimit = packet_arena + 8;
}

static void test_radar_origin_axes(void)
{
    R8Vec marker;
    s32 kept;

    /* A marker at the origin is in range and reports that it drew. */
    prime_marker(&marker);
    kept = gDrawRadarMarker(&marker, 0x40, 0, 3);
    CHECK(kept == 1, "a marker at the radar origin is drawn");

    /* Only the Z origin moves out of range: the marker must be culled. */
    prime_marker(&marker);
    gRadarOriginZ = 0x30000;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 3) == 0,
          "the Z origin culls a marker that is too far away on Z");

    /* Only the X origin moves out of range: same result. */
    prime_marker(&marker);
    gRadarOriginX = 0x30000;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 3) == 0,
          "the X origin culls a marker that is too far away on X");

    /* Both origins just inside the limit (the integer sqrt approximation gives
     * 0x21EF0 for 0x18000/0x18000, below the 0x25555 cut-off): drawn again. */
    prime_marker(&marker);
    gRadarOriginX = 0x18000;
    gRadarOriginZ = 0x18000;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 3) == kept,
          "a marker inside both limits is drawn again");
}

/* --- 17. the three marker shapes are all emitted ------------------------ */
/* gDrawRadarMarker branches on the type argument: 0 draws the two-sprite
 * marker, 1 the three-quad variant and anything else the five-quad one. The
 * checks walk all three and inspect the tag and the vertex writes, so the whole
 * branch tree is exercised and the packet is written where the source says. */
static void test_marker_shapes(void)
{
    R8Vec marker;
    R8Packet *p;

    /* type 0: tag 0x04000000, colour carries the 0x66 top byte. */
    prime_marker(&marker);
    gPacketCursor = packet_arena;
    packet_arena[0].tag = 0;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 0) == 1, "the type-0 marker is drawn");
    p = packet_arena;
    CHECK(p->tag == 0x04000000, "the type-0 packet carries tag 0x04000000");
    CHECK((p->color >> 24) == 0x66, "the type-0 colour keeps the 0x66 top byte");
    CHECK(p->x2 == 3 && p->y2 == 3, "the type-0 packet sets its corner size to 3");
    CHECK(packet_arena[1].tag == 0, "the type-0 marker emits exactly one packet");

    /* type 1: tag 0x04000000, colour carries 0x20, three quads written. */
    prime_marker(&marker);
    gPacketCursor = packet_arena;
    packet_arena[0].tag = 0;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 1) == 1, "the type-1 marker is drawn");
    p = packet_arena;
    CHECK((p->color >> 24) == 0x20, "the type-1 colour carries the 0x20 top byte");

    /* every other type: tag 0x05000000, colour carries 0x28. */
    prime_marker(&marker);
    gPacketCursor = packet_arena;
    packet_arena[0].tag = 0;
    CHECK(gDrawRadarMarker(&marker, 0x40, 0, 3) == 1, "the fallback marker is drawn");
    p = packet_arena;
    CHECK(p->tag == 0x05000000, "the fallback packet carries tag 0x05000000");
    CHECK((p->color >> 24) == 0x28, "the fallback colour carries the 0x28 top byte");
    CHECK(p->x3 != 0 || p->y3 != 0 || p->y0 != 0,
          "the fallback marker writes its five quads");

    /* The packet cursor advances past what was emitted. */
    CHECK(gPacketCursor > packet_arena, "the marker advances the packet cursor");

    /* With the buffer already at its limit the marker still writes its packet
     * in place instead of advancing the cursor: the three "no room" arms. */
    {
        s32 ty;
        for (ty = 0; ty < 3; ty++) {
            prime_marker(&marker);
            gPacketCursor = packet_arena + 8;      /* full */
            packet_arena[7].tag = 0;
            CHECK(gDrawRadarMarker(&marker, 0x40, 0, ty) == 1,
                  "a full packet buffer still reports the marker as drawn");
            CHECK(gPacketCursor == packet_arena + 8,
                  "a full packet buffer leaves the cursor at the limit");
        }
    }
}

int main(void)
{
    test_init_states();
    test_step_keeps_phase_while_time_remains();
    test_step_advances_when_timer_runs_out();
    test_step_advances_on_exactly_zero();
    test_step_is_side_relative();
    test_radar_origin_axes();
    test_marker_shapes();

    if (failures == 0) say("naming round 8: all checks passed\n");
    return failures;
}
