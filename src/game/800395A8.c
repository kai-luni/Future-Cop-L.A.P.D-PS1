// Categories: Gameplay > Entity Update | Game Engine > Message Dispatch | Gameplay > Damage
#include "game/types.h"

struct Unk395A8Child {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
};

struct Unk395A8Self {
    u8 pad00[0x1C];
    u16 unk1C;
    u8 pad1E[6];
    u32 unk24;
    u8 pad28[0x130];
    struct Unk395A8Child *unk158;
};

extern void func_80039504(s32 arg0, void *arg1, void *arg2, s32 arg3);
extern s32 gEntityBaseHandler(s32 message, void *object, void *data, s32 arg3) __asm__("func_8002AD58");
extern void func_800382B4(s32 arg0, void *arg1, void *arg2, s32 arg3);
extern void gUpdateMountedTurret(void *arg0, void *arg1) __asm__("func_80038A00");
extern void func_8003BA50(struct Unk395A8Child *object, s32 value);

/**
 * @brief Message handler for entity type 20 (0x14), wrapping class-5 behavior and propagating updates/damage to its +0x158 child.
 *
 * Confirmed: case 1 returns 0x15C (size); case 2 calls func_80039504 and returns 0; case 3 skips on bit 0x20 of unk24, else calls func_8002AD58 on self and unk158; case 6 calls func_800382B4 then copies unk1C and calls func_80038A00 on unk158; cases 9/11/14 call func_800382B4, copy bit 0x02000000 of unk24 and unk1C onto unk158, and call func_8003BA50 when unk158->unk20 != 0 && !(unk158->unk24 & 0x40000). Case 5 sets unk158->unk1C = -1 and falls through with 4/7/8/10/12/13/default to func_800382B4 (returning its $v0). Category follows type-20 dispatch, 0x15C size, +0x158 child sync, and quartile damage check.
 * Likely: mounted/composite class-5 entity (e.g., vehicle + turret child at +0x158).
 *
 * @param message Message selector (1=size, 2=init, 3=render, 5=retire, 6=tick, 9/11/14=damage/hit, others delegated).
 * @param object Entity block (+0x1C health, +0x24 flags, +0x158 child pointer).
 * @param arg2 Message payload forwarded to child/base handlers.
 * @param arg3 Extra context word forwarded to base handlers.
 * @return 0x15C for msg 1; 0 for msgs 2/3/6/9/11/14; func_800382B4 return value for msgs 4/5/7/8/10/12/13/default.
 * @par Globals
 * None.
 * @par Control flow
 * Switch on message with case 5 falling through into the shared func_800382B4 tail.
 * @par Calls
 * func_80039504 (spawn + child setup), func_8002AD58 (base render/msg 3), func_800382B4 (class-5 handler), func_80038A00 (child orientation/tick), func_8003BA50 (quartile flag).
 * @par Side effects
 * Updates self and child (+0x158) health (+0x1C), flags (+0x24), and spawn/tick state via sub-handlers.
 * @par Callers
 * Confirmed: func_80039298 (0x800393E0, msgs 11/14 for child type 0x14); address taken in func_8002AE54 (0x8002AF18, type 20) / Likely: indirect callers via entity->update / Unknown: indirect call sites.
 */
s32 func_800395A8(s32 message, struct Unk395A8Self *object, void *arg2, s32 arg3)
{
    register s32 msg __asm__("$17") = message;
    register struct Unk395A8Self *self __asm__("$16") = object;
    register void *data __asm__("$18") = arg2;
    register s32 extra __asm__("$19") = arg3;
    s32 previous;

    switch (msg) {
    case 1:
        return 0x15C;
    case 2:
        func_80039504(msg, self, data, extra);
        return 0;
    case 3:
        if (self->unk24 & 0x20) {
            return 0;
        }
        gEntityBaseHandler(msg, self, data, extra);
        if (self->unk158 != 0) {
            gEntityBaseHandler(msg, self->unk158, data, extra);
        }
        return 0;
    case 6:
        func_800382B4(msg, self, data, extra);
        if (self->unk158 != 0) {
            self->unk158->unk1C = self->unk1C;
            gUpdateMountedTurret(self->unk158, data);
        }
        return 0;
    case 9:
    case 11:
    case 14:
        func_800382B4(msg, self, data, extra);
        if (self->unk158 != 0) {
            if (self->unk24 & 0x02000000) {
                self->unk158->unk24 |= 0x02000000;
            } else {
                self->unk158->unk24 &= 0xFDFFFFFF;
            }
            if (self->unk158->unk20 != 0 && (self->unk158->unk24 & 0x40000) == 0) {
                previous = self->unk158->unk1C;
                self->unk158->unk1C = self->unk1C;
                func_8003BA50(self->unk158, previous);
            } else {
                self->unk158->unk1C = self->unk1C;
            }
        }
        return 0;
    case 5:
        if (self->unk158 != 0) {
            self->unk158->unk1C = -1;
        }
    case 4:
    case 7:
    case 8:
    case 10:
    case 12:
    case 13:
    default:
        func_800382B4(msg, self, data, extra);
        break;
    }
}
