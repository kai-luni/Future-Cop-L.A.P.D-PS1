// Categories: Gameplay > Movement | Utilities > Random
#include "game/types.h"

struct Unk381D4 {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[0x86];
    u16 unkA4;
    u8 padA6[0x4];
    s16 unkAA;
    s32 unkAC;
    s32 unkB0;
    u16 unkB4;
    u8 padB6[0x62];
    u32 unk118;
    u8 pad11C[0x14];
    s16 unk130;
    u8 pad132[0x4];
    u16 unk136;
};

extern s32 func_80019668(void);
void gStepIdleActorTurn(struct Unk381D4 *arg) __asm__("func_800381D4");


/**
 * @brief Picks a random turn rate for an entity leaving its idle state.
 *
 * The two-bit field at the top of arg->unk118 is the entity state. In state 0
 * the function draws a random nibble from func_80019668 and only acts while it
 * stays below 0xC, so the transition is delayed by roughly a quarter of the
 * frames. On transition it sets bit 30, clears the timer at 0xAC, copies the
 * angle limits 0x136 and 0x130 into 0xB4 and 0xB0, sets flag 0x100 and derives
 * the turn rate (random 1..4) with its sign chosen from the side the target
 * angle lies on. Values above 0x800 are treated as the negative half of the
 * 12-bit angle circle. State 1 then falls through and clears the frame timer.
 *
 * @param arg Entity block; angle at 0x130/0x136, rate at 0xAA, state at 0x118.
 *
 * @par Globals
 * None; all state lives in the entity block.
 *
 * @par Control flow
 * Switch on the state bits with an intentional fall-through from state 0 into
 * state 1; the random guard breaks out without changing state.
 *
 * @par Calls
 * func_80019668 - random source (called three times per transition).
 *
 * @par Side effects
 * Writes 0xAA, 0xAC, 0xB0, 0xB4, 0x118 and 0x1C of the entity.
 *
 * @par Callers
 * Confirmed: func_800382B4.
 */
void gStepIdleActorTurn(struct Unk381D4 *arg)
{
    s32 v;

    switch (arg->unk118 >> 30) {
    case 0:
        if (((u32)func_80019668() & 0xF) < 0xC) {
            arg->unk118 |= 0x40000000;
            arg->unkAC = 0;
            arg->unkB4 = arg->unk136;
            arg->unkA4 |= 0x100;
            arg->unkB0 = arg->unk130;
            v = (func_80019668() & 3) + 1;
            if ((s16)arg->unk136 != 0) {
                if ((s16)arg->unk136 >= 0x801) {
                    v = -v;
                }
            } else if (v & 1) {
                v = -v;
            } else {
                arg->unkAA = v;
            }
            arg->unkAA = v;
            arg->unk118 &= -6;
        } else {
            break;
        }
    case 1:
        arg->unk1C = 0;
        break;
    }
}
