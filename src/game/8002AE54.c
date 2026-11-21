// Categories: Game Engine > Dispatch
#include "game/types.h"

extern u8 gCameraOverrideHandlerAddress[] __asm__("D_80039C58");
extern u8 gStrategicDroneEntityHandler[] __asm__("D_800357C0");
extern u8 gHandleElevatorEntity[] __asm__("D_80041B6C");
extern u8 gType11EntityHandler[] __asm__("D_8002DEA8");
extern u8 gPlacedPropHandlerAddress[] __asm__("D_80041C6C");
extern u8 gHandleType13Entity[] __asm__("D_8002DB9C");
extern u8 gRotatingPropEntityHandler[] __asm__("D_8003015C");
extern u8 gResupplyEntityHandlerAddress[] __asm__("D_80030960");
extern u8 gHandleMoverEntity[] __asm__("D_8002F7C8");
extern u8 gHandleType27Entity[] __asm__("D_8002ED7C");
extern u8 gHandleType26Entity[] __asm__("D_8002C3F0");
extern u8 gHandleType29Entity[] __asm__("D_80030D3C");
extern u8 gHandleForceFieldEntity[] __asm__("D_8003A844");
extern u8 gPortraitScreenEntityHandler[] __asm__("D_80042330");
extern u8 gType35EntityHandlerAddress[] __asm__("D_800426A8");
extern u8 gHandleTriggerAttachmentEntity[] __asm__("D_8003151C");

extern void gHandleTumblingFlyer(void) __asm__("func_80032F3C");
extern void gHandlePlayerEntityMsg(void) __asm__("func_8005908C");
extern void gHandleType5Entity(void) __asm__("func_800382B4");
extern void gHandleLinkedPropEntity(void) __asm__("func_8002D664");
extern void gHandleMountedTurretCarrierMessages(void) __asm__("func_800395A8");
extern void gHandleType28Entity(void) __asm__("func_800398EC");
s32 gGetEntityHandlerForType(u32 type) __asm__("func_8002AE54");


/**
 *  @brief Maps a type number to a stored address.
 *
 *  The switch returns addresses of declared data symbols or functions as signed
 *  integers, allowing the caller to select an address by type.
 *  @param type Numeric selector for the address mapping.
 *  @return The selected address cast to s32, or zero for an unrecognized type.
 *  @par Globals
 *  Returns addresses of D_80039C58, D_800357C0, D_80041B6C, D_8002DEA8,
 *  D_80041C6C, D_8002DB9C, D_8003015C, D_80030960, D_8002F7C8,
 *  D_8002ED7C, D_8002C3F0, D_80030D3C, D_8003A844, D_80042330,
 *  D_800426A8, or D_8003151C for their corresponding selectors.
 *  @par Control flow
 *  A switch dispatches the selector; several cases share an address.
 *  @par Calls
 *  None; function addresses are returned without invoking them.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80060E74.
 */
s32 gGetEntityHandlerForType(u32 type)
{
    switch (type) {
    case 80:
        return (s32)gCameraOverrideHandlerAddress;
    case 9:
        return (s32)gHandleTumblingFlyer;
    case 37:
        return (s32)gStrategicDroneEntityHandler;
    case 1:
        return (s32)gHandlePlayerEntityMsg;
    case 5:
        return (s32)gHandleType5Entity;
    case 6:
    case 8:
    case 30:
    case 33:
    case 36:
        return (s32)gHandleLinkedPropEntity;
    case 10:
        return (s32)gHandleElevatorEntity;
    case 11:
        return (s32)gType11EntityHandler;
    case 12:
        return (s32)gPlacedPropHandlerAddress;
    case 13:
        return (s32)gHandleType13Entity;
    case 16:
        return (s32)gRotatingPropEntityHandler;
    case 32:
        return (s32)gResupplyEntityHandlerAddress;
    case 20:
        return (s32)gHandleMountedTurretCarrierMessages;
    case 25:
        return (s32)gHandleMoverEntity;
    case 27:
        return (s32)gHandleType27Entity;
    case 26:
        return (s32)gHandleType26Entity;
    case 28:
        return (s32)gHandleType28Entity;
    case 29:
        return (s32)gHandleType29Entity;
    case 31:
        return (s32)gHandleForceFieldEntity;
    case 34:
        return (s32)gPortraitScreenEntityHandler;
    case 35:
        return (s32)gType35EntityHandlerAddress;
    case 14:
    case 38:
        return (s32)gHandleTriggerAttachmentEntity;
    default:
        return 0;
    }
}
