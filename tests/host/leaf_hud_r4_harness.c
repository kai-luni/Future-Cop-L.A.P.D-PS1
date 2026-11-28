/* HUD/geometry leaf suite (round 4): unchanged game C under qemu-mipsel.
 * Expectations are derived from the named units' source, not captured output.
 */
/* @link 8001846C */
/* @link 80019668 */
#include "leaf_kit.h"

/* Sparse quarter-wave fixture for the real gSinCos. Deliberately distinct
 * values for each axis make swapped angle inputs and signs observable.
 */
u16 gSineTable[0x401] __asm__("D_8009A234") = {
    [1] = 4096, [2] = 8192, [3] = 12288,
    [0x3FD] = 20480, [0x3FE] = 16384, [0x3FF] = 24576,
    [0x400] = 32768
};

extern void gBuildRotationMatrix(s32 x, s32 y, s32 z, s32 *matrix)
    __asm__("func_800188D0");

/* @leaf 800188D0 source Angle masking, all axis combinations, and nine fixed-point coefficients. */
static void test_800188D0(void)
{
    static const struct {
        s32 angles[3];
        s32 matrix[9];
    } cases[] = {
        {{0x1000, -0x1000, 0x2000}, {65536, 0, 0, 0, 65536, 0, 0, 0, 65536}},
        {{0x1001, 0, 0}, {49152, 0, -8192, 0, 65536, 0, 8192, 0, 49152}},
        {{0, -2, 0}, {65536, 0, 0, 0, 32768, -16384, 0, 16384, 32768}},
        {{0, 0, 0x2003}, {40960, 24576, 0, -24576, 40960, 0, 0, 0, 65536}},
        {{1, -2, 0}, {49152, 0, -8192, -2048, 32768, -12288, 4096, 16384, 24576}},
        {{1, 0, 3}, {30720, 24576, -5120, -18432, 40960, 3072, 8192, 0, 49152}},
        {{0, -2, 3}, {40960, 12288, -6144, -24576, 20480, -10240, 0, 16384, 32768}},
        /* Reduced sine/cosine inputs are shifted by two. The general path's
         * cross terms t and u are then -1536 and -2560 respectively.
         */
        {{0x1001, -2, 0x2003}, {29952, 12288, -9728, -19712, 20480, -4608, 4096, 16384, 24576}}
    };
    s32 guarded[11];
    unsigned i, j;

    LK_FUNC("func_800188D0");
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        lk_fill(guarded, 0x5A, sizeof(guarded));
        gBuildRotationMatrix(cases[i].angles[0], cases[i].angles[1],
                             cases[i].angles[2], &guarded[1]);
        for (j = 0; j < 9; j++)
            FCK_EQ(guarded[j + 1], cases[i].matrix[j], "matrix coefficient");
        FCK_EQ(guarded[0], 0x5A5A5A5A, "leading matrix guard");
        FCK_EQ(guarded[10], 0x5A5A5A5A, "trailing matrix guard");
    }
}

/* The decoder's value plane follows the byte-length plane in each table. */
struct VlcPrimary {
    u8 length[0x2000];
    u16 value[0x2000];
};

struct VlcSecondary {
    u8 length[0x200];
    u16 value[0x200];
};

static struct {
    u32 before;
    struct VlcPrimary table;
    u32 after;
} primary;

static struct {
    u32 before;
    struct VlcSecondary table;
    u32 after;
} secondary;

u8 *gMovieVlcTableA __asm__("D_8009F51C");
u8 *gMovieVlcTableB __asm__("D_8009F520");

extern s32 gMovieDecodeVlcStream(u16 *input, u16 *output)
    __asm__("func_80089BBC");

static void check_decode(u16 *input, unsigned words, const u16 *decoded,
                         unsigned decoded_words)
{
    u16 guarded[72], original[12];
    unsigned i;

    memcpy(original, input, words * sizeof(*input));
    lk_fill(guarded, 0x5A, sizeof(guarded));
    FCK_EQ(gMovieDecodeVlcStream(input, &guarded[1]), 0, "decode returns zero");
    FCK_EQ(guarded[0], 0x5A5A, "leading decode guard");
    for (i = 0; i < decoded_words; i++)
        FCK_EQ(guarded[1 + i], decoded[i], "decoded word");
    for (i = decoded_words; i < decoded_words + 64; i++)
        FCK_EQ(guarded[1 + i], 0xFE00, "64-word end padding");
    FCK_CHECK(lk_all(guarded, (1 + i) * sizeof(*guarded), sizeof(guarded), 0x5A),
              "decoder stops after padding");
    FCK_EQ(memcmp(input, original, words * sizeof(*input)), 0, "input unchanged");
}

