// Categories: Game Engine > Audio
#include "game/types.h"

struct Actor {
    u8 pad0[0x4B];
    s8 unk4B;
    u8 pad4C[0x54 - 0x4C];
    u16 unk54;
};

struct Slot {
    struct Actor *actor;
    s16 timer;
    s8 sound_index;
    s8 pad;
};

struct SoundManager {
    u32 mask;
    struct Slot slots[32];
};

extern struct SoundManager *gMissionTable __asm__("D_8009F930");

extern s32 func_80045768(s32 arg0, s32 index);
extern void func_800457B8(s32 arg0, s32 index);

/**
 *  @brief Updates sound-slot timers and releases slots whose sound check fails.
 *
 *  The manager mask identifies slots to skip for sound checks. Timers are
 *  decremented for every slot visited, including slots already marked in the mask.
 *  @param elapsed Amount subtracted from each visited slot's timer.
 *  @par Globals
 *  D_8009F930 points to the sound manager, its mask, and its 32 slots.
 *  @par Control flow
 *  If the mask is not full, slots are visited until all remaining mask bits
 *  are accounted for; unmasked slots are checked when their timer is negative.
 *  @par Calls
 *  func_80045768 checks the actor and sound index; func_800457B8 is called
 *  when that check returns zero.
 *  @par Side effects
 *  Updates timers and the mask; a failed check sets the actor's unk4B to -1.
 *  A successful check resets the slot timer to 300.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8008352C(s32 elapsed)
{
    u32 bit = 1;
    struct SoundManager *mgr = gMissionTable;
    struct Slot *slot = mgr->slots;
    u32 mask = mgr->mask;
    u32 pending = mask;

    if (mask != (u32)-1) {
        do {
            slot->timer -= elapsed;
            if (!(pending & bit)) {
                if (slot->timer < 0) {
                    struct Actor *actor = slot->actor;
                    if ((u8)func_80045768(actor->unk54 | 0x40000000, slot->sound_index) != 0) {
                        slot->timer = 300;
                    } else {
                        mask |= bit;
                        actor->unk4B = -1;
                        func_800457B8(actor->unk54 | 0x40000000, slot->sound_index);
                    }
                }
            }
            pending |= bit;
            bit <<= 1;
            slot++;
        } while (pending != (u32)-1);
    }

    mgr->mask = mask;
}
