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

/* gFinalizeMissionLoadout.c uses a pointer view of D_8009C0A8; the
 * callee gClampLoadoutSelections.c uses the three 20-byte row records.
 * Pointer slot 10 aliases the first word of the third row. */
struct LoadoutRow {
    u8 *record_ids;
    u8 *selections;
    u8 unused, replacement, limit;
    u8 pad[9];
};

/* @link 8007D2DC */
static s8 loadout_menu[6];
static s32 session[24];
static u8 special_records[60];
s8 *loadout_menu_ptr __asm__("D_8009F468") = loadout_menu;
s32 *session_ptr __asm__("D_8009FA94") = session;
s32 active_player __asm__("D_8009F8B8");
s32 D_8009F8E0_arr[2];
__asm__(".globl D_8009F8E0\nD_8009F8E0 = D_8009F8E0_arr\n"
        ".globl D_8009F8E4\nD_8009F8E4 = D_8009F8E0_arr + 4");
s16 p1_ready_label[4] __asm__("D_8009B7E6");
s16 p2_ready_label[4] __asm__("D_8009B856");
u8 front_end_mode __asm__("D_8009F45A");
u8 row_clear __asm__("D_8009FA10");
s32 screen_state __asm__("D_8009FA00");
u8 primary_indices[2] __asm__("D_8009F924");
u8 heavy_indices[2] __asm__("D_8009F926");
u8 special_indices[2] __asm__("D_8009F928");
u8 primary_records[60] __asm__("D_8009C030");
u8 heavy_records[60] __asm__("D_8009C050");
struct LoadoutRow loadout_rows[3] __asm__("D_8009C0A8") = {
    { 0, primary_indices, 0, 1, 2, { 0 } },
    { 0, heavy_indices, 0, 1, 2, { 0 } },
    { special_records, special_indices, 0, 1, 2, { 0 } }
};

extern void gFinalizeMissionLoadout(s32, s32, s32, s32, s32)
    __asm__("func_8007E3EC");

static void test_mission_loadout(void)
{
    /* @leaf 8007E3EC source gFinalizeMissionLoadout.c: selection and ready labels, both-player gate, clamp-before-copy and transition globals */
    LK_FUNC("func_8007E3EC");
    loadout_menu[5] = 2;
    loadout_menu[0] = 1;
    loadout_menu[1] = 6;
    active_player = 1;
    D_8009F8E0_arr[0] = 1;
    D_8009F8E0_arr[1] = 1;
    p1_ready_label[0] = 77;
    p2_ready_label[0] = 88;
    p2_ready_label[1] = 99;
    session[9] = 19;
    session[10] = -1;
    session[16] = 26;
    session[23] = 1;
    front_end_mode = 44;
    row_clear = 9;
    screen_state = 8;

    gFinalizeMissionLoadout(0, 100, 101, 102, 6);
    FCK_EQ(D_8009F8E0_arr[1], 1, "matching selection keeps player two ready");
    FCK_EQ(p2_ready_label[0], 110, "matching ready player gets label 110");
    gFinalizeMissionLoadout(0, 100, 101, 102, 7);
    FCK_EQ(D_8009F8E0_arr[1], 0, "changed selection clears player two ready");
    FCK_EQ(p2_ready_label[0], 100, "changed selection resets label to 100");
    FCK_EQ(p2_ready_label[1], 99, "only first label word changes");
    FCK_EQ(D_8009F8E0_arr[0], 1, "player two change leaves player one ready");
    FCK_EQ(p1_ready_label[0], 77, "player two change leaves player one label");

    active_player = 0;
    gFinalizeMissionLoadout(0, 100, 101, 102, 2);
    FCK_EQ(D_8009F8E0_arr[0], 0, "changed selection clears player one ready");
    FCK_EQ(p1_ready_label[0], 100, "changed player one selection resets label");
    FCK_EQ(screen_state, 8, "selection update does not transition");

    primary_indices[0] = 9;
    primary_indices[1] = 2;
    heavy_indices[0] = 0;
    heavy_indices[1] = 9;
    special_indices[0] = 2;
    special_indices[1] = 1;
    primary_records[6] = 31;
    primary_records[12] = 32;
    heavy_records[0] = 40;
    heavy_records[6] = 41;
    special_records[6] = 51;
    special_records[12] = 52;

    gFinalizeMissionLoadout(2, 100, 101, 102, 0);
    FCK_EQ(D_8009F8E0_arr[0], 0, "unhandled op does not mark readiness");
    FCK_EQ(front_end_mode, 44, "unhandled op leaves front-end mode");
    FCK_EQ(screen_state, 8, "unhandled op leaves screen state");

    gFinalizeMissionLoadout(3, 100, 101, 102, 0);
    FCK_EQ(D_8009F8E0_arr[0], 1, "first confirmation marks its player ready");
    FCK_EQ(D_8009F8E0_arr[1], 0, "first confirmation does not mark other player");
    FCK_EQ(primary_indices[0], 9, "waiting for other player does not clamp");
    FCK_EQ(session[10], -1, "waiting does not copy to session");
    FCK_EQ(row_clear, 9, "waiting does not clear row flag");
    FCK_EQ(front_end_mode, 44, "waiting does not assign front-end mode");
    FCK_EQ(screen_state, 8, "waiting leaves the screen unchanged");

    /* Both invalid selections are on highlighted rows. They must still
     * clamp in mode 1, and the copied values must use the new indices. */
    active_player = 1;
    gFinalizeMissionLoadout(3, 100, 101, 102, 0);
    FCK_EQ(D_8009F8E0_arr[1], 1, "second confirmation marks its player ready");
    FCK_EQ(primary_indices[0], 1, "confirm clamps highlighted primary row");
    FCK_EQ(heavy_indices[1], 1, "confirm clamps highlighted heavy row");
    FCK_EQ(session[10], 31, "player one primary copied after clamp");
    FCK_EQ(session[11], 40, "player one heavy copied");
    FCK_EQ(session[12], 52, "player one special copied");
    FCK_EQ(session[13], 32, "player two primary copied");
    FCK_EQ(session[14], 41, "player two heavy copied after clamp");
    FCK_EQ(session[15], 51, "player two special copied");
    FCK_EQ(session[9], 19, "word before session outputs preserved");
    FCK_EQ(session[16], 26, "word after session outputs preserved");
    FCK_EQ(front_end_mode, 2, "session word 23 equal to 1 selects mode 2");
    FCK_EQ(row_clear, 0, "transition clears row flag");
    FCK_EQ(screen_state, 5, "transition sets screen 5");

    loadout_menu[5] = 1;
    session[23] = 0;
    D_8009F8E0_arr[0] = 0;
    D_8009F8E0_arr[1] = 0;
    screen_state = 8;
    gFinalizeMissionLoadout(3, 100, 101, 102, 0);
    FCK_EQ(front_end_mode, 1, "single-player session chooses mode 1");
    FCK_EQ(screen_state, 5, "single-player confirmation transitions without ready flags");
    FCK_EQ(D_8009F8E0_arr[1], 0, "single-player confirmation does not mark readiness");
}

