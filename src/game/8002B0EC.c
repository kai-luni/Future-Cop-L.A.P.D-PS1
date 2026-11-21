// Categories: Gameplay > Entity Update | Graphics > Animation
#include "game/types.h"

extern s32 func_80019668(void);
extern void gSelectAnimationClip(void *track, s32 index, s32 direction, s32 flags) __asm__("func_800448D8");
void gSetType26AnimationClip(u8 *arg0, u32 arg1) __asm__("func_8002B0EC");


/**
 * @brief Switches an actor's animation clip when the requested id differs from +0x15E.
 *
 * Updates the actor's active clip id (+0x15E) and attack-pose match flag (+0x162 vs +0x164), randomizes clip 0x3A across 0x3A..0x42 and 0x44, and applies the selection to the animation track at +0xC8.
 *
 * @param arg0 Actor block; reads/writes +0x15E, +0x164, +0x162, +0x74, and passes track +0xC8.
 * @param arg1 Requested clip index.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Guards the body with arg0[0x15E] != arg1. When arg1 == 0x3A, computes (func_80019668() % 10) + 0x3A, remaps 0x43 to 0x44, and zeroes *(s32 *)(arg0 + 0x74) if the result is 0x40..0x42 or 0x44 before selecting the clip.
 *
 * @par Calls
 * func_80019668 - provides a 31-bit pseudo-random value for clip 0x3A variant selection.
 * func_800448D8 - selects the final clip index forward with flag 1 on track arg0 + 0xC8.
 *
 * @par Side effects
 * Writes arg0[0x15E], arg0[0x162], optionally *(s32 *)(arg0 + 0x74), and mutates track arg0 + 0xC8 via func_800448D8.
 *
 * @par Callers
 * Confirmed: func_8002B1DC (0x8002B468), func_8002B588 (0x8002B684), func_8002B6C8 (0x8002B764), func_8002B784 (0x8002BA98, 0x8002BB20, 0x8002BF84), func_8002C174 (0x8002C2F0, 0x8002C314).
 */
void gSetType26AnimationClip(u8 *arg0, u32 arg1)
{
    u32 v;

    if (arg0[0x15E] != arg1) {
        arg0[0x15E] = arg1;
        arg0[0x162] = (arg1 == arg0[0x164]);
        if (arg1 == 0x3A) {
            v = (u32)func_80019668();
            arg1 = v % 10 + 0x3A;
            if (arg1 == 0x43) {
                arg1 = 0x44;
            }
            if (arg1 - 0x40 < 3 || arg1 == 0x44) {
                *(s32 *)(arg0 + 0x74) = 0;
            }
        }
        gSelectAnimationClip(arg0 + 0xC8, arg1, 1, 1);
    }
}