/* @leaf 80089BBC source Header, immediate terminator, primary/secondary paths, escape, refill, EOB and padding. */
static void test_80089BBC(void)
{
    /* Header words 0/1 copy verbatim. Word 2 is shifted into the DC prefix;
     * word 3 is ignored. The stream starts at word 4, MSB-first in each word:
     * 0101010101 | 101 | 00000000 10 | 1001001000110100 | 11 | 0111111111
     * DC=0x155, a primary symbol, a secondary escape with raw word 0x9234,
     * then primary EOB and the 0x1FF DC terminator. This crosses input words.
     */
    u16 stream[12] = {0x1234, 0xFEDC, 5, 0xBEEF, 0x5568, 0x0524, 0x69BF, 0xE000};
    u16 end[12] = {0xACE1, 0x0087, 7, 0xBAD0, 0x7FC0};
    static const u16 decoded[] = {0x1234, 0xFEDC, 0x1555, 0x2468, 0x9234, 0xFE00};
    static const u16 empty[] = {0xACE1, 0x0087};

    LK_FUNC("func_80089BBC");
    gMovieVlcTableA = primary.table.length;
    gMovieVlcTableB = secondary.table.length;
    primary.table.length[5122] = 3;
    primary.table.value[5122] = 0x2468;
    secondary.table.length[329] = 2;
    secondary.table.value[329] = 0x7C1F;
    primary.table.length[7166] = 2;
    primary.table.value[7166] = 0xFE00;
    check_decode(end, 12, empty, sizeof(empty) / sizeof(empty[0]));
    check_decode(stream, 12, decoded, sizeof(decoded) / sizeof(decoded[0]));
    FCK_CHECK(gMovieVlcTableA == primary.table.length, "primary table pointer unchanged");
    FCK_CHECK(gMovieVlcTableB == secondary.table.length, "secondary table pointer unchanged");
}

struct VlcEntry {
    u8 bits;
    u8 pad;
    u16 code;
};

struct VlcEntry gPrimaryEntries[0x60] __asm__("D_8009C1E0");
struct VlcEntry gSecondaryEntries[0x80] __asm__("D_8009C360");
u16 gDefaultSymbols[0xE0] __asm__("D_8009C560");

extern s32 gBuildMovieVlcTables(u8 *table_a, u8 *table_b, u16 *symbols)
    __asm__("func_80089A88");

static void check_vlc_range(const u8 *length, const u16 *value,
                            unsigned start, unsigned end, u8 bits, u16 symbol)
{
    unsigned i;
    int matching_lengths = 1;
    int matching_values = 1;

    for (i = start; i < end; i++) {
        matching_lengths &= length[i] == bits;
        matching_values &= value[i] == symbol;
    }
    FCK_CHECK(matching_lengths, "VLC length range");
    FCK_CHECK(matching_values, "VLC value range");
}

