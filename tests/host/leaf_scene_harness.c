/* Source characterizations of unchanged src/game scene/camera/spawn/grid
 * helper units on synthetic RAM. Every fixture and global is initialized
 * explicitly before each check; no reliance on retail initial memory.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* =========================================================================
 * Shared struct layouts and globals (defined with retail D_ labels,
 * explicitly reset inside each test).
 * ========================================================================= */

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct SceneGameSession {
    u8  pad00[0x40];
    s32 unk40;
    s32 unk44;
    u8  pad48[0x14];
    s32 unk5C;
    u8  pad60[4];
    s32 unk64[0x100];
};

struct GridObj {
    char pad0[8];
    s32 active;
    char padc[0x18];
    s32 flags;
    char pad28[0x34];
    s32 z;
    s32 y;
    s32 x;
    s32 radius;
};

struct GridList {
    s32 count;
    s32 pad;
    struct GridObj *items[16];
};

struct GridNode {
    struct GridObj *obj;
    struct GridNode *next;
};

struct GridPair {
    s32 x;
    s32 z;
};

struct GridCell {
    u32 bits;
    s32 pad[3];
};

struct TileDefList {
    s8  count;
    u8  pad[3];
    u32 values[16];
};

/* Camera / player / session globals */
s32 gFocusPositions                        __asm__("D_8009F9A0");
s32 gScreenState                           __asm__("D_8009FA00");
void *gMapTileAnimList                     __asm__("D_8009FA04");
s32 gPlayerCount                           __asm__("D_8009FA0C");
void *gPlayerEntities[4]                   __asm__("D_8009FA58");
s32 gScreenResetFlag                       __asm__("D_8009FA70");
u8  gPlayerInvulnerable[4]                 __asm__("D_8009FA8C");
struct SceneGameSession *gGameSession      __asm__("D_8009FA94");
s32 gFrameTime                             __asm__("D_8009FAA4");

/* Registry / grid / map globals */
void *gRegistryTable                       __asm__("D_8009F800");
void **gTileBlockTable                     __asm__("D_8009F808");
void * volatile gMapGridHeader             __asm__("D_8009F80C");
void *gMapChunkNode                        __asm__("D_8009F810");
s32 gViewCount                             __asm__("D_8009F814");
struct GridObj **gCellEntityList           __asm__("D_8009F818");
u8 *gCellMap                               __asm__("D_8009F820");
s32 gCellEntityCount                       __asm__("D_8009F828");
u8  gGridOriginX                           __asm__("D_8009F834");
u8  gGridOriginY                           __asm__("D_8009F835");
void *gSinglePlayerEntity                  __asm__("D_8009F9EC");

/* Effect / pattern / table globals */
void *gDomeEffectBlock[4]                  __asm__("D_800A22F0");
u8  gTilePatternBuf[256]                   __asm__("D_800A2630");
u8  gTilePatternCounterState[512]          __asm__("D_800A2730");
u8  gSlfxFlagByte[4]                       __asm__("D_800A2931");
s32 gMapGridBounds[6]                      __asm__("D_800A2950");
s32 gCellClipRect[6]                       __asm__("D_800A2970");
struct GridNode gCellNodeArray[32]         __asm__("D_800A2990");
struct GridPair gViewGroundAnchors[2]      __asm__("D_800A3190");
struct TileDefList gTileDefTable           __asm__("D_800A6F38");
s32 gMarkerColorTable[32]                  __asm__("D_800A6F60");
struct GridList gCommandListHead           __asm__("D_800A8F90");
struct GridCell gCoverageGrid[30]          __asm__("D_800AB3B8");

/* Handler address symbols referenced by spawn constructors */
u8 gKind59GlowActorHandler[4]              __asm__("D_8006FF44");
u8 gAttachMessageName[4]                   __asm__("D_80075698");

/* =========================================================================
 * External stand-ins (bound by retail func_ label; record calls and return
 * test-configured values, never re-implementing game logic).
 * ========================================================================= */

void mock_sincos(s32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void mock_sincos(s32 angle, s32 *sine, s32 *cosine)
{
    fck_record("sincos", angle, 0, 0);
    switch (angle) {
    case 0:     *sine = 0;        *cosine = 0x10000;  break;
    case 0x400: *sine = 0x10000;  *cosine = 0;        break;
    case 0x200: *sine = 0x8000;   *cosine = 0x10000;  break;
    case 0x201: *sine = -0x8000;  *cosine = 0x10000;  break;
    case 0x202: *sine = 0x8000;   *cosine = -0x10000; break;
    case 0x203: *sine = -0x8000;  *cosine = -0x10000; break;
    case 0x204: *sine = 0x8000;   *cosine = 0xC000;   break;
    case 0x300: *sine = 0x8000;   *cosine = 0x8000;   break;
    default:    *sine = 0;        *cosine = 0x10000;  break;
    }
}

s32 mock_sine_lookup(u32 a) __asm__("func_80018534");
s32 mock_sine_lookup(u32 a)
{
    return ((s32)a - 0x800) << 4;
}

static void *mock_alloc_ret;
void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    fck_record("alloc", size, tag, 0);
    return mock_alloc_ret;
}

void mock_free(void *block) __asm__("func_800181D4");
void mock_free(void *block)
{
    fck_record("free", (s32)block, 0, 0);
}

static u32 mock_rng_seq[8];
static int mock_rng_idx;
u32 mock_rng(void) __asm__("func_80019668");
u32 mock_rng(void)
{
    u32 v = mock_rng_seq[mock_rng_idx & 7];
    mock_rng_idx++;
    fck_record("rng", (s32)v, 0, 0);
    return v;
}

struct MockWork {
    u8 pad[0x18];
    s32 x;
    s32 y;
    s32 z;
};
static s32 mock_rot_x, mock_rot_y, mock_rot_z;
void mock_build_rot(s32 x, s32 y, s32 z, struct MockWork *work) __asm__("func_800188D0");
void mock_build_rot(s32 x, s32 y, s32 z, struct MockWork *work)
{
    fck_record("rot_mat", x, y, z);
    work->x = mock_rot_x;
    work->y = mock_rot_y;
    work->z = mock_rot_z;
}

void mock_add_shake(void *obj, s32 amount) __asm__("func_80057358");
void mock_add_shake(void *obj, s32 amount)
{
    fck_record("add_shake", (s32)obj, amount, 0);
}

void mock_flag_quartile(void *obj, s32 value) __asm__("func_8003BA50");
void mock_flag_quartile(void *obj, s32 value)
{
    fck_record("flag_quartile", (s32)obj, value, 0);
}

void mock_emit_line(s32 origin, s32 id, s32 unused) __asm__("func_8005A740");
void mock_emit_line(s32 origin, s32 id, s32 unused)
{
    fck_record("emit_line", origin, id, unused);
}

static u8  mock_bearing_ret[8];
static s32 mock_bearing_ang[8];
static int mock_bearing_idx;
u8 mock_check_bearing(void *obj, s32 z, s32 x, s32 *angle) __asm__("func_8005A934");
u8 mock_check_bearing(void *obj, s32 z, s32 x, s32 *angle)
{
    int k = mock_bearing_idx & 7;
    mock_bearing_idx++;
    fck_record("check_bearing", (s32)obj, z, x);
    if (angle) *angle = mock_bearing_ang[k];
    return mock_bearing_ret[k];
}

void mock_emit_tri(s32 origin, s32 angle, s32 colour) __asm__("func_8005A828");
void mock_emit_tri(s32 origin, s32 angle, s32 colour)
{
    fck_record("emit_tri", origin, angle, colour);
}

static void *mock_dome_mesh_ret;
void *mock_build_dome_mesh(s32 a0, s32 a1) __asm__("func_8006C84C");
void *mock_build_dome_mesh(s32 a0, s32 a1)
{
    fck_record("dome_mesh", a0, a1, 0);
    return mock_dome_mesh_ret;
}

void mock_reset_motion(void *state) __asm__("func_8005CEA8");
void mock_reset_motion(void *state)
{
    fck_record("reset_motion", (s32)state, 0, 0);
}

u32 mock_hash_cheat(u32 state) __asm__("func_80060DD8");
u32 mock_hash_cheat(u32 state)
{
    fck_record("hash_cheat", (s32)state, 0, 0);
    return state;
}

void mock_init_gauge(void *gauge, s32 value, s32 divisor) __asm__("func_8005A0C4");
void mock_init_gauge(void *gauge, s32 value, s32 divisor)
{
    fck_record("init_gauge", (s32)gauge, value, divisor);
}

void mock_refill_reserve(void *actor, s32 mode) __asm__("func_8006BDFC");
void mock_refill_reserve(void *actor, s32 mode)
{
    fck_record("refill_reserve", (s32)actor, mode, 0);
}

static s32 mock_ammo_a3;
void mock_refill_ammo(void *actor, s32 mode, s32 a2, s32 a3) __asm__("func_8006BE90");
void mock_refill_ammo(void *actor, s32 mode, s32 a2, s32 a3)
{
    mock_ammo_a3 = a3;
    fck_record("refill_ammo", (s32)actor, mode, a2);
}

void mock_alloc_light_grid(s32 w, s32 h) __asm__("func_800496D0");
void mock_alloc_light_grid(s32 w, s32 h)
{
    fck_record("alloc_light_grid", w, h, 0);
}

void mock_alloc_cell_bucket(s32 w, s32 h) __asm__("func_800834C4");
void mock_alloc_cell_bucket(s32 w, s32 h)
{
    fck_record("alloc_cell_bucket", w, h, 0);
}

void mock_init_spawn_motion(void *dest, void *src) __asm__("func_8003C14C");
void mock_init_spawn_motion(void *dest, void *src)
{
    fck_record("init_spawn_motion", (s32)dest, (s32)src, 0);
}

