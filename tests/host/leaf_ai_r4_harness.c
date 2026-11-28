/* QEMU checks of unchanged game units using synthetic fixtures. Expectations
 * come from the cited C bodies; they pin source behavior, not retail parity.
 */
#include "leaf_kit.h"

/* gSetActorMode.c and gSelectAnimationClip.c: actor and track layouts. */
struct ActorPlayback {
    u8 pad[0x10];
    s32 enabled;
};

struct ActorClip {
    u8 pad[4];
    s16 start, end;
    u8 tail[8];
};

struct ActorTrack {
    struct ActorClip *clips, *clip;
    s32 elapsed;
    s16 frame, target;
    s8 step;
    u8 index, count, flags;
};

struct Actor {
    u8 pad00[0x24];
    u32 flags;
    struct ActorPlayback *playback;
    u8 pad2c[0x78 - 0x2c];
    s32 x, y, z, counter;
    u8 pad88[0xc8 - 0x88];
    struct ActorTrack track;
    u8 mode, control;
    s16 idle_duration, reverse_duration, timer;
    u8 padE4[4];
    s32 speed;
    u8 padEC[0x100 - 0xec];
    s32 saved_x, saved_y, saved_z;
    u8 clip_indices[4];
};

extern void gSetActorMode(struct Actor *, s32) __asm__("func_8002E2BC");

/* @link 800448D8 */
/* @link 80019668 */
s32 prng_cursor __asm__("D_8009F5A8");
s32 prng_state[20] __asm__("D_800A0480");

static void test_actor_mode(void)
{
    struct Actor actor;
    struct ActorPlayback playback;
    struct ActorClip clips[4];

    /* @leaf 8002E2BC source gSetActorMode.c: unchanged-mode guard, clip selection, jitter, fixed duration and saved-position restore */
    LK_FUNC("func_8002E2BC");
    lk_fill(&actor, 0, sizeof(actor));
    lk_fill(clips, 0, sizeof(clips));
    actor.playback = &playback;
    playback.enabled = 1;
    actor.track.clips = clips;
    actor.track.flags = 0x40;
    actor.clip_indices[0] = 1;
    actor.clip_indices[1] = 2;
    actor.clip_indices[2] = 0;
    actor.clip_indices[3] = 3;
    clips[1].start = 12;
    clips[1].end = 18;
    clips[3].start = 40;
    clips[3].end = 21;
    actor.flags = 0xa000;
    actor.control = 2;
    actor.mode = 1;
    actor.timer = 313;
    actor.idle_duration = 10;
    actor.reverse_duration = 12;
    actor.counter = 77;
    actor.speed = -45;
    actor.saved_x = 101;
    actor.saved_y = -202;
    actor.saved_z = 303;
    prng_cursor = 5;
    prng_state[4] = 6;
    prng_state[7] = 7;

    gSetActorMode(&actor, 1);
    FCK_EQ(actor.timer, 313, "same mode preserves timer");
    FCK_EQ(actor.counter, 77, "same mode preserves counter");
    FCK_CHECK(actor.track.clip == 0, "same mode leaves clip alone");
    FCK_EQ(prng_cursor, 5, "same mode leaves PRNG alone");

    /* Entering mode 0 selects +0x10c, sets 0x81, and adds 13 % 5. */
    gSetActorMode(&actor, 0);
    FCK_EQ(actor.mode, 0, "idle mode stored");
    FCK_EQ(actor.flags, 0xa081, "idle enables both actor bits");
    FCK_EQ(actor.timer, 13, "idle duration plus jitter");
    FCK_EQ(actor.counter, 0, "idle clears counter");
    FCK_EQ(prng_cursor, 4, "idle consumes one PRNG word");
    FCK_EQ(prng_state[4], 13, "PRNG updates its decremented slot");
    FCK_CHECK(actor.track.clip == &clips[1], "idle selects clip at +0x10c");
    FCK_EQ(actor.track.frame, 12, "idle starts selected clip");
    FCK_EQ(actor.track.target, 18, "idle passes positive direction");
    FCK_EQ(actor.track.flags, 0x40, "idle passes zero clip flags");

    actor.control = 0x32;
    gSetActorMode(&actor, 2);
    FCK_EQ(actor.mode, 2, "reverse mode stored");
    FCK_EQ(actor.flags, 0xa000, "leaving idle clears only 0x81");
    FCK_EQ(actor.timer, 12, "control bit 0x20 suppresses jitter");
    FCK_EQ(prng_cursor, 4, "reverse does not consume suppressed jitter");
    FCK_CHECK(actor.track.clip == &clips[3], "reverse selects clip at +0x10f");
    FCK_EQ(actor.track.frame, 40, "reverse clip starts at 40");
    FCK_EQ(actor.track.target, 21, "reverse clip targets its end");
    FCK_EQ(actor.track.step, -1, "descending clip reverses positive direction");
    FCK_EQ(actor.track.flags, 0x41, "reverse passes clip flag 1");

    /* Bit 0x10 in +0xdd participates in the mode-2 exit mask 0x1400. */
    playback.enabled = 0;
    gSetActorMode(&actor, 3);
    FCK_EQ(actor.mode, 3, "fixed mode stored");
    FCK_EQ(actor.x, 101, "mode-2 exit restores X");
    FCK_EQ(actor.y, -202, "mode-2 exit restores Y");
    FCK_EQ(actor.z, 303, "mode-2 exit restores Z");
    FCK_EQ(actor.timer, 0x4650, "fixed mode timer");
    /* The expression t - ((t >> 31) & t) clamps negatives to zero. */
    FCK_EQ(actor.counter, 0, "fixed mode clamps negative speed to zero");
    FCK_CHECK(actor.track.clip == &clips[3], "disabled playback preserves clip");

    playback.enabled = 1;
    actor.speed = 45;
    gSetActorMode(&actor, 1);
    FCK_EQ(actor.mode, 1, "mode one stored");
    FCK_EQ(actor.counter, 45, "fixed mode retains positive speed");
    FCK_CHECK(actor.track.clip == &clips[2], "mode one selects clip at +0x10d");

    actor.idle_duration = -3;
    gSetActorMode(&actor, 0);
    FCK_EQ(actor.timer, 0, "negative duration clears timer");
    FCK_EQ(actor.counter, 0, "negative duration clears counter");
    FCK_EQ(prng_cursor, 4, "negative duration does not consume jitter");
}

int main(void)
{
    test_actor_mode();
    return lk_done("leaf_ai_r4");
}
