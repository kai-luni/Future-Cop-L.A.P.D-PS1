// Categories: Game Engine > Audio
#include "game/types.h"

struct Unk450A4Slot {
    u8 unk00[0x50];
    s32 unk50;
    u8 unk54[4];
    s32 unk58;
};

extern u8 D_8009F738;

extern struct Unk450A4Slot *func_80045164(s32 id);
extern s32 func_800458E8(s32 arg0, s32 arg1, s8 *mix, s32 arg3);
extern void func_800460D4(s8 *mix);

/**
 *  @brief Computes a mix for a sound slot and boosts its volume.
 *
 *  A found slot supplies two values to the mix calculation. A failed
 *  calculation produces a silent, centered mix instead.
 *  @param id Identifier used to find the slot.
 *  @param mix Output mix buffer, including volume and pan in its first two bytes.
 *  @return Nonzero if the slot exists, otherwise zero.
 *  @par Globals
 *  D_8009F738 selects the upper volume limit of 63 rather than 127.
 *  @par Control flow
 *  A missing slot leaves the buffer untouched; a successful mix is boosted and capped.
 *  @par Calls
 *  func_80045164 finds the slot; func_800458E8 calculates the mix.
 *  func_800460D4 further processes a successfully calculated mix.
 *  @par Side effects
 *  Writes the mix buffer when a slot is found.
 *  @par Callers
 *  Confirmed: func_80023D78.
 */
s32 func_800450A4(s32 id, s8 *mix)
{
    struct Unk450A4Slot *slot;
    s32 volume;

    slot = func_80045164(id);
    if (slot != 0) {
        if (func_800458E8(slot->unk50, slot->unk58, mix, 0) != 0) {
            func_800460D4(mix);
            volume = mix[0];
            volume += (mix[0] >> 1) + (mix[0] >> 2);
            if (D_8009F738 != 0) {
                if (volume >= 0x40) {
                    volume = 0x3F;
                }
            } else if (volume >= 0x80) {
                volume = 0x7F;
            }
            mix[0] = volume;
        } else {
            mix[0] = 0;
            mix[1] = 0x40;
        }
    }
    return slot != 0;
}