static u8 mock_init_pool_ret;
u8 mock_init_pool(void *pool, s32 elem_size, s32 count) __asm__("func_8003BE2C");
u8 mock_init_pool(void *pool, s32 elem_size, s32 count)
{
    fck_record("init_pool", (s32)pool, elem_size, count);
    return mock_init_pool_ret;
}

/* =========================================================================
 * Declarations of the 12 units under test
 * ========================================================================= */
void gComputeCameraEye(struct Vec3s32 *pos, s32 heading, s32 angle, s32 pitch,
                       s32 dist, u8 *data, s32 *out, s32 arg7, s32 arg8) __asm__("func_8005612C");
void gApplyPlayerHealthDamage(void *object, s32 amount) __asm__("func_80058210");
void gDrawRadarMarkers(void *object) __asm__("func_8005A9C4");
void *gAllocDomeEffectBlock(void *ent, s32 idx) __asm__("func_8005D3FC");
s32  gApplyPlayerCheatCode(u32 state, s32 index) __asm__("func_80060A0C");
void gRecordSpawnedUnitRetirement(void *arg0) __asm__("func_80061020");
void gBuildSingleViewObjectGrid(void) __asm__("func_80062C3C");
void gParseMapSectionChunks(u8 *data, u32 size, s32 id) __asm__("func_800639D4");
void gInitMapFromGrdb(void *arg0) __asm__("func_80063DC8");
void gBuildTilePattern(u32 input) __asm__("func_800654F8");
void *gCreateKind59Actor(void *src) __asm__("func_8007043C");
void *gCreateEmitterObject(void *arg) __asm__("func_800762B0");

/* =========================================================================
 * Group 1: Camera & Player Combat / Radar / Dome Effect Helpers
 * ========================================================================= */

static void test_camera_eye(void)
{
    struct Vec3s32 pos, off;
    s32 out[3];
    struct FckCall c;

    /* @leaf 8005612C source src/game/gpu-2d-hud/gComputeCameraEye.c: offset shrink/bias, focus-zeroing, four-quadrant pitch cotangent, heading rotation, and ray step */
    LK_FUNC("func_8005612C");
    gFocusPositions = 0;
    pos.x = 1000; pos.y = 200; pos.z = 3000;
    off.x = 512;  off.y = 256; off.z = 1024;
    fck_log_reset();
    gComputeCameraEye(&pos, 0x300, 0x300, 0x200, 500, (u8 *)&off, out, 64, 256);
    FCK_EQ(fck_calls_to("sincos"), 3, "calls gSinCos for heading, pitch, angle");
    c = fck_call_n("sincos", 0); FCK_EQ(c.a0, 0x300, "first sincos is heading");
    c = fck_call_n("sincos", 1); FCK_EQ(c.a0, 0x200, "second sincos is pitch");
    c = fck_call_n("sincos", 2); FCK_EQ(c.a0, 0x300, "third sincos is angle");
    FCK_EQ(out[0], 1475, "out[0] = rz(672) + base.x(1000) - scale(394)*sinA(0.5)");
    FCK_EQ(out[1], 600,  "out[1] = distance(500) + pos.y/2(100)");
    FCK_EQ(out[2], 3027, "out[2] = rx(224) + base.z(3000) - scale(394)*cosA(0.5)");

    /* Non-zero gFocusPositions zeroes offset; negative (distance - vec.y) flips scale positive */
    gFocusPositions = 1;
    pos.x = 100; pos.y = 40; pos.z = 200;
    off.x = 512; off.y = 256; off.z = 1024;
    gComputeCameraEye(&pos, 0, 0x400, 0x204, -100, (u8 *)&off, out, 0, 0);
    FCK_EQ(out[0], -20, "focus zeroed offset, negative delta abs-flipped to 120 along sinA=1");
    FCK_EQ(out[1], -80, "distance(-100) + pos.y/2(20)");
    FCK_EQ(out[2], 200, "cosA=0 leaves base.z untouched");

    /* Remaining three pitch sign quadrants: (+,-), (-,+), (-,-) all yield |cot|=2 -> scale=200 */
    gFocusPositions = 0;
    pos.x = 0; pos.y = 0; pos.z = 0;
    off.x = 0; off.y = 0; off.z = 0;
    gComputeCameraEye(&pos, 0, 0, 0x201, 100, (u8 *)&off, out, 0, 0);
    FCK_EQ(out[2], -200, "cosP>0 sinP<0 mixed_sign produces scale=200");
    gComputeCameraEye(&pos, 0, 0, 0x202, 100, (u8 *)&off, out, 0, 0);
    FCK_EQ(out[2], -200, "cosP<0 sinP>0 mixed_sign produces scale=200");
    gComputeCameraEye(&pos, 0, 0, 0x203, 100, (u8 *)&off, out, 0, 0);
    FCK_EQ(out[2], -200, "cosP<0 sinP<0 both_negative produces scale=200");
}

struct HealthSub {
    u8  unk00[0x1C];
    u16 unk1C;
    u8  unk1E[0x84];
    s16 unkA2;
    u8  unkA4[0x1D];
    s8  unkC1;
    u8  unkC2[0x264];
    s16 unk326;
};

struct HealthObj {
    u8  unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8  unk20;
    u8  unk21[3];
    u32 unk24;
    u8  unk28[0x84];
    struct HealthSub *unkAC;
    u8  unkB0[0x2C];
    struct HealthSub *unkDC;
};

static void test_player_damage(void)
{
    struct HealthObj obj;
    struct HealthSub sub_ac, sub_dc;
    struct SceneGameSession sess;
    struct FckCall c;

    /* @leaf 80058210 source src/game/gameplay-ai/gApplyPlayerHealthDamage.c: damage guards, health mirror, unkA2/shake clamps, session halving, and quartile callback */
    LK_FUNC("func_80058210");
    lk_fill(&obj, 0, sizeof(obj));
    lk_fill(&sub_ac, 0, sizeof(sub_ac));
    lk_fill(&sub_dc, 0, sizeof(sub_dc));
    lk_fill(&sess, 0, sizeof(sess));
    lk_fill(gPlayerInvulnerable, 0, sizeof(gPlayerInvulnerable));
    gGameSession = &sess;
    obj.unkAC = &sub_ac;
    obj.unkDC = &sub_dc;
    obj.unk1C = 100;
    obj.unk1E = 200;
    obj.unk20 = 1;
    obj.unk24 = 0;
    sub_ac.unkC1 = 1;
    sub_ac.unkA2 = 50;
    sub_dc.unk1C = 100;

    /* Guard 1: amount <= 0 */
    fck_log_reset();
    gApplyPlayerHealthDamage(&obj, 0);
    gApplyPlayerHealthDamage(&obj, -10);
    FCK_EQ(obj.unk1C, 100, "non-positive damage leaves health untouched");
    FCK_EQ(fck_log_count, 0, "non-positive damage emits no calls");

    /* Guard 2: invulnerable table entry */
    gPlayerInvulnerable[1] = 1;
    gApplyPlayerHealthDamage(&obj, 10);
    FCK_EQ(obj.unk1C, 100, "invulnerable flag blocks damage");
    FCK_EQ(fck_log_count, 0, "invulnerable flag emits no calls");
    gPlayerInvulnerable[1] = 0;

    /* Guard 3: unk326 timer > 0 */
    sub_ac.unk326 = 1;
    gApplyPlayerHealthDamage(&obj, 10);
    FCK_EQ(obj.unk1C, 100, "active timer blocks damage");
    FCK_EQ(fck_log_count, 0, "active timer emits no calls");
    sub_ac.unk326 = 0;

    /* Normal damage: value = 50 + ((10*5)<<10)/200 = 306 */
    fck_log_reset();
    gApplyPlayerHealthDamage(&obj, 10);
    FCK_EQ(obj.unk1C, 90, "health decremented by amount");
    FCK_EQ(sub_dc.unk1C, 90, "unkDC mirror updated");
    FCK_EQ(sub_ac.unkA2, 306, "unkA2 updated in range");
    FCK_EQ(fck_calls_to("add_shake"), 1, "calls gAddCameraShake");
    c = fck_call_n("add_shake", 0);
    FCK_CHECK(c.a0 == (s32)&obj && c.a1 == 306, "shake gets object and value=306");
    FCK_EQ(fck_calls_to("flag_quartile"), 1, "calls gFlagQuartileChange");
    c = fck_call_n("flag_quartile", 0);
    FCK_CHECK(c.a0 == (s32)&obj && c.a1 == 100, "quartile gets oldHealth=100");

    /* Exact boundary value == 0x1C3 (451): unkA2 clamps to 0x1C2 while shake stays 451 */
    obj.unk1C = 100; obj.unk1E = 640; sub_ac.unkA2 = 371; sess.unk5C = 0;
    fck_log_reset();
    gApplyPlayerHealthDamage(&obj, 10);
    FCK_EQ(sub_ac.unkA2, 0x1C2, "value==0x1C3 clamps unkA2 to 0x1C2");
    c = fck_call_n("add_shake", 0); FCK_EQ(c.a1, 451, "shake preserves 451 <= 0x200");

    /* Clamp shake to 0x200 and halve when sess.unk5C != 0; flags 0x40000 skips quartile */
    obj.unk1C = 100; obj.unk1E = 640; sub_ac.unkA2 = 500; sess.unk5C = 1; obj.unk24 = 0x40000;
    fck_log_reset();
    gApplyPlayerHealthDamage(&obj, 10);
    FCK_EQ(sub_ac.unkA2, 0x1C2, "unkA2 clamped to 0x1C2 above 0x200");
    c = fck_call_n("add_shake", 0); FCK_EQ(c.a1, 0x100, "shake clamped to 0x200 then halved to 0x100");
    FCK_EQ(fck_calls_to("flag_quartile"), 0, "flag 0x40000 suppresses quartile callback");

    /* Negative value clamps both unkA2 and shake to 0; unk20 == 0 skips quartile */
    obj.unk1C = 50; obj.unk1E = 100; sub_ac.unkA2 = -500; sess.unk5C = 0; obj.unk20 = 0; obj.unk24 = 0;
    fck_log_reset();
    gApplyPlayerHealthDamage(&obj, 1);
    FCK_EQ(sub_ac.unkA2, 0, "negative value clamps unkA2 to 0");
    c = fck_call_n("add_shake", 0); FCK_EQ(c.a1, 0, "negative value clamps shake to 0");
    FCK_EQ(fck_calls_to("flag_quartile"), 0, "unk20==0 suppresses quartile callback");
}

