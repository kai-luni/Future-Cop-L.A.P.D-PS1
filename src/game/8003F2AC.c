// Categories: Game Engine > Entity Lifecycle | Gameplay > Object Spawning | Gameplay > Entity Update
#include "game/types.h"

struct Unk3F2AC {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 unk0F;
    u8 unk10;
    u8 unk11[3];
    s16 unk14;
    u8 unk16[2];
    u16 unk18;
    u16 unk1A;
    u8 *unk1C;
    u8 *unk20;
    u8 unk24[0x48];
    void *unk6C;
};

extern s32 func_8003CAE4(struct Unk3F2AC *object, s32 arg, u8 *flag);
extern struct Unk3F2AC *gSpawnObjectFromTable(void *handle, struct Unk3F2AC *object,
                                      s32 flag, s32 arg3, s32 arg4) __asm__("func_8003E984");
extern void func_800498F0(void *object);

/**
 * @brief Advances one entity node's spawn/retire timers and unlinks any attached grid object on reactivation.
 *
 * Confirmed: while bit 0 of object->unk0F is clear, retires (unk0F |= 1) if bit 3 is set and (unk07 == -1 || unk04 <= 0); then calls func_8003CAE4(object, arg1, &flag). When flag != 0, spawns child via func_8003E984(object->unk1C, object, flag, 0, (object->unk10 >> 1) & 1), copying unk18 and forcing immortal/one-shot fields on child when result & 0xFF is non-zero. If unk0F & 1 is set while unk07 >= 0 or unk04 > 0, clears bit 0, sets bit 3 (0x8), and when unk20[4] == 0x5D with non-null unk6C calls func_800498F0(unk6C) and nulls unk6C. Category follows retire flags, func_8003E984 spawn, and func_800498F0 grid release.
 * Likely: per-tick lifecycle/respawn driver for D_800A1070 list nodes, returning bit 0 so func_8003F570 can unlink and send message 10. Unknown: exact meaning of unk0F bit 1 and bit 3.
 *
 * @param object List node (+0x04/+0x07/+0x14 timers, +0x0F state bits, +0x10 row bit, +0x18/+0x1C/+0x20/+0x6C links).
 * @param arg1 Frame delta passed to func_8003CAE4.
 * @return object->unk0F & 1 (1 if the node should be retired/unlinked by caller, 0 otherwise).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Two sequential state guards on object->unk0F bit 0, with nested child spawn and kind-0x5D grid cleanup.
 * @par Calls
 * func_8003CAE4 - updates node timers and sets spawn flag. func_8003E984 - spawns child entity. func_800498F0 - unlinks and frees grid object unk6C.
 * @par Side effects
 * Mutates object->unk0F, may spawn and configure a child entity, and may free/null object->unk6C.
 * @par Callers
 * Confirmed: func_8003F570 (0x8003F5CC, iterating over D_800A1070[0..6]) / Likely: none / Unknown: none.
 */
s32 func_8003F2AC(struct Unk3F2AC *object, s32 arg1)
{
    u8 flag;
    s32 result;
    struct Unk3F2AC *child;

    flag = 0;
    if (!(object->unk0F & 1)) {
        if ((object->unk0F & 8) &&
            (object->unk07 == -1 || object->unk04 <= 0)) {
            object->unk0F |= 1;
        }
        result = func_8003CAE4(object, arg1, &flag);
        if (flag != 0) {
            child = gSpawnObjectFromTable(object->unk1C, object, flag, 0,
                                  (object->unk10 >> 1) & 1);
            if (child != 0) {
                child->unk18 = object->unk18;
                if (result & 0xFF) {
                    child->unk07 = -1;
                    child->unk04 = -1;
                    child->unk14 = 0;
                    child->unk0F |= 2;
                    child->unk1A &= 0xF7FF;
                }
            }
        }
    }
    if (object->unk0F & 1) {
        if (object->unk07 >= 0 || object->unk04 > 0) {
            object->unk0F = (object->unk0F & ~1) | 8;
            if (object->unk20[4] == 0x5D) {
                if (object->unk6C != 0) {
                    func_800498F0(object->unk6C);
                    object->unk6C = 0;
                }
            }
        }
    }
    return object->unk0F & 1;
}