/* Layouts shared by gDrawTargetMarkers.c and the real radar pre-pass. */
struct EffectNode {
    u8 pad00[8];
    s32 count;
    u8 pad0c[0x17];
    u8 type;
    u32 flags;
    u8 pad28[0x28];
    struct Vec pos;
};

struct MarkerExtra {
    u8 pad00[0xa4];
    u32 flags;
    u8 types[8];
    s16 coords[8][2];
};

struct MarkerControl {
    u8 pad00[0xc1];
    s8 mode;
    u8 padc2[0x23e];
    struct MarkerExtra *extra;
};

struct MarkerObject {
    u8 pad00[0xac];
    struct MarkerControl *control;
    s16 heading, padb2;
};

/* @link 8005A9C4 */
/* @link 8005A740 */
/* @link 8005A934 */
/* @link 8005A828 */
/* @link 8001846C */
/* @link 80095C30 */
s32 effect_count[1] __asm__("D_800A8F90");
struct EffectNode *effect_nodes[5] __asm__("D_800A8F98");
u32 frame_time __asm__("D_8009FAA4");
u32 marker_colors[32] __asm__("D_800A6F60");
s32 player_count __asm__("D_8009FA0C");
struct EffectNode *player_entities[2] __asm__("D_8009FA58");
u16 sine_table[0x401] __asm__("D_8009A234");
s16 atan_table[0x401] __asm__("D_8009E0A0");
s16 radius_table[6] __asm__("D_8009AFB4");

extern void gDrawTargetMarkers(struct MarkerObject *) __asm__("func_8004EDC8");