struct RadarNode {
    u8  unk00[0xA4];
    u32 unkA4;
    u8  flags[8];
    struct { s16 a; s16 b; } pos[8];
};
struct RadarAC {
    u8  unk00[0xC1];
    s8  unkC1;
    u8  unkC2[0x300 - 0xC2];
    struct RadarNode *unk300;
};
struct RadarObj {
    u8  unk00[0xAC];
    struct RadarAC *unkAC;
    s16 unkB0;
    s16 unkB2;
};
struct RadarOther {
    u8  unk00[0x50];
    s32 unk50;
    u8  unk54[4];
    s32 unk58;
};

static void test_radar_markers(void)
{
    struct RadarObj obj;
    struct RadarAC ac;
    struct RadarNode node;
    struct RadarOther other;
    struct SceneGameSession sess;
    struct FckCall c;
    int i;

    /* @leaf 8005A9C4 source src/game/gpu-2d-hud/gDrawRadarMarkers.c: side origin selection, heading line, 2P bearing line, and 3-bit-stride radar triangles */
    LK_FUNC("func_8005A9C4");
    lk_fill(&obj, 0, sizeof(obj));
    lk_fill(&ac, 0, sizeof(ac));
    lk_fill(&node, 0, sizeof(node));
    lk_fill(&other, 0, sizeof(other));
    lk_fill(&sess, 0, sizeof(sess));
    for (i = 0; i < 32; i++) gMarkerColorTable[i] = 0x1000 + i;
    gGameSession = &sess;
    obj.unkAC = &ac;

    /* Side 0, 1P, blink off -> heading line only */
    ac.unkC1 = 0;
    obj.unkB0 = 0x100;
    gPlayerCount = 1;
    gFrameTime = 0;
    fck_log_reset();
    gDrawRadarMarkers(&obj);
    FCK_EQ(fck_calls_to("emit_line"), 1, "1P blink-off emits heading line only");
    c = fck_call_n("emit_line", 0);
    FCK_CHECK(c.a0 == 0x280000 && c.a1 == -0x900 && c.a2 == 0xF8F8F8, "side 0 origin 0x280000 and heading -unkB0-0x800");
    FCK_EQ(fck_calls_to("check_bearing"), 0, "1P skips other-player bearing");
    FCK_EQ(fck_calls_to("emit_tri"), 0, "blink off skips triangles");

    /* Side 1, 2P with unk5C==0, blink on with slots 0, 2, 7 active */
    ac.unkC1 = 1;
    obj.unkB0 = -0x200;
    ac.unk300 = &node;
    other.unk50 = 0x111;
    other.unk58 = 0x222;
    gPlayerEntities[0] = &other;
    gPlayerEntities[1] = 0;
    gPlayerCount = 2;
    sess.unk5C = 0;
    gFrameTime = 0x80;
    node.unkA4 = (1u << 0) | (1u << 6) | (1u << 21) | (1u << 24);
    node.pos[0].a = 3;  node.pos[0].b = 5;  node.flags[0] = 0x23;
    node.pos[2].a = 7;  node.pos[2].b = 9;  node.flags[2] = 0x05;
    node.pos[7].a = -2; node.pos[7].b = 4;  node.flags[7] = 0x1F;
    mock_bearing_idx = 0;
    mock_bearing_ret[0] = 1; mock_bearing_ang[0] = 0x333; /* other player */
    mock_bearing_ret[1] = 1; mock_bearing_ang[1] = 0x444; /* slot 0 */
    mock_bearing_ret[2] = 0; mock_bearing_ang[2] = 0x555; /* slot 2 fails */
    mock_bearing_ret[3] = 1; mock_bearing_ang[3] = 0x666; /* slot 7 */
    fck_log_reset();
    gDrawRadarMarkers(&obj);
    FCK_EQ(fck_calls_to("emit_line"), 2, "emits heading line plus other-player bearing line");
    c = fck_call_n("emit_line", 0);
    FCK_CHECK(c.a0 == 0x780000 && c.a1 == -0x600, "side 1 origin 0x780000 and heading -(-0x200)-0x800");
    c = fck_call_n("emit_line", 1);
    FCK_CHECK(c.a0 == 0x780000 && c.a1 == 0x333 && c.a2 == 0xF800, "other-player line uses bearing angle and 0xF800");
    FCK_EQ(fck_calls_to("check_bearing"), 4, "checks other player plus 3 active marker slots");
    c = fck_call_n("check_bearing", 0);
    FCK_CHECK(c.a1 == 0x222 && c.a2 == 0x111, "other player bearing reads unk58 then unk50");
    c = fck_call_n("check_bearing", 1);
    FCK_CHECK(c.a1 == (3 << 9) && c.a2 == (5 << 9), "slot 0 shifts pos a/b by 9");
    c = fck_call_n("check_bearing", 3);
    FCK_CHECK(c.a1 == (-2 << 9) && c.a2 == (4 << 9), "slot 7 shifts signed pos a/b by 9");
    FCK_EQ(fck_calls_to("emit_tri"), 2, "only slots with bearing hit emit triangles");
    c = fck_call_n("emit_tri", 0);
    FCK_CHECK(c.a0 == 0x780000 && c.a1 == 0x444 && c.a2 == 0x1003, "slot 0 colour masked by 0x1F");
    c = fck_call_n("emit_tri", 1);
    FCK_CHECK(c.a0 == 0x780000 && c.a1 == 0x666 && c.a2 == 0x101F, "slot 7 colour uses index 31");

    /* sess.unk5C != 0 skips other-player; non-0x249249 bits skip loop */
    sess.unk5C = 1;
    node.unkA4 = (1u << 1) | (1u << 2);
    fck_log_reset();
    gDrawRadarMarkers(&obj);
    FCK_EQ(fck_calls_to("emit_line"), 1, "sess.unk5C!=0 skips other-player line");
    FCK_EQ(fck_calls_to("check_bearing"), 0, "mask without 0x249249 skips marker scan");

    /* Null marker node with blink on */
    ac.unk300 = 0;
    gFrameTime = 0x100;
    fck_log_reset();
    gDrawRadarMarkers(&obj);
    FCK_EQ(fck_calls_to("emit_tri"), 0, "null marker node emits no triangles");
}

struct DomeBlock {
    void *unk00;
    void *unk04;
    s32 unk08;
    s32 unk0C;
    u8  unk10[4];
    s32 unk14;
    u8  unk18[0x30];
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    u8  unk4E[2];
    s16 unk50;
    s16 unk52;
    s32 unk54;
    u8  subs[12 * 0x1C];
    u8  unk1A8, unk1A9, unk1AA, unk1AB, unk1AC, unk1AD, unk1AE, unk1AF;
};

struct DomeEnt {
    u8  unk00[8];
    s32 unk08;
    u8  unk0C[0xA];
    u8  unk16;
    u8  unk17[0x57];
    u16 unk6E;
    u8  unk70[0x3C];
    u8 *unkAC;
};

