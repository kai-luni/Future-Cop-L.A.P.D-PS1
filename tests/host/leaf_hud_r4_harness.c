/* HUD/geometry leaf suite (round 4): unchanged game C under qemu-mipsel.
 * Expectations are derived from the named units' source, not captured output.
 */
/* @link 8001846C */
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

int main(void)
{
    test_800188D0();
    test_80089BBC();
    return lk_done("leaf_hud_r4");
}
