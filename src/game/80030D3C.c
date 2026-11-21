// Categories: Gameplay > Entity Script | Game Engine > Object Setup
#include "game/types.h"

struct Entity {
    u8 pad[0xAD];
    u8 unkAD;
    s16 unkAE;
};

extern s32 gInitEntityGroundPair(void *, void *, s32) __asm__("func_80030AD4");
extern u8 gTestEntityFieldMatch(u8, s16, void *, u8) __asm__("func_8002F8B8");
extern s32 gWarpEntityToSelf(void *, void *, s32) __asm__("func_80030C60");
extern s32 gEntityBaseHandler(s32, struct Entity *, void *, s32) __asm__("func_8002AD58");
s32 gHandleGroundPairEntity(s32 message, struct Entity *self, void *data, s32 arg3) __asm__("func_80030D3C");


/**
 * @brief Entity message handler for object type 29 (size 0xB0).
 *
 * Dispatches lifecycle, query, and draw/callback messages for type-29 entities,
 * handling messages 1, 2, 10, 11 and no-op cases while forwarding defaults to func_8002AD58.
 *
 * @param message Message opcode (1=size, 2=spawn, 10=field test, 11=part transform, 3..9/12/13=no-op).
 * @param self Entity block (+0xAD selector, +0xAE match value).
 * @param data Payload pointer (spawn record, target entity, etc.).
 * @param arg3 Extra context argument forwarded to helpers.
 * @return 0xB0 for message 1, 0 for messages 3..9/12..13, or the dispatched helper's return value.
 *
 * @par Globals
 * None directly.
 * @par Control flow
 * Single switch on message: handles 1, 2, 10, 11, returns 0 for 3..9/12..13, defaults to func_8002AD58.
 * @par Calls
 * func_80030AD4 (msg 2 spawn), func_8002F8B8 (msg 10 predicate), func_80030C60 (msg 11 vertex/callback), func_8002AD58 (default).
 * @par Side effects
 * Delegated to the called handlers (entity placement/flags or callback invocation).
 * @par Callers
 * Confirmed: none direct; address D_80030D3C returned by func_8002AE54 (0x8002AF4C..0x8002AF54) for type 29. Unknown: indirect callers via entity->update.
 */
s32 gHandleGroundPairEntity(s32 message, struct Entity *self, void *data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB0;
    case 2:
        return gInitEntityGroundPair(self, data, arg3);
    case 10:
        return gTestEntityFieldMatch(self->unkAD, self->unkAE, data, 0);
    case 11:
        return gWarpEntityToSelf(self, data, arg3);
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 12:
    case 13:
        return 0;
    default:
        return gEntityBaseHandler(message, self, data, arg3);
    }
}