static void test_dome_alloc(void)
{
    struct DomeBlock blk;
    struct DomeEnt ent;
    u8 ent_ac[0xA0];
    u8 dummy_mesh[8];
    u8 occ0[4], occ1[4], occ2[4], occ3[4];
    void *ret;
    struct FckCall c;

    /* @leaf 8005D3FC source src/game/renderer-scene/gAllocDomeEffectBlock.c: 4-slot scan, 0x1B0 alloc, 3-way mesh/colour selection, and 12 motion sub-state resets */
    LK_FUNC("func_8005D3FC");
    lk_fill(&ent, 0, sizeof(ent));
    lk_fill(ent_ac, 0, sizeof(ent_ac));
    ent.unk08 = 0x1234;
    ent.unk6E = 0x35;
    ent.unkAC = ent_ac;
    mock_alloc_ret = &blk;
    mock_dome_mesh_ret = dummy_mesh;

    /* Full table returns NULL without allocating */
    gDomeEffectBlock[0] = occ0; gDomeEffectBlock[1] = occ1;
    gDomeEffectBlock[2] = occ2; gDomeEffectBlock[3] = occ3;
    fck_log_reset();
    ret = gAllocDomeEffectBlock(&ent, 0);
    FCK_CHECK(ret == 0, "full 4-slot table returns NULL");
    FCK_EQ(fck_calls_to("alloc"), 0, "full table does not allocate");

    /* Slot 1 free, ent.unk16 == 1, s16 at unkAC + idx*2 + 0x8A > 0 */
    gDomeEffectBlock[1] = 0;
    lk_fill(&blk, 0xCC, sizeof(blk));
    ent.unk16 = 1;
    *(s16 *)(ent_ac + (2 << 1) + 0x8A) = 5;
    fck_log_reset();
    ret = gAllocDomeEffectBlock(&ent, 2);
    FCK_CHECK(ret == &blk && gDomeEffectBlock[1] == &blk, "installs allocated block in first free slot 1");
    c = fck_call_n("alloc", 0); FCK_CHECK(c.a0 == 0x1B0 && c.a1 == 2, "allocates 0x1B0 bytes with tag 2");
    c = fck_call_n("dome_mesh", 0); FCK_CHECK(c.a0 == 0x100 && c.a1 == 9, "unk16==1 builds 0x100/9 dome mesh");
    FCK_CHECK(blk.unk00 == dummy_mesh && blk.unk04 == &ent && blk.unk08 == 0x1234, "header links mesh and owner");
    FCK_CHECK(blk.unk4C == 0x6A && blk.unk54 == 2, "unk4C=unk6E*2 and unk54=idx");
    FCK_CHECK(blk.unk0C == 0 && blk.unk14 == 0 && blk.unk48 == 0 && blk.unk4A == 0 &&
              blk.unk50 == 0 && blk.unk52 == 0 && blk.unk1AE == 0, "zeroed state fields");
    FCK_CHECK(blk.unk1A8 == 0x40 && blk.unk1A9 == 0x40 && blk.unk1AA == 0x60 &&
              blk.unk1AB == 0x7F && blk.unk1AC == 0x60 && blk.unk1AD == 0, "unk16==1 positive palette");
    FCK_EQ(fck_calls_to("reset_motion"), 12, "resets all 12 motion sub-states");
    c = fck_call_n("reset_motion", 0); FCK_EQ(c.a0, (s32)((u8 *)&blk + 0x58), "sub 0 at +0x58");
    c = fck_call_n("reset_motion", 11); FCK_EQ(c.a0, (s32)((u8 *)&blk + 0x58 + 11 * 0x1C), "sub 11 at +0x58+11*0x1C");

    /* ent.unk16 == 1, s16 <= 0 -> alternate palette */
    gDomeEffectBlock[2] = 0;
    *(s16 *)(ent_ac + (1 << 1) + 0x8A) = 0;
    fck_log_reset();
    ret = gAllocDomeEffectBlock(&ent, 1);
    FCK_CHECK(ret == &blk && gDomeEffectBlock[2] == &blk, "fills slot 2");
    c = fck_call_n("dome_mesh", 0); FCK_CHECK(c.a0 == 0x100 && c.a1 == 9, "unk16==1 non-positive still uses 0x100/9");
    FCK_CHECK(blk.unk1A8 == 0x40 && blk.unk1A9 == 0x40 && blk.unk1AA == 0x40 &&
              blk.unk1AB == 0x60 && blk.unk1AC == 0x30 && blk.unk1AD == 0, "unk16==1 non-positive palette");

    /* ent.unk16 != 1 -> 0x200/0xD mesh and third palette */
    gDomeEffectBlock[3] = 0;
    ent.unk16 = 2;
    fck_log_reset();
    ret = gAllocDomeEffectBlock(&ent, 0);
    FCK_CHECK(ret == &blk && gDomeEffectBlock[3] == &blk, "fills slot 3");
    c = fck_call_n("dome_mesh", 0); FCK_CHECK(c.a0 == 0x200 && c.a1 == 0xD, "unk16!=1 builds 0x200/0xD mesh");
    FCK_CHECK(blk.unk1A8 == 0x60 && blk.unk1A9 == 0x30 && blk.unk1AA == 0 &&
              blk.unk1AB == 0x7F && blk.unk1AC == 0x7F && blk.unk1AD == 0x7F, "unk16!=1 palette");
}

/* =========================================================================
 * Group 2: Cheat Codes & Unit Retirement Bookkeeping
 * ========================================================================= */

struct CheatSub {
    u8  pad00[0x54];
    u8  gauge[0x14];
    u8  pad68[0x59];
    s8  unkC1;
    u8  padC2[0x1D];
    u8  unkDF;
};

struct CheatActor {
    u8  pad00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8  tableIndex;
    u8  pad21[3];
    u32 flags;
    u8  pad28[0x6A];
    u8  unk92;
    u8  pad93[0x19];
    struct CheatSub *unkAC;
};

static void test_cheat_codes(void)
{
    struct CheatActor a0, a1;
    struct CheatSub s0, s1;
    struct SceneGameSession sess;
    struct FckCall c;
    s32 ret;

    /* @leaf 80060A0C source src/game/boot-frontend/gApplyPlayerCheatCode.c: 13-case cheat code dispatch, health refill/max fallthrough, mission exit, ammo/reserve, score, and unknown-code return */
    LK_FUNC("func_80060A0C");
    lk_fill(&a0, 0, sizeof(a0)); lk_fill(&a1, 0, sizeof(a1));
    lk_fill(&s0, 0, sizeof(s0)); lk_fill(&s1, 0, sizeof(s1));
    lk_fill(&sess, 0, sizeof(sess));
    lk_fill(gPlayerInvulnerable, 0, sizeof(gPlayerInvulnerable));
    a0.unkAC = &s0; a1.unkAC = &s1;
    gPlayerEntities[0] = &a0; gPlayerEntities[1] = &a1;
    gGameSession = &sess;

    /* 0x00011081: refill health + gauge + quartile */
    a0.unk1C = 40; a0.unk1E = 250; a0.tableIndex = 1; a0.flags = 0;
    fck_log_reset();
    ret = gApplyPlayerCheatCode(0x00011081u, 0);
    FCK_EQ(ret, 0, "recognized cheat returns 0");
    FCK_EQ(a0.unk1C, 250, "0x00011081 refills unk1C to unk1E");
    c = fck_call_n("init_gauge", 0);
    FCK_CHECK(c.a0 == (s32)s0.gauge && c.a1 == 250 && c.a2 == 250, "updates HUD gauge");
    c = fck_call_n("flag_quartile", 0);
    FCK_CHECK(c.a0 == (s32)&a0 && c.a1 == 40, "flags quartile with old health 40");

    /* 0x0A9CF51C: sets unk1E=0x7FFF and falls through; flag 0x40000 suppresses quartile */
    a1.unk1C = 10; a1.unk1E = 100; a1.tableIndex = 1; a1.flags = 0x40000;
    fck_log_reset();
    ret = gApplyPlayerCheatCode(0x0A9CF51Cu, 1);
    FCK_EQ(ret, 0, "0x0A9CF51C returns 0");
    FCK_CHECK(a1.unk1E == 0x7FFF && a1.unk1C == 0x7FFF, "sets max and current health to 0x7FFF");
    FCK_EQ(fck_calls_to("flag_quartile"), 0, "flag 0x40000 skips quartile on refill");

    /* 0x01ADCFED: unk5C==1 vs unk5C!=1 */
    sess.unk5C = 1; sess.unk44 = 50; gScreenResetFlag = 0; gScreenState = 0;
    ret = gApplyPlayerCheatCode(0x01ADCFEDu, 0);
    FCK_CHECK(ret == 0 && gScreenResetFlag == 0x78 && sess.unk40 == 51, "unk5C==1 sets reset flag and unk44-(index-1)");
    sess.unk5C = 0; sess.unk44 = 77;
    ret = gApplyPlayerCheatCode(0x01ADCFEDu, 1);
    FCK_CHECK(ret == 0 && gScreenState == 0x0B && sess.unk40 == 77, "unk5C!=1 sets screen state 0x0B and unk40=unk44");

    /* 0x016499A4: 1P toggles actor0->unk92 via 0x80 - unk92; 2P no-op */
    gPlayerCount = 1; a0.unk92 = 0x20;
    ret = gApplyPlayerCheatCode(0x016499A4u, 0);
    FCK_CHECK(ret == 0 && a0.unk92 == 0x60, "1P toggles unk92 = 0x80 - 0x20");
    gPlayerCount = 2; a0.unk92 = 0x20;
    ret = gApplyPlayerCheatCode(0x016499A4u, 0);
    FCK_CHECK(ret == 0 && a0.unk92 == 0x20, "2P leaves unk92 untouched");

    /* 0x031F78DF: invulnerability flag */
    ret = gApplyPlayerCheatCode(0x031F78DFu, 1);
    FCK_CHECK(ret == 0 && gPlayerInvulnerable[1] == 1 && gPlayerInvulnerable[0] == 0, "sets invulnerable[index]");

    /* Ammo refills: slots 0, 1, 2 */
    fck_log_reset();
    FCK_EQ(gApplyPlayerCheatCode(0x01BD6EFDu, 0), 0, "ammo 0 returns 0");
    FCK_EQ(gApplyPlayerCheatCode(0x03449B84u, 1), 0, "ammo 1 returns 0");
    FCK_EQ(gApplyPlayerCheatCode(0x0058E758u, 0), 0, "ammo 2 returns 0");
    FCK_EQ(fck_calls_to("refill_ammo"), 3, "3 ammo refills recorded");
    c = fck_call_n("refill_ammo", 0); FCK_CHECK(c.a0 == (s32)&a0 && c.a1 == 0 && c.a2 == -1, "ammo slot 0");
    c = fck_call_n("refill_ammo", 1); FCK_CHECK(c.a0 == (s32)&a1 && c.a1 == 1 && c.a2 == -1, "ammo slot 1");
    c = fck_call_n("refill_ammo", 2); FCK_CHECK(c.a0 == (s32)&a0 && c.a1 == 2 && c.a2 == -1, "ammo slot 2");
    FCK_EQ(mock_ammo_a3, 0, "ammo a3 is 0");

    /* Reserve refills: slots 0, 1, 2 */
    fck_log_reset();
    FCK_EQ(gApplyPlayerCheatCode(0x032D496Du, 1), 0, "reserve 0 returns 0");
    FCK_EQ(gApplyPlayerCheatCode(0x004F464Fu, 0), 0, "reserve 1 returns 0");
    FCK_EQ(gApplyPlayerCheatCode(0x0063A563u, 1), 0, "reserve 2 returns 0");
    FCK_EQ(fck_calls_to("refill_reserve"), 3, "3 reserve refills recorded");
    c = fck_call_n("refill_reserve", 0); FCK_CHECK(c.a0 == (s32)&a1 && c.a1 == 0, "reserve slot 0");
    c = fck_call_n("refill_reserve", 1); FCK_CHECK(c.a0 == (s32)&a0 && c.a1 == 1, "reserve slot 1");
    c = fck_call_n("refill_reserve", 2); FCK_CHECK(c.a0 == (s32)&a1 && c.a1 == 2, "reserve slot 2");

    /* 0xFEDAE5DA: OR bit 0 into unkDF */
    s1.unkDF = 0x20;
    ret = gApplyPlayerCheatCode(0xFEDAE5DAu, 1);
    FCK_CHECK(ret == 0 && s1.unkDF == 0x21, "sets bit 0 of unkDF preserving existing bits");

    /* 0x00F8F778: +0xC8 score indexed by unkC1 */
    s0.unkC1 = 3; sess.unk64[3] = 100;
    ret = gApplyPlayerCheatCode(0x00F8F778u, 0);
    FCK_CHECK(ret == 0 && sess.unk64[3] == 300, "adds 0xC8 to session unk64[unkC1]");

    /* Default: unrecognized code returns 1 */
    fck_log_reset();
    ret = gApplyPlayerCheatCode(0x12345678u, 0);
    FCK_EQ(ret, 1, "unrecognized code returns 1");
    FCK_EQ(fck_calls_to("hash_cheat"), 1, "hashes code before default");
}

