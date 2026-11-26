/* Exercise the real range/cone -> opponent scan -> player lock-on chain.
 * Only the GTE SquareRoot12 entry point is replaced. Coordinates are chosen
 * to give exact roots in its Q12 output; no GTE table or initial RAM needed.
 */
#include "leaf_kit.h"

typedef struct Entity Entity;

typedef struct {
    u8 pad00[0x68];
    s32 target_id;
    Entity *target;
    Entity *exclude;
} LockOn;

struct Entity {
    u8 pad00[8];
    s32 id;
    u8 pad0C[0x0A];
    u8 type;
    u8 pad17;
    u8 team;
    u8 pad19[0x0B];
    u32 flags;
    u8 pad28[0x28];
    s32 x;
    u8 pad54[4];
    s32 z;
    u8 pad5C[0x1C];
    s32 heading_x;
    u8 pad7C[4];
    s32 heading_z;
    u8 pad84[0x1C];
    s32 alternate_x;
    u8 padA4[4];
    s32 alternate_z;
    LockOn *lock;
};

enum { Q16 = 0x10000, LOCK_RANGE = 0x18000, LOCK_CONE = 0xF9EE };

struct {
    s32 count;
    s32 pad04;
    Entity *items[8];
} candidates __asm__("D_800A8F90");

s32 gCheckTargetRangeAndCone(Entity *, Entity *, s32, s32) __asm__("func_8004E5F0");
Entity *gFindNearestOpposingEntity(Entity *, Entity *, s32, s32) __asm__("func_8006C224");
void gAcquirePlayerLockOnTarget(Entity *, s32) __asm__("func_8006C34C");

static s32 sqrt_calls, sqrt_input, unexpected_squares;

s32 fixture_sqrt12(s32 value) __asm__("func_80095900");
s32 fixture_sqrt12(s32 value)
{
    sqrt_calls++;
    sqrt_input = value;
    switch (value) {
    case 0:       return 0;
    case 4096:    return 4096;    /* (1/4)^2 */
    case 16384:   return 8192;    /* (1/2)^2 */
    case 36864:   return 12288;   /* (3/4)^2 */
    case 65536:   return 16384;   /* 1^2 */
    case 147456:  return 24576;   /* (3/2)^2 */
    case 1638400: return 81920;   /* 3^2 + 4^2 */
    default:
        unexpected_squares++;
        return 0;
    }
}

static void init_entity(Entity *entity, s32 id, s32 team, s32 x, s32 z)
{
    lk_fill(entity, 0, sizeof(*entity));
    entity->id = id;
    entity->team = team;
    entity->x = x;
    entity->z = z;
}

static void test_range_and_cone(void)
{
    Entity seeker, target;

    /* @leaf 8004E5F0 source retail fixed-point range/cone control with exact square-root fixtures */
    LK_FUNC("func_8004E5F0");
    init_entity(&seeker, 80, 1, 2 * Q16, -2 * Q16);
    init_entity(&target, 81, 2, seeker.x + Q16, seeker.z);
    seeker.heading_x = Q16;
    seeker.alternate_z = Q16;
    sqrt_calls = 0;
    sqrt_input = -1;
    unexpected_squares = 0;

    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, Q16 + 1, Q16), Q16,
           "forward at cone equality, one LSB inside range");
    FCK_EQ(sqrt_calls, 1, "forward candidate reaches square root");
    FCK_EQ(sqrt_input, 65536, "unit displacement squared in Q16");
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, Q16, -Q16), -1,
           "distance exactly at the range cap is rejected");
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, Q16 + 1, Q16 + 1), -1,
           "forward projection below a stricter threshold is rejected");

    target.x = seeker.x - Q16;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, 0), -1,
           "behind the heading is rejected");
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, -Q16), Q16,
           "the disabled cone accepts a target behind the heading");

    target.x = seeker.x;
    target.z = seeker.z + Q16;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, 0), Q16,
           "perpendicular candidate lies exactly on a zero threshold");
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, 1), -1,
           "normal heading rejects perpendicular candidate at positive threshold");
    seeker.type = 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, Q16), Q16,
           "type 1 instead faces along the alternate heading");

    target.z = seeker.z;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, Q16), 0,
           "coincident positions bypass the cone and normalization");

    target.x = seeker.x + 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, 1, Q16), 0,
           "one LSB at the box limit rounds to zero distance");

    target.x = seeker.x + 3 * Q16;
    target.z = seeker.z + 4 * Q16;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, 5 * Q16 + 1, -Q16), 5 * Q16,
           "3-4 displacement has distance 5");
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, 4 * Q16 + 1, -Q16), -1,
           "a diagonal can pass the range box but fail the radial cap");

    sqrt_calls = 0;
    target.z = seeker.z;
    target.x = seeker.x + LOCK_RANGE + 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, -Q16), -1,
           "positive x outside range box");
    target.x = seeker.x - LOCK_RANGE - 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, -Q16), -1,
           "negative x outside range box");
    target.x = seeker.x;
    target.z = seeker.z + LOCK_RANGE + 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, -Q16), -1,
           "positive z outside range box");
    target.z = seeker.z - LOCK_RANGE - 1;
    FCK_EQ(gCheckTargetRangeAndCone(&seeker, &target, LOCK_RANGE, -Q16), -1,
           "negative z outside range box");
    FCK_EQ(sqrt_calls, 0, "all four range-box misses bypass square root");
    FCK_EQ(unexpected_squares, 0, "all in-box square-root inputs have fixtures");
}