/* @leaf 80089A88 source Pointer publication, null rejection, explicit/default symbols, and both table layouts. */
static void test_80089A88(void)
{
    struct VlcPrimary *a = &primary.table;
    struct VlcSecondary *b = &secondary.table;
    u16 custom[0xE0];
    unsigned i, pass;
    u16 base;

    LK_FUNC("func_80089A88");
    lk_fill(&primary, 0x5A, sizeof(primary));
    lk_fill(&secondary, 0x5A, sizeof(secondary));
    FCK_EQ(gBuildMovieVlcTables(0, b->length, custom), -1, "null primary rejected");
    FCK_CHECK(gMovieVlcTableA == 0, "null primary published");
    FCK_CHECK(gMovieVlcTableB == b->length, "secondary published on failure");
    FCK_CHECK(lk_all(&secondary, 0, sizeof(secondary), 0x5A), "secondary unchanged on failure");
    FCK_EQ(gBuildMovieVlcTables(a->length, 0, custom), -1, "null secondary rejected");
    FCK_CHECK(gMovieVlcTableA == a->length, "primary published on failure");
    FCK_CHECK(gMovieVlcTableB == 0, "null secondary published");
    FCK_CHECK(lk_all(&primary, 0, sizeof(primary), 0x5A), "primary unchanged on failure");

    /* In the primary table code >> 3 picks a 13-bit index; in the secondary
     * table code >> 7 picks a 9-bit index. Shorter codes replicate into
     * their low don't-care bits. Keep the two symbol sequences distinct.
     */
    for (i = 0; i < 0x60; i++) {
        gPrimaryEntries[i].bits = 13;
        gPrimaryEntries[i].code = (0x80 + i) << 3;
    }
    gPrimaryEntries[0].bits = 11;
    gPrimaryEntries[0].code = 0x40 << 3;
    gPrimaryEntries[0x5F].bits = 11;
    gPrimaryEntries[0x5F].code = 0x1FFC << 3;
    for (i = 0; i < 0x80; i++) {
        gSecondaryEntries[i].bits = 9;
        gSecondaryEntries[i].code = (0x40 + i) << 7;
    }
    gSecondaryEntries[0].bits = 8;
    gSecondaryEntries[0].code = 0x20 << 7;
    gSecondaryEntries[0x7F].bits = 7;
    gSecondaryEntries[0x7F].code = 0x1FC << 7;
    for (i = 0; i < 0xE0; i++) {
        custom[i] = 0x1200 + i;
        gDefaultSymbols[i] = 0xAB00 + i;
    }

    for (pass = 0; pass < 2; pass++) {
        base = pass == 0 ? 0x1200 : 0xAB00;
        lk_fill(&primary, 0x5A, sizeof(primary));
        lk_fill(&secondary, 0x5A, sizeof(secondary));
        FCK_EQ(gBuildMovieVlcTables(a->length, b->length, pass == 0 ? custom : 0), 0,
               "successful table build");
        FCK_CHECK(gMovieVlcTableA == a->length, "primary published");
        FCK_CHECK(gMovieVlcTableB == b->length, "secondary published");

        check_vlc_range(a->length, a->value, 0, 0x40, 0x5A, 0x5A5A);
        check_vlc_range(a->length, a->value, 0x40, 0x44, 11, base);
        check_vlc_range(a->length, a->value, 0x44, 0x81, 0x5A, 0x5A5A);
        for (i = 1; i < 0x5F; i++) {
            FCK_EQ(a->length[0x80 + i], 13, "primary full-length code");
            FCK_EQ(a->value[0x80 + i], base + i, "primary symbol order");
        }
        check_vlc_range(a->length, a->value, 0xDF, 0x1FFC, 0x5A, 0x5A5A);
        check_vlc_range(a->length, a->value, 0x1FFC, 0x2000, 11, base + 0x5F);

        check_vlc_range(b->length, b->value, 0, 0x20, 0x5A, 0x5A5A);
        check_vlc_range(b->length, b->value, 0x20, 0x22, 8, base + 0x60);
        check_vlc_range(b->length, b->value, 0x22, 0x41, 0x5A, 0x5A5A);
        for (i = 1; i < 0x7F; i++) {
            FCK_EQ(b->length[0x40 + i], 9, "secondary full-length code");
            FCK_EQ(b->value[0x40 + i], base + 0x60 + i, "secondary symbol order");
        }
        check_vlc_range(b->length, b->value, 0xBF, 0x1FC, 0x5A, 0x5A5A);
        check_vlc_range(b->length, b->value, 0x1FC, 0x200, 7, base + 0xDF);

        FCK_EQ(primary.before, 0x5A5A5A5A, "primary leading guard");
        FCK_EQ(primary.after, 0x5A5A5A5A, "primary trailing guard");
        FCK_EQ(secondary.before, 0x5A5A5A5A, "secondary leading guard");
        FCK_EQ(secondary.after, 0x5A5A5A5A, "secondary trailing guard");
    }
}

/* Camera fixtures expose only the fields and offsets used by the leaf units. */
struct Vec3 {
    s32 x, y, z;
};