struct RetDef  { s32 id; u8 pad04[0x18]; u8 team; u8 type; };
struct RetInfo { u8 pad00[0x16]; u16 unk16; u8 pad18[0x10]; struct RetDef *def; };
struct RetEnt  { s32 unk00; struct RetInfo *info; };
struct RetTbl  { s32 count; struct RetEnt *entries[6]; };
struct Ret134  { u8 pad00[0x18]; u16 unk18; u8 pad1A[0xE]; u16 unk28, p2A, unk2C, p2E, unk30, p32, unk34; };
struct RetUnit { u8 pad00[0x14]; u16 unk14; u8 pad16[2]; u8 team; u8 pad19[0x11B]; struct Ret134 *sub134; };
struct RetAC   { u16 unk00, p02, unk04, p06, unk08, p0A, unk0C; };
struct RetAct  { u8 pad00[0x18]; u8 team; u8 pad19[0x93]; struct RetAC *unkAC; };

static void test_unit_retirement(void)
{
    struct RetTbl tbl;
    struct RetEnt ents[6];
    struct RetInfo infos[6];
    struct RetDef defs[6];
    struct RetUnit arg, sp_unit;
    struct Ret134 sub134;
    struct RetAct p0, p1;
    struct RetAC ac0, ac1;
    struct SceneGameSession sess;
    int i;

    /* @leaf 80061020 source src/game/gameplay-ai/gRecordSpawnedUnitRetirement.c: registry count decrement and 1P/2P team-routed type 1..4 statistics */
    LK_FUNC("func_80061020");
    lk_fill(&tbl, 0, sizeof(tbl)); lk_fill(ents, 0, sizeof(ents));
    lk_fill(infos, 0, sizeof(infos)); lk_fill(defs, 0, sizeof(defs));
    lk_fill(&arg, 0, sizeof(arg)); lk_fill(&sp_unit, 0, sizeof(sp_unit));
    lk_fill(&sub134, 0, sizeof(sub134)); lk_fill(&p0, 0, sizeof(p0));
    lk_fill(&p1, 0, sizeof(p1)); lk_fill(&ac0, 0, sizeof(ac0));
    lk_fill(&ac1, 0, sizeof(ac1)); lk_fill(&sess, 0, sizeof(sess));
    for (i = 0; i < 6; i++) {
        tbl.entries[i] = &ents[i];
        ents[i].info = &infos[i];
        infos[i].def = &defs[i];
    }
    sp_unit.sub134 = &sub134;
    p0.unkAC = &ac0; p1.unkAC = &ac1;
    gRegistryTable = &tbl;
    gGameSession = &sess;
    gPlayerEntities[0] = &p0;
    gPlayerEntities[1] = &p1;

    /* Empty table (count == 0) does not enter loop */
    arg.unk14 = 42;
    tbl.count = 0;
    defs[0].id = 42; infos[0].unk16 = 5;
    gRecordSpawnedUnitRetirement(&arg);
    FCK_EQ(infos[0].unk16, 5, "count==0 leaves entries untouched");

    /* Mode != 1: decrements matching info->unk16 only */
    arg.unk14 = 42;
    tbl.count = 2;
    defs[0].id = 42; infos[0].unk16 = 5; defs[0].type = 1; defs[0].team = 1;
    defs[1].id = 99; infos[1].unk16 = 7;
    sess.unk5C = 0;
    gSinglePlayerEntity = &sp_unit;
    gRecordSpawnedUnitRetirement(&arg);
    FCK_EQ(infos[0].unk16, 4, "matching entry count decremented");
    FCK_EQ(infos[1].unk16, 7, "non-matching entry count untouched");
    FCK_EQ(ac0.unk00, 0, "unk5C!=1 skips statistics");

    /* 1P mode, same team (sub==0) -> increments sub134->unk28/2C/30/34, preserves unk18 */
    sess.unk5C = 1;
    sp_unit.team = 2;
    sub134.unk18 = 99;
    tbl.count = 5;
    for (i = 0; i < 4; i++) {
        defs[i].id = 42; defs[i].team = 2; defs[i].type = (u8)(i + 1); infos[i].unk16 = 3;
    }
    defs[4].id = 42; defs[4].team = 2; defs[4].type = 5; infos[4].unk16 = 3;
    gRecordSpawnedUnitRetirement(&arg);
    FCK_CHECK(sub134.unk28 == 1 && sub134.unk2C == 1 && sub134.unk30 == 1 && sub134.unk34 == 1,
              "1P same-team increments sub134 counters for types 1..4");
    FCK_EQ(sub134.unk18, 99, "same-team path does not clear sub134->unk18");

    /* 1P mode, enemy team (sub==p0->unkAC) -> increments ac0 counters and clears sub134->unk18 */
    for (i = 0; i < 4; i++) { defs[i].team = 3; infos[i].unk16 = 2; }
    tbl.count = 4;
    gRecordSpawnedUnitRetirement(&arg);
    FCK_CHECK(ac0.unk00 == 1 && ac0.unk04 == 1 && ac0.unk08 == 1 && ac0.unk0C == 1,
              "1P enemy-team increments p0->unkAC counters for types 1..4");
    FCK_EQ(sub134.unk18, 0, "enemy-team path clears sub134->unk18");

    /* 2P mode (gSinglePlayerEntity == 0): routes by team match with p0->team */
    gSinglePlayerEntity = 0;
    p0.team = 10;
    lk_fill(&ac0, 0, sizeof(ac0)); lk_fill(&ac1, 0, sizeof(ac1));
    tbl.count = 2;
    defs[0].id = 42; defs[0].team = 10; defs[0].type = 2; infos[0].unk16 = 1;
    defs[1].id = 42; defs[1].team = 20; defs[1].type = 4; infos[1].unk16 = 1;
    gRecordSpawnedUnitRetirement(&arg);
    FCK_CHECK(ac0.unk04 == 1 && ac1.unk0C == 1, "2P routes matching team to p0 and other team to p1");
}

/* =========================================================================
 * Group 3: Object Grid, Map Chunks, GRDB Init, and Tile Pattern
 * ========================================================================= */

