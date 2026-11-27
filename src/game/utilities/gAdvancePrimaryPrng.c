// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 gPrimaryPrngCursor __asm__("D_8009F5A8");
extern s32 gStateTable[] __asm__("D_800A0480");

/* additive Lagged-Fibonacci-Generator, 31 bit random number */

s32 gAdvancePrimaryPrng(void) __asm__("func_80019668");

/**
 * @brief Advances the primary lagged-additive random sequence and returns its
 * updated word.
 * @return The updated 32-bit state word.
 *
 * @par Confirmed mechanics
 * Decrements the cursor, wrapping a negative result to 19. Forms the other
 * index at the old cursor plus 2, falling back to the old cursor minus 18 for
 * the upper wrap. Adds the two table words with ADDU and stores the sum into
 * the decremented slot.
 *
 * @par Globals
 * Reads and writes gPrimaryPrngCursor and gStateTable.
 *
 * @par Callers
 * Direct calls in the retail listing are from gStepAnimations, func_8002AF94,
 * func_8002B0EC, func_8002C174, func_8002E2BC, gLoadMapZone, func_80031DE8,
 * func_800320E4, func_80032180, gUpdateHoverEnemy, func_80032F3C,
 * func_800331CC, func_800336DC, func_8003526C, func_8003532C, func_80035698,
 * gProjectileInit, func_80036BD4, func_800381D4, gSpawnModelDebris,
 * func_8003B4E0, gBuildNodePlacement, func_80042AF4, func_80044B34,
 * func_80049420, func_80049C38, gLaunchDebrisObject, func_8004B310,
 * gSpawnMeshFractureDebris, func_8004E80C, func_8004F330,
 * gFindClearCameraPreset, func_80056924, gUpdateChaseCamera, func_80058C3C,
 * gEmitObjectBeamSegment, gSpawnLobeArc, func_8005D8F4, func_8005EE04,
 * func_8005F018, func_8005F6E8, func_80060238, func_80060E74, func_80063568,
 * func_80069B58, func_80069D68, func_8006A524, gBeamPartUpdate, func_8007043C,
 * gComputeEffectEndPoint, func_80070E48, func_800718B8, func_800752E0,
 * gInitEmitterParticle, gCreateEmitterObject, gSpawnSceneNode.
 *
 * @par Role and path
 * gSeedPrng initializes this state separately from the second stream. Model
 * animation and many entity routines draw from it.
 *
 * @par Unknown
 * The retail addition supplies a full word without a 31-bit mask. The reason
 * for using two streams remains open.
 */
s32 gAdvancePrimaryPrng(void)
{
    s32 i;
    s32 j;

    i = gPrimaryPrngCursor - 1;
    j = gPrimaryPrngCursor + 2;
    if (i < 0) {
        i = 0x13;
    } else if (j >= 0x14) {
        j = gPrimaryPrngCursor - 0x12;
    }
    gPrimaryPrngCursor = i;
    return gStateTable[i] += gStateTable[j];
}
