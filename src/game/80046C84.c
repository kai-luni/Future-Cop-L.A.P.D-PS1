// Categories: Platform > Sound
#include "game/types.h"

extern u8 gSoundIdTable[] __asm__("D_800A1880");

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern s32 func_80046F9C(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4);

/**
 * @brief Plays a sound effect by id: looks the id up in the SE table and
 *        starts it on a free voice.
 *
 * @param id Effect id (e.g. 0x46/0x47 are the menu cursor sounds).
 * @param arg1 Playback parameter forwarded to the voice starter (pitch or
 *        priority, unconfirmed).
 * @return The voice handle, or 0 when the id is not in the table.
 *
 * @par Confirmed
 * func_80045494(id, D_800A1880, 0x45) searches the 0x45-entry effect table;
 * a slot of 0xFF means "not found" and returns 0. Otherwise
 * func_80046F9C(0, 0, slot, arg1, 1) starts the effect and its result is
 * returned.
 *
 * @par Likely role
 * The universal SE trigger of the game: every menu, HUD and entity handler
 * calls it for cursor moves, confirms, impacts and spawns.
 *
 * @par Unknown
 * The meaning of arg1 and of the trailing 1 in the voice starter.
 *
 * @par Callers
 * D_8007D27C (data reference), func_800260B8, func_800267C8, func_80027510,
 * func_8003E984, func_80047088, func_80049260, func_80055988, func_800588D4,
 * func_80058D78, func_8005A154, func_8006ACB0, func_8006BA50, func_8007A3AC,
 * func_8007A4CC, func_8007A7A0, func_8007AB0C, func_8007ABBC, func_8007B854,
 * func_8007C80C, func_8007C940, func_8007E1A4, func_8007F1F8, func_8007F618,
 * func_8008125C.
 */
s32 func_80046C84(s32 id, s32 arg1)
{
    s32 handle;
    s32 slot;

    handle = 0;
    slot = func_80045494(id, gSoundIdTable, 0x45);
    if (slot != 0xFF) {
        handle = func_80046F9C(handle, handle, slot, arg1, 1);
    }
    return handle;
}