static void test_single_view_grid(void)
{
    struct GridObj objs[6];
    struct GridObj *out_list[8];
    u8 cell_map[64];
    int i;

    /* @leaf 80062C3C source src/game/gpu-2d-hud/gBuildSingleViewObjectGrid.c: radius clamping, 5x5x16 coverage bit test, 256 cap, flags 4/0x100 gating, and cell linked list */
    LK_FUNC("func_80062C3C");
    lk_fill(objs, 0, sizeof(objs));
    lk_fill(out_list, 0, sizeof(out_list));
    lk_fill(cell_map, 0, sizeof(cell_map));
    lk_fill(gCoverageGrid, 0, sizeof(gCoverageGrid));
    lk_fill(gCellNodeArray, 0, sizeof(gCellNodeArray));
    gViewGroundAnchors[0].x = 0x40000;
    gViewGroundAnchors[0].z = 0x40000;
    gGridOriginX = 1;
    gGridOriginY = 1;
    gCellClipRect[0] = 4; /* x */
    gCellClipRect[1] = 4; /* z */
    gCellClipRect[2] = 8; /* w */
    gCellClipRect[3] = 8; /* h */
    gCellEntityList = out_list;
    gCellMap = cell_map;
    gCellEntityCount = 0;
    /* Anchor (0x40000,0x40000) -> cell col=(2-1)=1, row_x=(2-1)=1 -> idx 6, bit 15-0 = 15 */
    gCoverageGrid[6].bits = (1u << 15);

    /* 0: inactive */
    objs[0].active = 0; objs[0].x = 0x40000; objs[0].z = 0x40000;
    /* 1: active, covered, flags=0 -> in list and cell (4,4) */
    objs[1].active = 1; objs[1].x = 0x40000; objs[1].z = 0x40000; objs[1].radius = 0x1000; objs[1].flags = 0;
    /* 2: active, far +x/+z but radius pulls into anchor, flags=4 -> in list, NOT in grid */
    objs[2].active = 1; objs[2].x = 0x50000; objs[2].z = 0x50000; objs[2].radius = 0x10000; objs[2].flags = 4;
    /* 3: active, far -x/-z (miss), flags=0x100 -> NOT in list, IN grid at (4,4) chaining over objs[1] */
    objs[3].active = 1; objs[3].x = 0x40000; objs[3].z = 0x40000; objs[3].radius = 0; objs[3].flags = 0x100;
    /* Wait: to miss coverage while landing in grid cell (4,4), use x=0x48000 (subcell 1 -> bit 14 = 0) */
    objs[3].x = 0x48000; objs[3].z = 0x40000;
    /* 4: active, miss, flags=0 -> neither list nor grid */
    objs[4].active = 1; objs[4].x = 0x48000; objs[4].z = 0x48000; objs[4].radius = 0; objs[4].flags = 0;
    /* 5: active, covered via large radius, outside clip rect -> in list, NOT in grid */
    objs[5].active = 1; objs[5].x = 0xA0000; objs[5].z = 0x40000; objs[5].radius = 0x60000; objs[5].flags = 0;

    gCommandListHead.count = 6;
    for (i = 0; i < 6; i++) gCommandListHead.items[i] = &objs[i];
    gBuildSingleViewObjectGrid();

    FCK_EQ(gCellEntityCount, 3, "3 covered active entities appended");
    FCK_CHECK(out_list[0] == &objs[1] && out_list[1] == &objs[2] && out_list[2] == &objs[5],
              "output list preserves scan order of hits");
    FCK_EQ(gViewCount, 3, "2 grid nodes allocated (n starts at 1, ends at 3)");
    /* objs[1] at col=(0x40000>>15)-4=4, row=4 -> map[4*8+4 = 36] = 1 */
    FCK_EQ(cell_map[36], 1, "objs[1] head at cell (4,4) is node 1");
    FCK_CHECK(gCellNodeArray[1].obj == &objs[1] && gCellNodeArray[1].next == 0, "node 1 terminates");
    /* objs[3] at col=(0x48000>>15)-4=5, row=4 -> map[37] = 2 */
    FCK_EQ(cell_map[37], 2, "flags 0x100 miss inserted at cell (5,4) as node 2");
    FCK_CHECK(gCellNodeArray[2].obj == &objs[3] && gCellNodeArray[2].next == 0, "node 2 terminates");

    /* Same-cell chaining + 256 cap check */
    lk_fill(cell_map, 0, sizeof(cell_map));
    lk_fill(gCellNodeArray, 0, sizeof(gCellNodeArray));
    gCellEntityCount = 0;
    objs[3].x = 0x40000; /* now same cell (4,4) as objs[1] */
    gCommandListHead.count = 2;
    gCommandListHead.items[0] = &objs[1];
    gCommandListHead.items[1] = &objs[3];
    gBuildSingleViewObjectGrid();
    FCK_EQ(cell_map[36], 2, "second entity in same cell becomes head node 2");
    FCK_CHECK(gCellNodeArray[2].obj == &objs[3] && gCellNodeArray[2].next == &gCellNodeArray[1],
              "node 2 links to previous head node 1");

    /* Out-of-range coverage col == 5 is rejected even if cell 25 has bits set */
    gCoverageGrid[25].bits = 0xFFFFu;
    gCellEntityCount = 0;
    objs[4].active = 1; objs[4].x = 0x20000; objs[4].z = 0xC0000; objs[4].radius = 0; objs[4].flags = 0;
    gViewGroundAnchors[0].x = 0x20000; gViewGroundAnchors[0].z = 0xC0000;
    gCommandListHead.count = 1;
    gCommandListHead.items[0] = &objs[4];
    gBuildSingleViewObjectGrid();
    FCK_EQ(gCellEntityCount, 0, "col==5 is outside 0..4 coverage grid");

    /* Cap at 256 skips append and grid */
    gViewGroundAnchors[0].x = 0x40000; gViewGroundAnchors[0].z = 0x40000;
    gCellEntityCount = 256;
    lk_fill(cell_map, 0, sizeof(cell_map));
    gCommandListHead.count = 1;
    gCommandListHead.items[0] = &objs[1];
    gBuildSingleViewObjectGrid();
    FCK_EQ(gCellEntityCount, 256, "count capped at 256");
    FCK_EQ(gViewCount, 1, "256-capped hit skips grid insertion");
}

struct MapChunkHdr { u32 type; u32 size; s32 count; };
struct MapAnimEnt  { u8 a; u8 b; u8 kind; u8 c; s16 value; u16 d; u8 *p; u8 *q; };
struct MapSection  { u8 *a; u8 *b; u8 *c; u8 pad[0x363]; s8 index; u8 pad2[0x5a]; s16 n; };

static void test_parse_map_chunks(void)
{
    static u8 buf[0x800];
    void *tile_blocks[8];
    struct MapChunkHdr *scla, *anim, *slfx1, *slfx2, *slfx3, *unk;
    struct MapSection *sect;
    struct MapAnimEnt *e0, *e1;
    u8 *p;
    u8 sentinel_anim[12];

    /* @leaf 800639D4 source src/game/memory-resources/gParseMapSectionChunks.c: ScLA skip, Sect pointer relocation, ScTA list link & entry relocation/division, SLFX dedup & flag */
    LK_FUNC("func_800639D4");
    lk_fill(buf, 0, sizeof(buf));
    lk_fill(tile_blocks, 0, sizeof(tile_blocks));
    gTileBlockTable = tile_blocks;
    lk_fill(&gTileDefTable, 0, sizeof(gTileDefTable));
    lk_fill(gSlfxFlagByte, 0, sizeof(gSlfxFlagByte));
    gMapTileAnimList = sentinel_anim;

    p = buf;
    /* 1. ScLA (ignored) */
    scla = (struct MapChunkHdr *)p;
    scla->type = 0x53634C41u; scla->size = 16;
    p += 16;

    /* 2. Sect (size 0x620) */
    sect = (struct MapSection *)p;
    ((struct MapChunkHdr *)p)->type = 0x53656374u;
    ((struct MapChunkHdr *)p)->size = 0x620;
    sect->c = (u8 *)((3u << 16) | 5u);
    sect->n = 2;
    p += 0x620;

    /* 3. ScTA (header 12 + 2 * 16 = 44, size 64) */
    anim = (struct MapChunkHdr *)p;
    anim->type = 0x53635441u; anim->size = 64; anim->count = 2;
    e0 = (struct MapAnimEnt *)(anim + 1);
    e0->a = 2; e0->kind = 4;  e0->value = 100; e0->p = (u8 *)0x10; e0->q = (u8 *)0x20;
    e1 = e0 + 1;
    e1->a = 4; e1->kind = 16; e1->value = 8;   e1->p = (u8 *)0x30; e1->q = (u8 *)0x40;
    p += 64;

    /* 4. SLFX #1 (top nibble 1 -> sets flag and masks ~0xFC) */
    slfx1 = (struct MapChunkHdr *)p;
    slfx1->type = 0x534C4658u; slfx1->size = 12; slfx1->count = (s32)0x100000FFu;
    p += 12;

    /* 5. SLFX #2 (top nibble 2 -> unmasked) */
    slfx2 = (struct MapChunkHdr *)p;
    slfx2->type = 0x534C4658u; slfx2->size = 12; slfx2->count = (s32)0x200000FFu;
    p += 12;

    /* 6. SLFX #3 (duplicate of #1 -> deduplicates to index 0) */
    slfx3 = (struct MapChunkHdr *)p;
    slfx3->type = 0x534C4658u; slfx3->size = 12; slfx3->count = (s32)0x100000FFu;
    p += 12;

    /* 7. Unknown tag */
    unk = (struct MapChunkHdr *)p;
    unk->type = 0x58585858u; unk->size = 12;
    p += 12;

    gParseMapSectionChunks(buf, (u32)(p - buf), 3);

    FCK_CHECK(gTileBlockTable[3] == sect, "stores section pointer at table[id]");
    FCK_CHECK(sect->c == (u8 *)sect + 0x5D8, "sect->c = sect + 0x5D0 + n*4");
    FCK_CHECK(sect->a == (u8 *)sect + 0x5DE, "sect->a = base + hi*2");
    FCK_CHECK(sect->b == (u8 *)sect + 0x5E8, "sect->b = a + low*2");
    FCK_CHECK(gMapTileAnimList == anim, "ScTA linked onto gMapTileAnimList");
    FCK_CHECK(*(void **)anim == sentinel_anim, "ScTA links previous head at +0");
    FCK_CHECK(e0->p == (u8 *)anim + 12 + 32 + 0x10, "e0->p relocated from anim payload base");
    FCK_CHECK(e0->q == sect->c + 0x20, "e0->q relocated from sect->c");
    FCK_EQ(e0->value, 100, "kind < 16 preserves entry value");
    FCK_CHECK(e1->p == (u8 *)anim + 12 + 32 + 0x30, "e1->p relocated from anim payload base");
    FCK_CHECK(e1->q == sect->c + 0x40, "e1->q relocated from sect->c");
    FCK_EQ(e1->value, 0x1000, "kind >= 16 computes 0x20000 / (value * a)");
    FCK_EQ(gSlfxFlagByte[0], 1, "SLFX top nibble 1 sets gSlfxFlagByte[0]");
    FCK_EQ(gTileDefTable.count, 2, "SLFX deduplicates third chunk");
    FCK_EQ(gTileDefTable.values[0], 0x10000003u, "top nibble 1 clears bits 0xFC");
    FCK_EQ(gTileDefTable.values[1], 0x200000FFu, "top nibble 2 keeps bits 0xFC");
    FCK_EQ(sect->index, 0, "deduplicated SLFX assigns matched index 0 to section");
}

struct GrdbRes {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    struct Vec3s32 unk0C;
    s32 unk18;
};
struct GrdbDesc {
    u8 *unk00;
    u8  pad04[0x1C];
    u32 unk20;
};

