// Categories: Gameplay > Entity Script | Game Engine > Object Setup
#include "game/types.h"

extern int gProjectileInit();

struct Unk2AD58 {
    u8 unk00[0x24];
    u32 flags;
    u8 unk28[0x50];
    s32 unk78;
    u8 unk7C[4];
    s32 unk80;
};

extern void func_8002A5F4(struct Unk2AD58 *object, void *data);
extern s32 func_8002ACB4(struct Unk2AD58 *object, s32 value);
extern s32 gRetireEntityBase(struct Unk2AD58 *object, s32 arg1, s32 arg2) __asm__("func_8002A810");
extern s32 func_8002A940(struct Unk2AD58 *object, void *data);
extern s32 func_8002AA1C(s32 message, struct Unk2AD58 *object, void *hit);
extern s32 func_8002AB8C(struct Unk2AD58 *object, void *data);
extern s32 gAtan2(s32 x, s32 y) __asm__("func_80095C30");
s32 gEntityBaseHandler(s32 message, struct Unk2AD58 *object, void *data) __asm__("func_8002AD58");


/**
 * @brief The shared base entity handler: default cases for spawn, draw,
 *        damage, queries and the heading angle.
 *
 * @param message Handler case number.
 * @param object Entity block; +0x24 flags, +0x78/+0x80 the heading vector.
 * @param data Case payload.
 * @return Case 1: 0xA0 (the base block size). Case 10/16: 1. Case 15: the
 *         12-bit heading angle. Default: -1. Other cases: the sub-helper
 *         results.
 *
 * @par Confirmed
 * Case 2 runs func_8002A5F4 (base spawn); case 3 draws via func_8002ACB4
 * (object, 1) unless flag bit 0x20 is set; case 4 -> func_8002A810(1, 1);
 * cases 5/6/7/8/13 return 0; case 9 -> func_8002A940; cases 11/14 ->
 * func_8002AA1C (the damage path); case 12 -> func_8002AB8C; case 15
 * returns func_80095C30(unk78, unk80) & 0xFFF (atan2 of the stored heading
 * vector).
 *
 * @par Likely role
 * The base class of the entity system: every specialised handler forwards
 * its unhandled cases here (50 callers), and the spawn queue's size query
 * (message 1) composes with the 0xA0 base size. Message 15 is the shared
 * "which way am I facing" query.
 *
 * @par Callers
 * func_8002C3F0, func_8002C584, func_8002D664, func_8002DAA4, func_8002DB9C,
 * func_8002DC94, func_8002DEA8, func_8002ED7C, func_8002EED8, func_8002F7C8,
 * func_8002FA20, func_8003015C, func_800302E8, func_80030960, func_80030AD4,
 * func_80030D3C, func_80030DE0, func_8003151C, func_800317BC, func_8003190C,
 * func_80032180, func_80032F3C, gProjectileInit, func_80036624, func_80036940,
 * func_80036E6C, func_80037064, func_80037708, func_800378BC, func_800379D4,
 * func_800382B4, func_80038754, func_80039298, func_800395A8, func_800398EC,
 * func_80039C58, func_80039E8C, func_8003A844, func_80041488, func_80041B6C,
 * func_80041C6C, func_80041E0C, func_80042330, func_80042504, func_800426A8,
 * func_800578E4, func_800579D4, func_8005908C, func_8005920C, func_80059ABC.
 *
 * @par Environment path
 * The shared base entity message handler: message 1 returns the 0xA0 base
 * block size, message 2 runs the base spawn func_8002A5F4, message 3 draws
 * through func_8002ACB4 unless flag bit 0x20 is set, message 4 returns
 * func_8002A810(object, 1, 1), messages 9, 11, 12 and 14 dispatch to the
 * query and damage helpers, and message 15 returns func_80095C30(unk78,
 * unk80) & 0xFFF, the atan2 of the stored heading vector. Messages 5 to 8,
 * 10, 13 and 16 return constants and every other message returns -1. Only
 * message 3 reaches the renderer, and it does so through func_8002ACB4, which
 * calls gSubmitModelInstance (func_80017508). The unit is therefore the
 * per-entity dispatch layer beside the environment pass rather than inside
 * it: the file reads no global, calls no GTE function and emits no geometry.
 */
s32 gEntityBaseHandler(s32 message, struct Unk2AD58 *object, void *data)
{
    switch (message) {
    case 1:
        return 0xA0;
    case 2:
        func_8002A5F4(object, data);
        return 0;
    case 3:
        if (object->flags & 0x20) {
            return 0;
        }
        return func_8002ACB4(object, 1);
    case 4:
        return gRetireEntityBase(object, 1, 1);
    case 5:
    case 6:
    case 7:
    case 8:
    case 13:
        return 0;
    case 9:
        return func_8002A940(object, data);
    case 11:
    case 14:
        return func_8002AA1C(message, object, data);
    case 12:
        return func_8002AB8C(object, data);
    case 10:
    case 16:
        return 1;
    case 15:
        return gAtan2(object->unk78, object->unk80) & 0xFFF;
    default:
        return -1;
    }
}
