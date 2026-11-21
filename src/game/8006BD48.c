// Categories: Gameplay > Values
#include "game/types.h"

struct Unk6BD48Obj {
    u8 unk00[0xAC];
    u8 *ctl;
};

struct Unk6BD48Tgt {
    u8 unk00[0x8];
    s16 unk8;
    u8 unk0A[0x1E];
    s16 unk28;
};

struct Unk6BD48State {
    u8 unk00[0x8];
    s32 unk8;
    u8 unk0C[0x8C];
    s32 unk98;
};

extern struct Unk6BD48State *gGameSession __asm__("D_8009FA94");
s32 gGetScaledWeaponReserveLimit(struct Unk6BD48Obj *obj, struct Unk6BD48Tgt *tgt, s32 flag) __asm__("func_8006BD48");


/**
 *  @brief Selects and adjusts a target value according to shared state.
 *
 *  A control byte selects one of two state fields, which determines whether
 *  the selected target value is scaled or replaced by a fixed value.
 *  @param obj Object whose control byte selects the state field.
 *  @param tgt Target containing the two signed source values.
 *  @param flag Low byte selects offset 0x28 when nonzero, otherwise offset 0x08.
 *  @return The selected value after the state-dependent adjustment.
 *  @par Globals
 *  D_8009FA94 points to the two selectable state fields.
 *  @par Control flow
 *  States 1 and 2 add one quarter and one half; state 3 sets 9999.
 *  State 0 and unlisted states leave the selected value unchanged.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8006BDFC, func_8006BE90.
 */
s32 gGetScaledWeaponReserveLimit(struct Unk6BD48Obj *obj, struct Unk6BD48Tgt *tgt, s32 flag)
{
    s32 val;
    s32 state;

    if ((flag & 0xFF) != 0) {
        val = tgt->unk28;
    } else {
        val = tgt->unk8;
    }

    if (*(s8 *)(obj->ctl + 0xC1) == 0) {
        state = gGameSession->unk8;
    } else {
        state = gGameSession->unk98;
    }

    switch (state) {
    case 0:
        break;
    case 1:
        val = val + (val >> 2);
        break;
    case 2:
        val = val + (val >> 1);
        break;
    case 3:
        val = 0x270F;
        break;
    }

    return val;
}