static void test_init_grdb(void)
{
    u8 buf[128];
    struct GrdbDesc desc;
    struct GrdbRes *res2;
    u8 fake_cell_map[16];
    struct FckCall c;

    /* @leaf 80063DC8 source src/game/memory-resources/gInitMapFromGrdb.c: last-GRDB scan, origin rotate/negate, world bounds + 0x360000 inverse spans, and grid allocations */
    LK_FUNC("func_80063DC8");
    lk_fill(buf, 0, sizeof(buf));
    lk_fill(&desc, 0, sizeof(desc));
    lk_fill(gMapGridBounds, 0, sizeof(gMapGridBounds));
    lk_fill(gCellClipRect, 0, sizeof(gCellClipRect));
    gMapGridHeader = 0;
    gMapChunkNode = 0;
    gCellMap = 0;
    mock_alloc_ret = fake_cell_map;

    /* Chunk 0: non-GRDB (16 bytes) */
    *(u32 *)(buf + 0) = 0x11111111u;
    *(u32 *)(buf + 4) = 16;
    /* Chunk 1: first GRDB (48 bytes) - must be superseded */
    *(u32 *)(buf + 16) = 0x47524442u;
    *(u32 *)(buf + 20) = 48;
    /* Chunk 2: second GRDB (48 bytes) */
    *(u32 *)(buf + 64) = 0x47524442u;
    *(u32 *)(buf + 68) = 48;
    res2 = (struct GrdbRes *)(buf + 64 + 0xC);
    res2->unk04 = 9;
    res2->unk08 = 5;
    res2->unk0C.x = 0x10000;
    res2->unk0C.y = 0x20000;
    res2->unk0C.z = 0x30000;
    res2->unk18 = 1;

    desc.unk00 = buf;
    desc.unk20 = 112;
    fck_log_reset();
    gInitMapFromGrdb(&desc);

    FCK_CHECK(gMapChunkNode == &desc, "publishes descriptor in gMapChunkNode");
    FCK_CHECK(gMapGridHeader == res2, "publishes last GRDB payload in gMapGridHeader");
    FCK_CHECK(res2->unk0C.x == -0x20000 && res2->unk0C.y == 0 && res2->unk0C.z == -0x10000,
              "origin rotated (x,y,z)->(y,0,x) and negated in place");
    FCK_EQ(gMapGridBounds[0], 0x30000, "bound[0] = old_x + border<<17");
    FCK_EQ(gMapGridBounds[1], 0xF0000, "bound[1] = old_x + (w-border-1)<<17");
    FCK_EQ(gMapGridBounds[2], 0x40000, "bound[2] = old_y + border<<17");
    FCK_EQ(gMapGridBounds[3], 0x70000, "bound[3] = old_x + (h-border-1)<<17");
    FCK_EQ(gMapGridBounds[4], 0x47FEE, "bound[4] = FIXMUL(0x360000, 0x80000000/0x60000)");
    FCK_EQ(gMapGridBounds[5], 0x11FFEE, "bound[5] = FIXMUL(0x360000, 0x80000000/0x18000)");
    FCK_CHECK(gCellClipRect[4] == 32 && gCellClipRect[5] == 16, "cell clip limits (w-1)*4, (h-1)*4");
    FCK_CHECK(gCellMap == fake_cell_map, "stores allocated cell map");
    c = fck_call_n("alloc", 0); FCK_CHECK(c.a0 == 512 && c.a1 == 2, "allocates (w-1)*(h-1)*16 tag 2");
    c = fck_call_n("alloc_light_grid", 0); FCK_CHECK(c.a0 == 8 && c.a1 == 4, "sizes light grid to (w-1, h-1)");
    c = fck_call_n("alloc_cell_bucket", 0); FCK_CHECK(c.a0 == 8 && c.a1 == 4, "sizes bucket grid to (w-1, h-1)");
}

static void test_tile_pattern(void)
{
    /* @leaf 800654F8 source src/game/gpu-2d-hud/gBuildTilePattern.c: mode 0 sine-grid clamp/flatten/mask and mode 1 mirrored 4-byte blend */
    LK_FUNC("func_800654F8");
    lk_fill(gTilePatternBuf, 0xAA, sizeof(gTilePatternBuf));
    lk_fill(gTilePatternCounterState, 0, sizeof(gTilePatternCounterState));

    /* Mode 0, mode=0 (flat rows), n=0, shift=0 (+8), phase=100, speed=1, gFrameTime=0x200 */
    gFrameTime = 0x200;
    gBuildTilePattern(0x00006401u);
    FCK_EQ(gTilePatternBuf[0], 0, "out[0] clamps negative -12 to 0");
    FCK_EQ(gTilePatternBuf[8], 116, "out[8] = (+0x1000>>8) + 100");
    FCK_EQ(gTilePatternBuf[15], 4, "out[15] = (-0x6000>>8) + 100");
    FCK_CHECK(gTilePatternBuf[16] == 0 && gTilePatternBuf[31] == 4, "mode=0 repeats row 0 across all rows");

    /* Mode 0, mode=1 (2D), n=1 (mask=0x1FFF), shift=0 (+8), phase=240, speed=0 */
    gFrameTime = 0;
    gBuildTilePattern(0x0101F000u);
    FCK_EQ(gTilePatternBuf[247], 240, "masked angle 0x1000 skips sine lookup -> phase 240");
    FCK_EQ(gTilePatternBuf[248], 255, "angle 0xE00 clamps 336 to 255");

    /* Mode 1: base=40, sh=1 -> input=0x10002801 */
    gTilePatternCounterState[0] = 100; gTilePatternCounterState[0x100] = 60;
    gTilePatternCounterState[1] = 50;  gTilePatternCounterState[0x101] = 80;
    gTilePatternCounterState[2] = 200; gTilePatternCounterState[0x102] = 150;
    gTilePatternCounterState[3] = 30;  gTilePatternCounterState[0x103] = 30;
    gTilePatternCounterState[255] = 120; gTilePatternCounterState[0x1FF] = 100;
    gBuildTilePattern(0x10002801u);
    FCK_EQ(gTilePatternBuf[0], 50, "p<v mirrors v0 to 20 -> (80+20)>>1");
    FCK_EQ(gTilePatternBuf[1], 65, "p>=v keeps v1=50 -> (80+50)>>1");
    FCK_EQ(gTilePatternBuf[2], 90, "p<v mirrors v2 to 100 -> (80+100)>>1");
    FCK_EQ(gTilePatternBuf[3], 55, "p==v keeps v3=30 -> (80+30)>>1");
    FCK_EQ(gTilePatternBuf[255], 80, "64th iteration blends last byte (80+80)>>1");

    /* Unknown top nibble leaves buffer untouched */
    lk_fill(gTilePatternBuf, 0x5A, sizeof(gTilePatternBuf));
    gBuildTilePattern(0x20000000u);
    FCK_CHECK(lk_all(gTilePatternBuf, 0, 256, 0x5A), "unknown command leaves tile buffer untouched");
}

/* =========================================================================
 * Group 4: Spawn Constructors (Kind 0x59 and Kind 0x57 Emitter)
 * ========================================================================= */

struct Src59 {
    u8  unk00[0x34];
    u16 unk34; u8 unk36; u8 pad37;
    u16 unk38, unk3A, unk3C;
    u8  unk3E, unk3F, unk40, unk41, unk42, unk43, unk44, unk45, unk46, unk47, unk48, unk49;
    s16 unk4A, unk4C, unk4E;
    u16 unk50; u8 unk52; u8 pad53;
    u16 unk54, unk56;
};

struct Dest59 {
    u8  unk00[8];
    s16 unk08; u16 unk0A;
    u8  unk0C[6];
    u16 unk12;
    u8  unk14[0x10];
    s32 unk24;
    u8  unk28[0x3E];
    u16 unk66, unk68, unk6A;
    u8  unk6C, unk6D, unk6E, unk6F, unk70, unk71, unk72, unk73, unk74, unk75, unk76, unk77;
    u8  unk78[0x14];
    u16 unk8C; u8 unk8E, unk8F;
    u16 unk90; u8 unk92[2];
    s16 unk94, unk96, unk98;
    u16 unk9A;
};

