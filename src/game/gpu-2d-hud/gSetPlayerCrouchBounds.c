// Categories: Graphics > HUD | Gameplay > Player
#include "../../include/game/types.h"

struct DisplayRecord {
    u8 pad_00[2];
    s16 right;
    u8 pad_04[4];
    s16 left;
};

struct DisplayOwner {
    u8 pad_00[0x2DC];
    struct DisplayRecord *record;
};

struct DisplayState {
    u8 pad_00[0xAC];
    struct DisplayOwner *owner;
};
void gSetPlayerCrouchBounds(struct DisplayState *state, int compact) __asm__("func_80051AC8");


/**
 * @brief Sets the left/right display edges of the player's screen record.
 *
 * The record is reached through state->owner->record + 8 and describes the
 * visible horizontal band. Compact mode (second argument non-zero) narrows the
 * band around 0xCC instead of 0x126; both edges are placed 0x19 units around
 * the centre.
 * @param state HUD state whose owner holds the display record.
 * @param compact Non-zero selects the narrow 0xCC centre.
 * @par Globals
 * None.
 * @par Control flow
 * One branch on the compact flag.
 * @par Calls
 * None.
 * @par Side effects
 * Writes the right and left halfwords of the record.
 * @par Callers
 * Unknown: no direct edge; called from the split-screen/HUD layout path.
 */
void gSetPlayerCrouchBounds(struct DisplayState *state, int compact)
{
    struct DisplayRecord *record = (struct DisplayRecord *)
        ((u8 *)state->owner->record + 8);
    s32 center = 0x126;

    if (compact != 0) {
        center = 0xCC;
    }
    record->right = center + 0x19;
    record->left = center - 0x19;
}
