// Categories: Resources > Save Data | Game Engine > Front-End Configuration
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");

extern void gComputeDualChecksum(u8 *arg0, s32 arg1, u8 *arg2, u8 *arg3) __asm__("func_8007F688");

/**
 * @brief Recomputes the dual 32-bit checksums for the front-end profile image in D_8009F92C.
 *
 * Confirmed: calls func_8007F688 on the 0x10-byte options block at D_8009F92C + 0x40C (storing checksums at +0x41C and +0x420), then loops i = 0..9 over the ten 0x70-byte profile records starting at +0x424, hashing each 0x68-byte payload into its trailing +0x68 and +0x6C checksum words.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F92C - base pointer of the front-end profile block (+0x40C options block and 10 records at +0x424 updated).
 *
 * @par Control flow
 * One initial checksum call followed by a 10-iteration for loop stepping offset by +0x70 from 0x424.
 *
 * @par Calls
 * func_8007F688 - computes additive and shift-XOR 32-bit checksum words over a byte buffer.
 *
 * @par Side effects
 * Overwrites checksum words at D_8009F92C + 0x41C/+0x420 and at +0x68/+0x6C of each of the 10 profile records.
 *
 * @par Callers
 * Confirmed: func_80080B70 (0x80081084 in asm/us/main.s, profile save image builder) / Likely: none / Unknown: none.
 */
void gUpdateProfileChecksums(void) __asm__("func_8007F6E0");

void gUpdateProfileChecksums(void)
{
    u32 i;
    s32 offset;

    gComputeDualChecksum(gSaveProfileImage + 0x40C, 0x10, gSaveProfileImage + 0x41C, gSaveProfileImage + 0x420);
    for (i = 0, offset = 0x424; i < 10; i++) {
        gComputeDualChecksum(gSaveProfileImage + offset, 0x68, gSaveProfileImage + offset + 0x68, gSaveProfileImage + offset + 0x6C);
        offset += 0x70;
    }
}
