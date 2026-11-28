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

int main(void)
{
    test_800188D0();
    return lk_done("leaf_hud_r4");
}