static void test_kind59_ctor(void)
{
    struct Src59 src;
    struct Dest59 dst;
    struct FckCall c;
    void *ret;

    /* @leaf 8007043C source src/game/gameplay-ai/gCreateKind59Actor.c: 0x9C alloc, handler install, field copies, fixed/random angles & unk90, and scaled rotation vector */
    LK_FUNC("func_8007043C");
    lk_fill(&src, 0, sizeof(src));
    lk_fill(&dst, 0xEE, sizeof(dst));
    src.unk34 = 0x1122; src.unk36 = 7;
    src.unk38 = 0x3344; src.unk3A = 0x5566; src.unk3C = 0x7788;
    src.unk3E = 1; src.unk3F = 2; src.unk40 = 3; src.unk41 = 4;
    src.unk42 = 5; src.unk43 = 6; src.unk44 = 7; src.unk45 = 8;
    src.unk46 = 9; src.unk47 = 10; src.unk48 = 11; src.unk49 = 12;
    src.unk4A = 100; src.unk4C = 200; src.unk4E = 0x100;
    src.unk50 = 0x99AA; src.unk52 = 0x42; src.unk54 = 300; src.unk56 = 0xBBCC;
    mock_rot_x = 0x10000; mock_rot_y = -0x20000; mock_rot_z = 0x30000;

    /* Alloc failure returns 0 */
    mock_alloc_ret = 0;
    fck_log_reset();
    ret = gCreateKind59Actor(&src);
    FCK_CHECK(ret == 0 && fck_calls_to("init_spawn_motion") == 0, "alloc failure returns 0 early");

    /* Non-negative angles and unk54 */
    mock_alloc_ret = &dst;
    fck_log_reset();
    ret = gCreateKind59Actor(&src);
    FCK_CHECK(ret == &dst, "returns allocated dest");
    c = fck_call_n("alloc", 0); FCK_CHECK(c.a0 == 0x9C && c.a1 == 1, "allocates 0x9C bytes tag 1");
    c = fck_call_n("init_spawn_motion", 0); FCK_CHECK(c.a0 == (s32)&dst && c.a1 == (s32)&src, "inits spawn motion");
    FCK_EQ(dst.unk24, (s32)gKind59GlowActorHandler, "installs handler address at +0x24");
    FCK_CHECK(dst.unk12 == 0x1122 && dst.unk66 == 0x3344 && dst.unk68 == 0x5566 && dst.unk6A == 0x7788,
              "copies u16 shape fields");
    FCK_CHECK(dst.unk6C == 1 && dst.unk71 == 6 && dst.unk77 == 12, "copies 12 colour/param bytes");
    FCK_CHECK(dst.unk8C == 0xBBCC && dst.unk8E == 7 && dst.unk8F == 0x42 && dst.unk9A == 0x99AA,
              "copies trailer fields");
    FCK_CHECK(dst.unk0A == 0 && dst.unk08 == 8, "unk0A=0 and unk08=unk8E+1");
    FCK_EQ(fck_calls_to("rng"), 0, "non-negative fields skip RNG");
    c = fck_call_n("rot_mat", 0); FCK_CHECK(c.a0 == 100 && c.a1 == -200 && c.a2 == 0, "rot_mat gets (x, -y, 0)");
    FCK_CHECK(dst.unk94 == 256 && dst.unk96 == -512 && dst.unk98 == 768, "scaled matrix row");
    FCK_EQ(dst.unk90, 300, "non-negative unk54 copied to unk90");

    /* Zero unk4A takes non-negative path without RNG */
    src.unk4A = 0; src.unk4C = 0; src.unk54 = 0;
    fck_log_reset();
    ret = gCreateKind59Actor(&src);
    FCK_EQ(fck_calls_to("rng"), 0, "zero 4A/4C/54 do not invoke RNG");

    /* Negative angles and unk54 invoke RNG modulo */
    src.unk4A = -10; src.unk4C = -20; src.unk54 = (u16)-30;
    mock_rng_idx = 0;
    mock_rng_seq[0] = 23; mock_rng_seq[1] = 55; mock_rng_seq[2] = 100;
    fck_log_reset();
    ret = gCreateKind59Actor(&src);
    FCK_EQ(fck_calls_to("rng"), 3, "negative 4A/4C/54 call RNG 3 times");
    c = fck_call_n("rot_mat", 0); FCK_CHECK(c.a0 == 3 && c.a1 == -15 && c.a2 == 0, "RNG modulo angles (23%10, -(55%20), 0)");
    FCK_EQ(dst.unk90, 10, "RNG modulo unk90 (100%30)");
}

struct Src57 {
    u8  pad00[0x34];
    u16 a34; s16 a36;
    u8  colors[9]; u8 a41;
    s16 a42, a44, a46, a48, a4a, a4c, a4e, a50, a52, a54;
    u16 a56;
};

struct Dest57 {
    u8  pad00[8];
    s16 a08, a0a;
    u8  pad0c[6];
    u16 a12;
    u8  pad14[0x10];
    s32 a24;
    u8  pad28[0x3c];
    u8  pool[12];
    s16 a70; u8 a72;
    u8  colors[3][3];
    s16 slopes[3][3];
    u16 a8e;
    s16 a90, a92, a94, a96, a98, a9a, a9c, a9e;
};

static void test_emitter_ctor(void)
{
    struct Src57 src;
    struct Dest57 dst;
    struct FckCall c;
    void *ret;

    /* @leaf 800762B0 source src/game/gameplay-ai/gCreateEmitterObject.c: 0xA0 alloc + pool rollback, colour transpose, 4.12 slopes, rate, and 0x8000/0x4000 flags */
    LK_FUNC("func_800762B0");
    lk_fill(&src, 0, sizeof(src));
    lk_fill(&dst, 0xEE, sizeof(dst));
    src.a34 = 0x2222; src.a36 = 4;
    src.colors[0] = 10; src.colors[1] = 20; src.colors[2] = 30;
    src.colors[3] = 26; src.colors[4] = 52; src.colors[5] = 30;
    src.colors[6] = 58; src.colors[7] = 20; src.colors[8] = 94;
    src.a41 = 3;
    src.a42 = 16; src.a44 = 32; src.a46 = 64;
    src.a48 = 10; src.a4a = 50; src.a4c = 60;
    src.a4e = 0x200; src.a50 = 0; src.a52 = 0; src.a54 = 7;
    src.a56 = 0x0015;
    mock_rot_x = 0x10000; mock_rot_y = 0x20000; mock_rot_z = -0x10000;

    /* Alloc failure */
    mock_alloc_ret = 0;
    fck_log_reset();
    ret = gCreateEmitterObject(&src);
    FCK_CHECK(ret == 0 && fck_calls_to("init_pool") == 0, "alloc failure returns 0 early");

    /* Pool init failure frees block and returns 0 */
    mock_alloc_ret = &dst;
    mock_init_pool_ret = 0;
    fck_log_reset();
    ret = gCreateEmitterObject(&src);
    FCK_CHECK(ret == 0 && fck_calls_to("free") == 1, "pool init failure frees block and returns 0");
    c = fck_call_n("free", 0); FCK_EQ(c.a0, (s32)&dst, "frees allocated dest");

    /* Full success path */
    mock_init_pool_ret = 1;
    fck_log_reset();
    ret = gCreateEmitterObject(&src);
    FCK_CHECK(ret == &dst, "returns allocated emitter");
    c = fck_call_n("alloc", 0); FCK_CHECK(c.a0 == 0xA0 && c.a1 == 1, "allocates 0xA0 bytes tag 1");
    c = fck_call_n("init_pool", 0); FCK_CHECK(c.a0 == (s32)dst.pool && c.a1 == 0x30 && c.a2 == 0x28, "inits 40x0x30 pool");
    FCK_EQ(dst.a24, (s32)gAttachMessageName, "installs handler at +0x24");
    FCK_CHECK(dst.a72 == 3 && dst.a12 == 0x2222 && dst.a90 == 10 && dst.a98 == 7, "copies scalar fields");
    FCK_CHECK(dst.a9a == 512 && dst.a9c == 1024 && dst.a9e == -512, "scaled matrix row at +0x9A..9E");
    FCK_CHECK(dst.colors[0][0] == 10 && dst.colors[0][1] == 26 && dst.colors[0][2] == 58 &&
              dst.colors[1][0] == 20 && dst.colors[1][1] == 52 && dst.colors[1][2] == 20 &&
              dst.colors[2][0] == 30 && dst.colors[2][1] == 30 && dst.colors[2][2] == 94,
              "transposes 3x3 colour keyframes");
    FCK_CHECK(dst.slopes[0][0] == 4096 && dst.slopes[1][0] == 8192 && dst.slopes[2][0] == 0,
              "step 0 slopes");
    FCK_CHECK(dst.slopes[0][1] == 4096 && dst.slopes[1][1] == -4096 && dst.slopes[2][1] == 8192,
              "step 1 slopes");
    FCK_CHECK(dst.slopes[0][2] == -3712 && dst.slopes[1][2] == -1280 && dst.slopes[2][2] == -6016,
              "step 2 fade-to-zero slopes");
    FCK_EQ(dst.a70, 146, "rate = (4 << 12) / 112");
    FCK_EQ(dst.a8e, 0xC015, "sets both 0x8000 (a4e!=0) and 0x4000 (10 < 14)");
    FCK_CHECK(dst.a0a == 10 && dst.a08 == 0, "sets a0a=10 and a08=0");

    /* Short times (<16) zero slopes; sum==0 -> a70=0; flags without 0x8000/0x4000; negative angles */
    src.a42 = 0; src.a44 = 0; src.a46 = 0;
    src.a4e = 0; src.a50 = 0; src.a52 = 0; src.a48 = 0; src.a56 = 0x0007;
    src.a4a = -12; src.a4c = -15;
    mock_rng_idx = 0; mock_rng_seq[0] = 25; mock_rng_seq[1] = 34;
    fck_log_reset();
    ret = gCreateEmitterObject(&src);
    FCK_CHECK(dst.slopes[0][0] == 0 && dst.slopes[1][1] == 0 && dst.slopes[2][2] == 0, "time<16 zeroes slopes");
    FCK_EQ(dst.a70, 0, "zero total time leaves a70=0");
    FCK_EQ(dst.a8e, 0x0007, "zero a4e/a50/a52 leaves 0x8000 and 0x4000 clear");
    c = fck_call_n("rot_mat", 0); FCK_CHECK(c.a0 == 1 && c.a1 == -4 && c.a2 == 0, "RNG angles (25%12, -(34%15), 0)");

    /* a4e==0, a52!=0 still sets 0x8000; zero a4a/a4c skip RNG */
    src.a52 = 1; src.a4a = 0; src.a4c = 0;
    fck_log_reset();
    gCreateEmitterObject(&src);
    FCK_EQ(dst.a8e, 0x8007, "a52!=0 sets 0x8000 when a4e==0");
    FCK_EQ(fck_calls_to("rng"), 0, "zero a4a/a4c skip RNG");
}

int main(void)
{
    test_camera_eye();
    test_player_damage();
    test_radar_markers();
    test_dome_alloc();
    test_cheat_codes();
    test_unit_retirement();
    test_single_view_grid();
    test_parse_map_chunks();
    test_init_grdb();
    test_tile_pattern();
    test_kind59_ctor();
    test_emitter_ctor();
    return lk_done("leaf_scene");
}
