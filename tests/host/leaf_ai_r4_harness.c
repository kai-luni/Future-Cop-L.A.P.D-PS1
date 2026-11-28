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

/* gDrawRadarMarker.c writes up to 24 bytes but reserves 28 per packet. */
struct Vec {
    s32 x, y, z;
};

struct RadarPacket {
    u32 tag, color;
    s16 x0, y0, x1, y1, x2, y2, x3, y3;
    u32 tail;
};

static struct RadarPacket radar_packets[8];
struct RadarPacket *packet_cursor __asm__("D_8009FA90");
struct RadarPacket *packet_limit __asm__("D_8009F9C0");
s32 radar_sin __asm__("D_8009F7E0");
s32 radar_cos __asm__("D_8009F7E4");
s32 radar_z __asm__("D_8009F7E8");
s32 radar_x __asm__("D_8009F7EC");
u16 radar_uv __asm__("D_800A8EA6");

extern s32 gDrawRadarMarker(struct Vec *, u32, s32, s32)
    __asm__("func_8005B088");

static void reset_radar(void)
{
    lk_fill(radar_packets, 0xa5, sizeof(radar_packets));
    packet_cursor = radar_packets;
    packet_limit = radar_packets + 8;
    radar_sin = 0;
    radar_cos = 0x10000;
    radar_x = 0;
    radar_z = 0;
    radar_uv = 0x1234;
}

static void test_radar_marker(void)
{
    struct Vec marker;
    struct RadarPacket *p = radar_packets;

    /* @leaf 8005B088 source gDrawRadarMarker.c: projected packets for all shape branches, strict distance culling and cursor limit */
    LK_FUNC("func_8005B088");
    reset_radar();
    radar_sin = 0x10000;
    radar_cos = 0;
    radar_x = 300;
    radar_z = -700;
    marker.x = radar_x + 8192;
    marker.y = 919;
    marker.z = radar_z - 8192;

    /* Quarter-turn: xx = -3, yy = -3 after scaling and signed shifts. */
    FCK_EQ(gDrawRadarMarker(&marker, 0x112233, 12, 0), 1, "sprite accepted");
    FCK_CHECK(packet_cursor == p + 1, "sprite reserves 28 bytes");
    FCK_EQ(p[0].tag, 0x04000000, "sprite tag");
    FCK_EQ(p[0].color, 0x66112233, "sprite color and opcode");
    FCK_EQ(p[0].x0, 37, "sprite X includes side offset");
    FCK_EQ(p[0].y0, 41, "sprite Y from rotated X");
    FCK_EQ((u16)p[0].x1, 0xc980, "sprite fixed UV bytes");
    FCK_EQ(p[0].y1, 0x1234, "sprite UV word from global");
    FCK_EQ(p[0].x2, 3, "sprite width");
    FCK_EQ(p[0].y2, 3, "sprite height");
    FCK_CHECK(lk_all(&p[0], 20, 28, 0xa5), "sprite leaves trailing bytes alone");

    FCK_EQ(gDrawRadarMarker(&marker, 0x445566, 92, 1), 1, "triangle accepted");
    FCK_CHECK(packet_cursor == p + 2, "triangle reserves 28 bytes");
    FCK_EQ(p[1].tag, 0x04000000, "triangle tag");
    FCK_EQ(p[1].color, 0x20445566, "triangle color and opcode");
    FCK_EQ(p[1].x0, 118, "triangle first X");
    FCK_EQ(p[1].y0, 40, "triangle first Y");
    FCK_EQ(p[1].x1, 122, "triangle second X");
    FCK_EQ(p[1].y1, 44, "triangle second Y");
    FCK_EQ(p[1].x2, 114, "triangle third X");
    FCK_EQ(p[1].y2, 44, "triangle third Y");
    FCK_CHECK(lk_all(&p[1], 20, 28, 0xa5), "triangle leaves trailing bytes alone");

    /* Equality disables advance, not the write into the current slot. */
    packet_limit = packet_cursor;
    FCK_EQ(gDrawRadarMarker(&marker, 0x778899, 12, 2), 1, "quad accepted at limit");
    FCK_CHECK(packet_cursor == p + 2, "quad at limit does not advance");
    FCK_EQ(p[2].tag, 0x05000000, "quad tag");
    FCK_EQ(p[2].color, 0x28778899, "quad color and opcode");
    FCK_EQ(p[2].x0, 38, "quad first X");
    FCK_EQ(p[2].y0, 39, "quad first Y");
    FCK_EQ(p[2].x1, 41, "quad second X");
    FCK_EQ(p[2].y1, 42, "quad second Y");
    FCK_EQ(p[2].x2, 35, "quad third X");
    FCK_EQ(p[2].y2, 42, "quad third Y");
    FCK_EQ(p[2].x3, 38, "quad fourth X");
    FCK_EQ(p[2].y3, 45, "quad fourth Y");
    FCK_CHECK(lk_all(&p[2], 24, 28, 0xa5), "quad leaves tail alone");

    reset_radar();
    marker.x = 0;
    marker.z = -0x25556;
    FCK_EQ(gDrawRadarMarker(&marker, 0x112233, 12, 0), 0, "negative Z beyond limit");
    marker.x = 0x25556;
    marker.z = 0;
    FCK_EQ(gDrawRadarMarker(&marker, 0x112233, 12, 0), 0, "positive X beyond limit");
    marker.x = 0x20000;
    marker.z = 0x20000;
    FCK_EQ(gDrawRadarMarker(&marker, 0x112233, 12, 0), 0, "diagonal exceeds combined limit");
    FCK_CHECK(packet_cursor == p, "culling preserves cursor");
    FCK_CHECK(lk_all(p, 0, sizeof(radar_packets), 0xa5), "culling writes no packets");

    marker.x = -0x25555;
    marker.z = 0;
    FCK_EQ(gDrawRadarMarker(&marker, 0x112233, 12, 0), 1, "exact distance limit accepted");
    FCK_CHECK(packet_cursor == p + 1, "exact distance limit consumes packet");
}

int main(void)
{
    test_actor_mode();
    test_radar_marker();
    return lk_done("leaf_ai_r4");
}