struct ViewPreset {
    s16 angle, pitch;
    s32 distance;
    u8 data[12];
};

struct CameraInfo {
    u8 pad00[0xA8];
    s16 bias_a8;
    u8 padAA[0x12];
    s32 bias_bc;
};

struct CameraObject {
    u8 pad00[0x5C];
    struct Vec3 position;
    u8 pad68[0x44];
    struct CameraInfo *info;
    s16 heading;
};

struct ViewPreset gViewPresets[17] __asm__("D_8009AD08");
s32 gPrimaryPrngCursor __asm__("D_8009F5A8");

static struct CameraInfo camera_info;
static struct CameraObject camera;
static struct CameraEyeCall {
    int step;
    u32 position_address;
    struct Vec3 position;
    s32 heading, angle, pitch, distance;
    const u8 *data;
    s32 *out;
    s32 bias_a8, bias_bc;
    s32 prng_cursor;
} eye_calls[8];

static int camera_step, eye_count;
static struct {
    int count, step;
    s32 angle, pitch, roll;
} rotation_call;
static struct {
    int count, step;
    s32 *out;
    struct Vec3 value;
} position_call;

/* Observe these boundaries directly: eye calculation is given the exact
 * preset fields and a copied origin; setters must see the produced eye in
 * the right order without touching the application's active camera state.
 */
void record_eye(struct Vec3 *position, s32 heading, s32 angle, s32 pitch,
                s32 distance, u8 *data, s32 *out, s32 a8, s32 bc)
    __asm__("func_8005612C");
void record_eye(struct Vec3 *position, s32 heading, s32 angle, s32 pitch,
                s32 distance, u8 *data, s32 *out, s32 a8, s32 bc)
{
    struct CameraEyeCall *call;

    camera_step++;
    if (eye_count < 8) {
        call = &eye_calls[eye_count];
        call->step = camera_step;
        call->position_address = (u32)position;
        call->position = *position;
        call->heading = heading;
        call->angle = angle;
        call->pitch = pitch;
        call->distance = distance;
        call->data = data;
        call->out = out;
        call->bias_a8 = a8;
        call->bias_bc = bc;
        call->prng_cursor = gPrimaryPrngCursor;
    }
    eye_count++;
    out[0] = distance;
    out[1] = angle;
    out[2] = pitch;
}

void record_rotation(s32 angle, s32 pitch, s32 roll) __asm__("func_80017860");
void record_rotation(s32 angle, s32 pitch, s32 roll)
{
    rotation_call.count++;
    rotation_call.step = ++camera_step;
    rotation_call.angle = angle;
    rotation_call.pitch = pitch;
    rotation_call.roll = roll;
}

void record_position(s32 *out) __asm__("func_800177AC");
void record_position(s32 *out)
{
    position_call.count++;
    position_call.step = ++camera_step;
    position_call.out = out;
    position_call.value.x = out[0];
    position_call.value.y = out[1];
    position_call.value.z = out[2];
}

static void reset_camera(void)
{
    camera.position.x = 314;
    camera.position.y = -271;
    camera.position.z = 159;
    camera.heading = -600;
    camera.info = &camera_info;
    camera_info.bias_a8 = -1234;
    camera_info.bias_bc = 0x12345678;
    camera_step = 0;
    eye_count = 0;
    memset(eye_calls, 0, sizeof(eye_calls));
    memset(&rotation_call, 0, sizeof(rotation_call));
    memset(&position_call, 0, sizeof(position_call));
}

static void check_eye_call(const struct CameraEyeCall *call, int step,
                           int slot, s32 angle, s32 pitch, s32 distance)
{
    FCK_EQ(call->step, step, "eye calculation order");
    FCK_CHECK(call->position_address != (u32)&camera.position, "origin copied to a local");
    FCK_EQ(call->position.x, 314, "origin x forwarded");
    FCK_EQ(call->position.y, -271, "origin y forwarded");
    FCK_EQ(call->position.z, 159, "origin z forwarded");
    FCK_EQ(call->heading, -600, "signed heading forwarded");
    FCK_EQ(call->angle, angle, "heading plus preset angle");
    FCK_EQ(call->pitch, pitch, "signed preset pitch forwarded");
    FCK_EQ(call->distance, distance, "preset distance forwarded");
    FCK_CHECK(call->data == gViewPresets[slot].data, "exact preset data pointer");
    FCK_EQ(call->bias_a8, -1234, "signed halfword bias forwarded");
    FCK_EQ(call->bias_bc, 0x12345678, "word bias forwarded");
}

