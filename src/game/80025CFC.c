// Categories: Gameplay > Player > Retire | Platform > Sound > SE
#include "game/types.h"

extern s16 gTeamFlag[] __asm__("D_8009F65C");
extern void gPlayPositionalSoundEffect(s32 id, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80047284");

register s32 zero __asm__("$5");

void gSignalFirstTeamRetirement(u8 *arg0) __asm__("func_80025CFC");

/**
 * @brief Starts the team's retirement timer and conditionally plays its first retirement sound.
 *
 * @param arg0 Unit object; the data block at +0xAC supplies the team byte at +0xC1 and the
 * suppression byte at +0xDE.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the signed team index from unit data +0xC1, tests the corresponding halfword and writes
 * 1 if clear. If data byte +0xDE is zero, calls func_80047284 with effect 0x18, two zero
 * arguments and -1.
 *
 * @par Globals
 * Updates gTeamFlag and its second halfword gSecondTeamRetireTimer. The effect callee uses
 * gSoundIdTable.
 *
 * @par Callers
 * Direct calls from func_80058D78.
 *
 * @par Role and path
 * The retirement handler calls it from its non-gameplay path, seeding the per-team fade timer.
 *
 * @par Unknown
 * The effect is suppressed by +0xDE, whose other uses are not established here.
 */
void gSignalFirstTeamRetirement(u8 *arg0)
{
    s32 index;

    index = *(s8 *)(*(u8 **)(arg0 + 0xAC) + 0xC1);
    if (gTeamFlag[index] == 0) {
        gTeamFlag[index] = 1;
        if (*(u8 *)(*(u8 **)(arg0 + 0xAC) + 0xDE) == 0) {
            zero = 0;
            gPlayPositionalSoundEffect(0x18, zero, zero, -1);
        }
    }
}