static void test_target_markers(void)
{
    struct MarkerObject object;
    struct MarkerControl control;
    struct MarkerExtra extra;
    struct EffectNode nodes[5];
    struct RadarPacket *p = radar_packets;
    int i;

    /* @leaf 8004EDC8 source gDrawTargetMarkers.c: ordered radar pre-pass, texpage, filtered effect nodes, packed extra slots and show-all flag */
    LK_FUNC("func_8004EDC8");
    lk_fill(&object, 0, sizeof(object));
    lk_fill(&control, 0, sizeof(control));
    lk_fill(&extra, 0, sizeof(extra));
    lk_fill(nodes, 0, sizeof(nodes));
    object.control = &control;
    control.extra = &extra;
    player_count = 1;
    sine_table[0x400] = 0x8000; /* gSinCos produces -65536 cos at heading 0x800. */

    effect_count[0] = 5;
    for (i = 0; i < 5; i++) {
        effect_nodes[i] = &nodes[i];
        nodes[i].count = 1;
        nodes[i].type = 1;
    }
    nodes[0].count = 0;
    nodes[1].type = 0;
    nodes[2].flags = 0x20;
    nodes[3].flags = 0x40;
    nodes[3].pos.x = 8192;
    nodes[3].pos.z = -4096;
    nodes[4].type = (1 << 5) | 24;
    nodes[4].pos.z = 4096;

    /* Bit 2 of each three-bit group selects a slot. Bit 0 in slot 0
     * does not select it here; the pre-pass sees its zero distance. */
    extra.flags = 1 | (4 << 3) | (4 << 21);
    extra.types[1] = (2 << 5) | 3;
    extra.coords[1][0] = 16;
    extra.coords[1][1] = -16;
    extra.types[7] = 25;
    extra.coords[7][0] = -24;
    extra.coords[7][1] = 24;

    marker_colors[1] = 0x112233;
    marker_colors[3] = 0x445566;
    marker_colors[24] = 0x778899;
    marker_colors[25] = 0x123456;

    reset_radar();
    frame_time = 0;
    gDrawTargetMarkers(&object);
    FCK_CHECK(packet_cursor == p + 4, "disabled high types and invalid entries emit no packets");
    FCK_EQ(p[0].color, 0x40808080, "heading pre-pass runs before texpage");
    FCK_EQ(p[0].x0, 16, "heading pre-pass uses side zero origin");
    FCK_EQ(p[0].y0, 44, "heading pre-pass uses object heading");
    FCK_EQ(p[1].tag, 0x01000000, "texpage tag clears address and sets word count");
    FCK_EQ(p[1].color, 0xe100069e, "texpage command precedes markers");
    FCK_CHECK(lk_all(&p[1], 8, 28, 0xa5), "texpage writes only first two words");
    FCK_EQ(p[2].color, 0x66112233, "eligible node color and shape");
    FCK_EQ(p[2].x0, 37, "node X uses small side offset");
    FCK_EQ(p[2].y0, 44, "node Y uses node position");
    FCK_EQ(p[3].color, 0x28445566, "slot one appears after effect nodes");
    FCK_EQ(p[3].x0, 41, "slot one scales and switches stored Z into marker X");
    FCK_EQ(p[3].y0, 39, "slot one scales and switches stored X into marker Y");
    FCK_CHECK(lk_all(p + 4, 0, 4 * sizeof(*p), 0xa5), "no other packets touched");

    reset_radar();
    control.mode = 1;
    frame_time = 0x80;
    gDrawTargetMarkers(&object);
    FCK_CHECK(packet_cursor == p + 6, "show-all adds one node and one extra");
    FCK_EQ(p[0].x0, 96, "heading pre-pass uses side one origin");
    FCK_EQ(p[1].color, 0xe100069e, "texpage follows pre-pass on side one");
    FCK_EQ(p[2].color, 0x66112233, "first effect stays first under show-all");
    FCK_EQ(p[2].x0, 117, "nonzero control mode uses large side offset");
    FCK_EQ(p[3].color, 0x20778899, "high node indexes color and selects triangle");
    FCK_EQ(p[3].x0, 120, "high node passes position and large offset");
    FCK_EQ(p[4].color, 0x28445566, "low extra follows both nodes");
    FCK_EQ(p[4].x0, 121, "low extra uses large offset");
    FCK_EQ(p[5].color, 0x66123456, "slot seven appears last");
    FCK_EQ(p[5].x0, 116, "slot seven projects signed Z");
    FCK_EQ(p[5].y0, 45, "slot seven projects signed X");

    reset_radar();
    packet_limit = p;
    effect_count[0] = 0;
    control.extra = 0;
    gDrawTargetMarkers(&object);
    FCK_CHECK(packet_cursor == p, "texpage at limit does not advance cursor");
    FCK_EQ(p[0].color, 0xe100069e, "texpage still writes at limit");
    FCK_CHECK(lk_all(p + 1, 0, 7 * sizeof(*p), 0xa5), "empty lists leave later packets alone");
}

/* gFindGroundObjectAtPosition.c: the radius-test center is the triple
 * immediately after matrix[11]; the remaining six bytes precede the radius.
 * The table has a 16-byte prefix after its header for transform index 1. */
struct GroundBox {
    s16 x, y, z, half_x, half_y, half_z;
    u8 stride_padding[12];
};

struct GroundTable {
    s32 count, stride;
    u8 index_prefix[16];
    struct GroundBox boxes[4];
};

struct GroundModel {
    u8 pad00[0x34];
    struct GroundTable *table;
};