extern void gPlaceCameraFromPreset(struct CameraObject *object, s32 index,
                                   s32 *out, s32 apply)
    __asm__("func_8005678C");

/* @leaf 8005678C source Preset forwarding and compute/rotate/position order for query and apply. */
static void test_8005678C(void)
{
    static const struct {
        s32 slot, apply, angle, pitch, distance;
    } cases[] = {
        {4, 0, -710, 330, -74565},
        {6, -7, 250, -451, 0x76543}
    };
    s32 guarded[5];
    unsigned i;

    LK_FUNC("func_8005678C");
    gViewPresets[4].angle = -110;
    gViewPresets[4].pitch = 330;
    gViewPresets[4].distance = -74565;
    gViewPresets[6].angle = 850;
    gViewPresets[6].pitch = -451;
    gViewPresets[6].distance = 0x76543;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        reset_camera();
        lk_fill(guarded, 0x5A, sizeof(guarded));
        gPlaceCameraFromPreset(&camera, cases[i].slot, &guarded[1], cases[i].apply);
        FCK_EQ(eye_count, 1, "exactly one eye calculation");
        check_eye_call(&eye_calls[0], 1, cases[i].slot, cases[i].angle,
                       cases[i].pitch, cases[i].distance);
        FCK_CHECK(eye_calls[0].out == &guarded[1], "caller output pointer forwarded");
        FCK_EQ(guarded[1], cases[i].distance, "eye x retained");
        FCK_EQ(guarded[2], cases[i].angle, "eye y retained");
        FCK_EQ(guarded[3], cases[i].pitch, "eye z retained");
        FCK_EQ(guarded[0], 0x5A5A5A5A, "leading eye guard");
        FCK_EQ(guarded[4], 0x5A5A5A5A, "trailing eye guard");
        FCK_EQ(camera.heading, -600, "object heading unchanged");
        FCK_EQ(camera.position.x, 314, "object x unchanged");
        FCK_EQ(camera.position.y, -271, "object y unchanged");
        FCK_EQ(camera.position.z, 159, "object z unchanged");
        if (cases[i].apply == 0) {
            FCK_EQ(rotation_call.count, 0, "query does not rotate");
            FCK_EQ(position_call.count, 0, "query does not move");
        } else {
            FCK_EQ(rotation_call.count, 1, "apply rotates once");
            FCK_EQ(rotation_call.step, 2, "rotation follows eye calculation");
            FCK_EQ(rotation_call.angle, cases[i].angle, "rotation receives preset yaw");
            FCK_EQ(rotation_call.pitch, cases[i].pitch, "rotation receives preset pitch");
            FCK_EQ(rotation_call.roll, 0, "rotation roll is zero");
            FCK_EQ(position_call.count, 1, "apply moves once");
            FCK_EQ(position_call.step, 3, "position follows rotation");
            FCK_CHECK(position_call.out == &guarded[1], "position receives caller output pointer");
            FCK_EQ(position_call.value.x, cases[i].distance, "position receives computed x");
            FCK_EQ(position_call.value.y, cases[i].angle, "position receives computed y");
            FCK_EQ(position_call.value.z, cases[i].pitch, "position receives computed z");
        }
    }
}

s32 gPrimaryPrngState[20] __asm__("D_800A0480");

static int sight_count, clear_on;
static struct SightCall {
    int step;
    u32 from_address;
    int to_is_eye_output;
    struct Vec3 from, to;
} sight_calls[8];

/* The map-dependent sight test is the only additional stand-in for search.
 * The PRNG and camera preset placement run as real C units.
 */
s32 record_sight_line(struct Vec3 *from, struct Vec3 *to) __asm__("func_80055EE8");
s32 record_sight_line(struct Vec3 *from, struct Vec3 *to)
{
    struct SightCall *call;

    camera_step++;
    if (sight_count < 8) {
        call = &sight_calls[sight_count];
        call->step = camera_step;
        call->from_address = (u32)from;
        call->to_is_eye_output = (u32)to == (u32)eye_calls[sight_count].out;
        call->from = *from;
        call->to = *to;
    }
    sight_count++;
    return sight_count == clear_on ? 0 : -27;
}