static void test_nearest_opponent(void)
{
    Entity seeker, ally, inactive, hidden, side, far, near, tied;
    int i;
    static const u32 rejected_flags[] = {1, 4, 0x20000};

    /* @leaf 8006C224 source opponent and eligibility filters, ordered nearest scan and exclusion */
    LK_FUNC("func_8006C224");
    init_entity(&seeker, 80, 1, 0, 0);
    seeker.heading_x = Q16;
    init_entity(&ally, 81, 1, Q16 / 4, 0);
    init_entity(&inactive, 0, 2, Q16 / 4, 0);
    init_entity(&hidden, 83, 2, Q16 / 4, 0);
    init_entity(&side, 84, 2, 0, Q16 / 4);
    init_entity(&far, 85, 2, Q16, 0);
    init_entity(&near, 86, 2, Q16 / 2, 0);
    init_entity(&tied, 87, 2, Q16 / 2, 0);
    unexpected_squares = 0;

    lk_fill(&candidates, 0, sizeof(candidates));
    FCK_EQ(gFindNearestOpposingEntity(&seeker, 0, LOCK_RANGE, LOCK_CONE), 0,
           "empty candidate list has no target");
    candidates.count = 8;
    candidates.items[0] = &seeker;
    candidates.items[1] = &ally;
    candidates.items[2] = &inactive;
    candidates.items[3] = &hidden;
    candidates.items[4] = &side;
    candidates.items[5] = &far;
    candidates.items[6] = &near;
    candidates.items[7] = &tied;

    for (i = 0; i < sizeof(rejected_flags) / sizeof(rejected_flags[0]); i++) {
        hidden.flags = rejected_flags[i];
        FCK_EQ(gFindNearestOpposingEntity(&seeker, 0, LOCK_RANGE, LOCK_CONE), &near,
               "ignore ally, zero id, each rejection flag and out-of-cone candidate");
    }
    FCK_EQ(gFindNearestOpposingEntity(&seeker, &near, LOCK_RANGE, LOCK_CONE), &tied,
           "excluding the first equally near opponent selects the second");
    inactive.id = -1;
    FCK_EQ(gFindNearestOpposingEntity(&seeker, &tied, LOCK_RANGE, LOCK_CONE), &near,
           "a negative id is also inactive");
    hidden.flags = 0x2;
    FCK_EQ(gFindNearestOpposingEntity(&seeker, 0, LOCK_RANGE, LOCK_CONE), &hidden,
           "an unrelated flag does not exclude the closest opponent");
    FCK_EQ(gFindNearestOpposingEntity(&seeker, 0, Q16 / 4, LOCK_CONE), 0,
           "no eligible opponent strictly inside the range cap");
    FCK_EQ(unexpected_squares, 0, "candidate square-root inputs have fixtures");
}

static void test_player_lock_on(void)
{
    Entity player, near, far, cap;
    LockOn control;

    /* @leaf 8006C34C source initial acquisition, mode-byte cycling, recycled ids and no replacement */
    LK_FUNC("func_8006C34C");
    init_entity(&player, 91, 1, 0, 0);
    player.type = 1;
    player.alternate_x = Q16;
    init_entity(&near, 92, 2, Q16 / 2, 0);
    init_entity(&far, 93, 2, 3 * Q16 / 4, 0);
    init_entity(&cap, 94, 2, LOCK_RANGE, 0);
    lk_fill(&control, 0, sizeof(control));
    player.lock = &control;
    control.target_id = -7;
    control.exclude = &near;
    lk_fill(&candidates, 0, sizeof(candidates));
    candidates.count = 3;
    candidates.items[0] = &cap;
    candidates.items[1] = &far;
    candidates.items[2] = &near;
    unexpected_squares = 0;

    gAcquirePlayerLockOnTarget(&player, 1);
    FCK_EQ(control.target, &near, "initial acquisition drops stale exclusion");
    FCK_EQ(control.target_id, 92, "initial acquisition snapshots target id");
    FCK_EQ(control.exclude, 0, "initial acquisition leaves no exclusion");

    gAcquirePlayerLockOnTarget(&player, 0x100);
    FCK_EQ(control.target, &far, "a zero low byte cycles to the other opponent");
    FCK_EQ(control.target_id, 93, "cycling snapshots replacement id");
    FCK_EQ(control.exclude, &near, "cycle retains the excluded previous target");

    far.id = 97;
    gAcquirePlayerLockOnTarget(&player, 0x101);
    FCK_EQ(control.target, &far, "nonzero mode uses the existing exclusion for this scan");
    FCK_EQ(control.target_id, 97, "reacquisition refreshes the id of the same slot");
    FCK_EQ(control.exclude, 0, "nonzero mode clears exclusion on success");

    candidates.count = 2;
    gAcquirePlayerLockOnTarget(&player, 0);
    FCK_EQ(control.target, &far, "failed cycle preserves the previous target");
    FCK_EQ(control.target_id, 97, "failed cycle preserves the previous id");
    FCK_EQ(control.exclude, &far, "failed cycle still records the previous target");
    candidates.count = 1;
    candidates.items[0] = &near;
    control.target = 0;
    control.target_id = -7;
    player.alternate_x = LOCK_CONE - 2;
    gAcquirePlayerLockOnTarget(&player, 1);
    FCK_EQ(control.target, 0, "initial scan just outside the lock-on cone finds nothing");
    FCK_EQ(control.target_id, -7, "failure to find an initial target leaves the id unchanged");

    player.alternate_x = LOCK_CONE;
    gAcquirePlayerLockOnTarget(&player, 1);
    FCK_EQ(control.target, &near, "initial scan exactly on the lock-on cone accepts the target");
    FCK_EQ(control.target_id, 92, "cone-boundary acquisition stores the id");
    FCK_EQ(unexpected_squares, 0, "lock-on square-root inputs have fixtures");
}

int main(void)
{
    test_range_and_cone();
    test_nearest_opponent();
    test_player_lock_on();
    return lk_done("leaf_lock_on");
}