struct GroundTransform {
    struct GroundModel *model;
    s32 matrix[12];
    struct Vec radius_center;
    u8 pad40[6];
    s16 radius, index;
};

struct GroundObject {
    u8 pad00[0x24];
    u32 flags;
    struct GroundTransform transform;
};

struct GroundEntry {
    struct GroundObject *object;
    u8 id;
    u8 pad05[3];
};

/* @link 8001A750 */
/* @link 80018860 */
/* @link 80019470 */
/* @link 8001A81C */
/* @link 8001A970 */
s32 ground_count __asm__("D_8009F838");
struct GroundEntry ground_entries[4] __asm__("D_800A31A0");

extern struct GroundObject *gFindGroundObjectAtPosition(struct Vec *, struct Vec *, s32 *, u8 *)
    __asm__("func_8006320C");

static void test_ground_object(void)
{
    static struct GroundTable table = {
        4, 24, { 0 }, {
            { 9, 16, 19, 1, 4, 1, { 0 } }, /* would match, but skipped */
            { 5, 60, 19, 1, 4, 1, { 0 } }, /* X outside */
            { 9, 61, 19, 1, 4, 1, { 0 } }, /* Y one grid step too high */
            { 9, 60, 19, 1, 4, 1, { 0 } }  /* X/Z edges and exact Y threshold */
        }
    };
    struct GroundModel model;
    struct GroundObject objects[4];
    struct Vec pos = { 1000, 535, -300 };
    struct Vec out = { -11, -22, -33 };
    s32 height = 123;
    u8 id = 0xfe;
    int i;

    /* @leaf 8006320C source gFindGroundObjectAtPosition.c: flags and strict radius, indexed strided boxes, first hit, affine height rounding and optional output */
    LK_FUNC("func_8006320C");
    model.table = &table;
    lk_fill(objects, 0, sizeof(objects));
    for (i = 0; i < 4; i++) {
        struct GroundTransform *t = &objects[i].transform;
        objects[i].flags = 0x20000000;
        t->model = &model;
        t->matrix[0] = t->matrix[4] = t->matrix[8] = 0x10000;
        t->matrix[9] = 840;
        t->matrix[10] = 151;
        t->matrix[11] = -620;
        t->radius_center = pos;
        t->radius = 16;
        t->index = 1;
        ground_entries[i].object = &objects[i];
        ground_entries[i].id = 40 + i;
    }
    objects[0].flags = 0x10000000;
    objects[1].transform.radius_center.x += 256;

    ground_count = 0;
    FCK_CHECK(gFindGroundObjectAtPosition(&pos, &out, &height, &id) == 0,
              "empty ground queue misses");
    ground_count = 2;
    FCK_CHECK(gFindGroundObjectAtPosition(&pos, &out, &height, &id) == 0,
              "wrong flag and exact-radius candidates rejected");
    FCK_EQ(height, 123, "miss preserves height");
    FCK_EQ(id, 0xfe, "miss preserves ID");
    FCK_EQ(out.x, -11, "miss preserves optional X");
    FCK_EQ(out.y, -22, "miss preserves optional Y");
    FCK_EQ(out.z, -33, "miss preserves optional Z");

    /* Inverse translation yields local (160, 384, 320). The fourth
     * box's top is (60+4)*16, then the forward translation adds 151:
     * world height 1175 rounds up to 1280. Entry 3 would also match. */
    ground_count = 4;
    FCK_CHECK(gFindGroundObjectAtPosition(&pos, &out, &height, &id) == &objects[2],
              "first eligible matching object returned");
    FCK_EQ(height, 1280, "hit transforms and rounds strided box top");
    FCK_EQ(id, 42, "hit returns first matching entry ID");
    FCK_EQ(out.x, 0, "optional X comes from matrix word 3");
    FCK_EQ(out.y, 0x10000, "optional Y comes from matrix word 4");
    FCK_EQ(out.z, 0, "optional Z comes from matrix word 5");

    pos.y--;
    height = 321;
    id = 0xfd;
    FCK_CHECK(gFindGroundObjectAtPosition(&pos, 0, &height, &id) == 0,
              "one unit below vertical threshold misses all tested boxes");
    FCK_EQ(height, 321, "box miss preserves height");
    FCK_EQ(id, 0xfd, "box miss preserves ID");

    pos.y++;
    table.count = 1;
    FCK_CHECK(gFindGroundObjectAtPosition(&pos, 0, &height, &id) == &objects[2],
              "single box is tested and null optional output is accepted");
    FCK_EQ(height, 512, "single box height includes translation and rounding");
    FCK_EQ(id, 42, "null optional output still writes matching ID");
}

int main(void)
{
    test_actor_mode();
    test_radar_marker();
    test_mission_loadout();
    test_target_markers();
    test_ground_object();
    return lk_done("leaf_ai_r4");
}
