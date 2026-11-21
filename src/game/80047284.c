// Categories: Platform > Sound > SE
#include "game/types.h"

extern u8 gSoundIdTable[] __asm__("D_800A1880");

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern s32 gPlaySpatialSoundBankSlot(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4) __asm__("func_80046F9C");
void gPlayPositionalSoundEffect(s32 id, s32 arg1, s32 arg2) __asm__("func_80047284");


/**
 * @brief Plays a one-shot sound effect with explicit position/parameter
 *        arguments.
 *
 * @param id Sound id resolved against D_800A1880 (limit 0x45).
 * @param arg1, arg2 Forwarded to the voice start (position/pan parameters).
 * @return Nothing.
 *
 * @par Confirmed
 * func_80045494 resolves the slot; 0xFF (not found) aborts, otherwise
 * func_80046F9C(arg1, arg2, slot, 0, 1) starts the voice. Same table as
 * func_80046C84 but with caller-supplied parameters and mode 1.
 *
 * @par Likely role
 * The parameterised SE trigger of the combat and effect handlers: the
 * arguments likely carry the 3D position or volume/pan of the source.
 *
 * @par Callers
 * func_80025CFC, func_8002F30C, func_8003E984, func_80041880, func_8005F6E8.
 */
void gPlayPositionalSoundEffect(s32 id, s32 arg1, s32 arg2)
{
    s32 slot;

    slot = func_80045494(id, gSoundIdTable, 0x45);
    if (slot != 0xFF) {
        gPlaySpatialSoundBankSlot(arg1, arg2, slot, 0, 1);
    }
}
