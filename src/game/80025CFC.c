// Categories: Gameplay > Player > Retire | Platform > Sound > SE
#include "game/types.h"

extern s16 gTeamFlag[] __asm__("D_8009F65C");
extern void func_80047284(s32 id, s32 arg1, s32 arg2, s32 arg3);

register s32 zero __asm__("$5");

/**
 * @brief Marks the first retirement of a team and plays the matching one-shot
 *        sound effect.
 *
 * Called from the non-gameplay branch of the unit retire handler func_80058D78,
 * once per team. The team byte comes from the unit's data block (+0xC1, the same
 * side byte the camera code reads), the per-team flag in D_8009F65C guards the
 * one-shot, and bit-free byte +0xDE of the data block suppresses the sound.
 *
 * @param arg0 Unit object; the data block at +0xAC supplies the team byte at
 *        +0xC1 and the suppression byte at +0xDE.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F65C - two 16-bit per-team "already reported" flags (index 0 and 1 are
 * the words at 0x8009F65C and 0x8009F65E, both cleared by gInitializePanelInputActions at
 * module load).
 *
 * @par Control flow
 * Flag test, flag set, second byte test, one call. The third argument is a
 * register-pinned zero, which is how the retail code keeps $5 clear.
 *
 * @par Calls
 * func_80047284 - plays SE id 0x18 with zero pan/parameter through the slot table
 * D_800A1880.
 *
 * @par Side effects
 * Writes D_8009F65C[team] and starts one sound effect.
 *
 * @par Callers
 * Confirmed: func_80058D78 (message 4 path, only while screen state is not 5).
 */
void func_80025CFC(u8 *arg0)
{
    s32 index;

    index = *(s8 *)(*(u8 **)(arg0 + 0xAC) + 0xC1);
    if (gTeamFlag[index] == 0) {
        gTeamFlag[index] = 1;
        if (*(u8 *)(*(u8 **)(arg0 + 0xAC) + 0xDE) == 0) {
            zero = 0;
            func_80047284(0x18, zero, zero, -1);
        }
    }
}
