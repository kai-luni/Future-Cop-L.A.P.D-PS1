// Categories: AI > Target Selection | Gameplay > Entity Update
#include "game/types.h"

typedef struct Target {
    u8 pad00[0x08];
    s32 unk08;
    u8 pad0C[0x14 - 0x0C];
    u16 unk14;
    u8 pad16[2];
    u8 unk18;
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    u8 pad1E[0xB8 - 0x1E];
    struct Target *unkB8;
    s32 unkBC;
    s16 unkC0;
    u8 padC2[0x118 - 0xC2];
    u32 unk118;
    u8 pad11C[2];
    s16 unk11E;
    u8 pad120[0x138 - 0x120];
    struct Target *unk138;
    s32 unk13C;
    u8 pad140[0x15F - 0x140];
    u8 unk15F;
    u8 unk160;
} Target;

extern void gDispatchType26Command(Target *arg0, s32 arg1, Target *arg2) __asm__("func_8002B1DC");
extern void func_8002B1AC(Target *arg0);
void gConsiderType26Target(Target *s0, Target *s1) __asm__("func_8002BFB4");


/**
 * @brief Evaluates and assigns a candidate target for a class-26 actor.
 *
 * Rejects invalid, dead, same-team (unk18), or unready (unkC0 == 0) pairs and state-4 actors with a target. When !(unk160 & 2), unk1C < 5, and state != 4, switches to mode 0xA / state 2 on unk138 unless state 2 already holds a target with >= priority (unk19 & 0xF); otherwise acquires via state 0 or upgrades unkB8 in state 1.
 *
 * @param s0 Actor block (+0x18 team, +0x1C/+0xC0/+0x15F/+0x160 state, +0xB8/+0x138 target slots).
 * @param s1 Candidate target entity (+0x08 id, +0x14 word id, +0x18 team, +0x19 priority nibble).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Seven early-return guards followed by a two-way branch: priority override (state 2, unk138) vs standard acquisition/upgrade (state 0/1, unkB8).
 *
 * @par Calls
 * func_8002B1DC - issues target-acquire command 1 to the actor state machine; func_8002B1AC - syncs +0x10C/+0x10E counters from +0x158.
 *
 * @par Side effects
 * Writes actor state +0x15F, +0x118 mode nibble, +0x11E, target fields +0xB8/+0xBC or +0x138/+0x13C, and +0x10C/+0x10E.
 *
 * @par Callers
 * Confirmed: func_8002C3B0 (0x8002C3D0), func_8002C3F0 (0x8002C4A4, 0x8002C4DC) / Likely: none / Unknown: none.
 */
void gConsiderType26Target(Target *s0, Target *s1)
{
    s16 unk1C;

    if (s0 == 0) {
        return;
    }

    unk1C = s0->unk1C;
    if (unk1C < 0) {
        return;
    }

    if (s1 == 0) {
        return;
    }

    if (s1->unk08 < 0) {
        return;
    }

    if (s1->unk18 == s0->unk18) {
        return;
    }

    if (s0->unkC0 == 0) {
        return;
    }

    if (s0->unk15F == 4 && s0->unkB8 != 0) {
        return;
    }

    if (!(s0->unk160 & 2) && unk1C < 5 && s0->unk15F != 4) {
        if (s0->unk15F == 2) {
            if (s0->unk138 != 0) {
                if ((s0->unk138->unk19 & 0xF) >= (s1->unk19 & 0xF)) {
                    return;
                }
            }
        }

        gDispatchType26Command(s0, 1, s1);
        s0->unk118 = (s0->unk118 & 0xF0FFFFFF) | 0x0A000000;
        s0->unk15F = 2;
        s0->unk11E = s1->unk14 & 0x7FFF;
        s0->unk138 = s1;
        s0->unk13C = s1->unk08;
        func_8002B1AC(s0);
    } else {
        if (s0->unk15F == 0) {
            gDispatchType26Command(s0, 1, s1);
        } else if (s0->unk15F == 1) {
            if (s0->unkB8 != 0 && (s0->unkB8->unk19 & 0xF) < (s1->unk19 & 0xF)) {
                s0->unkB8 = s1;
                s0->unkBC = s1->unk08;
            }
        }
    }
}
