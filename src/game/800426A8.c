// Categories: Game Engine > Entity Dispatch
#include "game/types.h"

struct Entity {
    u8 unk00[0x0C];
    u8 unk0C;
    u8 unk0D[0x94];
    u8 unkA1;
};

extern s32 gEntityBaseHandler(s32 message, struct Entity *self, void *data) __asm__("func_8002AD58");
extern void gSetPlayerInfoTarget(struct Entity *self) __asm__("func_800423F0");
extern s32 gClearPlayerInfoTarget(s32 message, struct Entity *self) __asm__("func_80042470");
extern s32 gInitType35Entity(s32 message, struct Entity *self, void *data) __asm__("func_80042504");
extern void gStepType35Commands(struct Entity *self) __asm__("func_800425BC");
s32 gHandleType35Msg(s32 message, struct Entity *self, void *data) __asm__("func_800426A8");


/**
 *  @brief Dispatches entity messages to initialization, update, or default handling.
 *
 *  Message 1 reports the entity size, while selected messages are ignored.
 *  Message 6 conditionally processes and clears the active byte and checks unk0C.
 *  @param message Message selector.
 *  @param self Entity receiving the message.
 *  @param data Message-specific data passed to applicable handlers.
 *  @return Size, handler result, or zero according to the message.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Switches on message; message 6 has two independent conditional calls.
 *  @par Calls
 *  func_80042504 initializes; func_80042470 handles message 5.
 *  func_800423F0 and func_800425BC handle message 6; func_8002AD58 handles defaults.
 *  @par Side effects
 *  Clears self->unkA1 when set and may modify state through dispatched calls.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 gHandleType35Msg(s32 message, struct Entity *self, void *data)
{
    switch (message) {
    case 1:
        return 0xD0;
    case 2:
        return gInitType35Entity(message, self, data);
    case 6:
        if (self->unkA1 != 0) {
            gSetPlayerInfoTarget(self);
            self->unkA1 = 0;
        }
        if (self->unk0C != 0) {
            gStepType35Commands(self);
        }
        return 0;
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 14:
        return 0;
    case 5:
        return gClearPlayerInfoTarget(message, self);
    case 12:
    default:
        return gEntityBaseHandler(message, self, data);
    }
}