extern s32 gFindClearCameraPreset(struct CameraObject *object) __asm__("func_80056870");

/* @leaf 80056870 source Real PRNG seed selection, ordered placement/raycast, wraparound, last success and exhaustion. */
static void test_80056870(void)
{
    static const struct {
        s32 seed_word, clear_on, result, tries;
        s32 slots[8];
    } cases[] = {
        {0x208, 1, 9, 1, {9}},
        {0x206, 8, 14, 8, {15, 16, 9, 10, 11, 12, 13, 14}},
        {0x207, 0, -1, 8, {16, 9, 10, 11, 12, 13, 14, 15}}
    };
    const struct SightCall *sight;
    const struct CameraEyeCall *eye;
    s32 j, slot;
    unsigned i;

    LK_FUNC("func_80056870");
    for (slot = 9; slot <= 16; slot++) {
        gViewPresets[slot].angle = 100 + slot;
        gViewPresets[slot].pitch = -200 - slot;
        gViewPresets[slot].distance = 30000 + slot;
    }
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        reset_camera();
        sight_count = 0;
        clear_on = cases[i].clear_on;
        memset(sight_calls, 0, sizeof(sight_calls));

        /* At cursor zero the real PRNG wraps to 19 and adds word 2.
         * The search uses only the low three bits of the returned sum.
         */
        memset(gPrimaryPrngState, 0, sizeof(gPrimaryPrngState));
        gPrimaryPrngCursor = 0;
        gPrimaryPrngState[19] = 0x100;
        gPrimaryPrngState[2] = cases[i].seed_word;
        FCK_EQ(gFindClearCameraPreset(&camera), cases[i].result, "search result");
        FCK_EQ(gPrimaryPrngCursor, 19, "exactly one PRNG update");
        FCK_EQ(gPrimaryPrngState[19], 0x100 + cases[i].seed_word, "updated PRNG word");
        FCK_EQ(gPrimaryPrngState[2], cases[i].seed_word, "lagged PRNG word preserved");
        FCK_EQ(eye_count, cases[i].tries, "candidate count");
        FCK_EQ(sight_count, cases[i].tries, "sight test count");
        FCK_EQ(camera_step, 2 * cases[i].tries, "only eye/sight pairs");
        FCK_EQ(rotation_call.count, 0, "search does not rotate");
        FCK_EQ(position_call.count, 0, "search does not move");
        FCK_EQ(camera.position.x, 314, "object x unchanged");
        FCK_EQ(camera.position.y, -271, "object y unchanged");
        FCK_EQ(camera.position.z, 159, "object z unchanged");
        for (j = 0; j < cases[i].tries; j++) {
            slot = cases[i].slots[j];
            eye = &eye_calls[j];
            sight = &sight_calls[j];
            check_eye_call(eye, 2 * j + 1, slot, -500 + slot, -200 - slot, 30000 + slot);
            FCK_EQ(eye->prng_cursor, 19, "PRNG advanced before placement");
            FCK_EQ(sight->step, 2 * j + 2, "each sight test follows placement");
            FCK_CHECK(sight->to_is_eye_output, "exact candidate pointer fed to sight test");
            FCK_CHECK(sight->from_address != (u32)&camera.position &&
                      sight->from_address != eye->position_address, "sight test uses its own origin copy");
            FCK_CHECK(sight->from_address == sight_calls[0].from_address, "origin reused across attempts");
            FCK_EQ(sight->from.x, 314, "sight origin x");
            FCK_EQ(sight->from.y, -271, "sight origin y");
            FCK_EQ(sight->from.z, 159, "sight origin z");
            FCK_EQ(sight->to.x, 30000 + slot, "sight destination x");
            FCK_EQ(sight->to.y, -500 + slot, "sight destination y");
            FCK_EQ(sight->to.z, -200 - slot, "sight destination z");
        }
    }
}

int main(void)
{
    test_800188D0();
    test_80089BBC();
    test_80089A88();
    test_8005678C();
    test_80056870();
    return lk_done("leaf_hud_r4");
}
