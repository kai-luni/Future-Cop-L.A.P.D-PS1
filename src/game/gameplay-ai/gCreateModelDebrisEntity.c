// Categories: Gameplay > Object Spawning | Graphics > Effects | Game Engine > Object Setup
#include "game/types.h"

extern int gSpawnQueuedEntity();

struct Unk3A8C0Args {
    s32 arg0;
    s32 arg2;
    s32 arg3;
    u8 arg1;
};

struct Unk3A8C0Request {
    void (*handler)(void);
    s32 unk04;
    struct Unk3A8C0Args *args;
    s32 unk0C;
};

extern void gModelDebrisEntityHandler(void) __asm__("D_8003B2D0");
extern void gSpawnQueuedEntityAndGetId(struct Unk3A8C0Request *request) __asm__("func_8004D4D4");
void gCreateModelDebrisEntity(s32 arg0, u8 arg1, s32 arg2, s32 arg3) __asm__("func_8003A8C0");


/**
 * @brief Queues a spawn request for a particle-emitter entity handled by func_8003B2D0.
 *
 * Confirmed: populates stack-local request (handler = D_8003B2D0 [func_8003B2D0], unk04 = 0, unk0C = 0, args = &args) and args (arg0, arg2, arg3, u8 arg1) and calls func_8004D4D4(&request), which spawns the entity via gSpawnQueuedEntity and discards its entity id.
 * Category follows spawn dispatcher func_8004D4D4, particle-emitter handler func_8003B2D0 (type 0xCC, particle decay and skinned-part draw), and debris caller func_8004C324.
 * Likely: spawns a short-lived particle-emitter attachment for debris/fracture objects using the nine copied matrix words; Unknown: exact meaning of arg0..arg3 inside the packed Unk3A8C0Args layout read by func_8003AA94.
 *
 * @param arg0 First s32 argument stored at args.arg0 (+0x00, data pointer from func_8004C324).
 * @param arg1 Byte argument stored at args.arg1 (+0x0C, -1 / 0xFF from func_8004C324).
 * @param arg2 Second s32 argument stored at args.arg2 (+0x04, owner entity pointer from func_8004C324).
 * @param arg3 Third s32 argument stored at args.arg3 (+0x08, forwarded arg2 from func_8004C324).
 *
 * @par Globals
 * None directly (references address of func_8003B2D0, declared as D_8003B2D0).
 * @par Control flow
 * Linear stack setup and single call to func_8004D4D4(&request); no branches or loops.
 * @par Calls
 * func_8004D4D4 - allocates, initializes via func_8003B2D0 (message 1 and message 2 / func_8003AA94), links the entity, and returns its id.
 * @par Side effects
 * Allocates and links a new 0xCC-byte particle-emitter entity in the active entity list via func_8004D4D4.
 * @par Callers
 * Confirmed: func_8004C324 (0x8004C4E0 in asm/us/main.s, message 0 when (obj->flags & 0x10), sub->flag is set, and !link->flag38) / Likely: none / Unknown: none.
 */
void gCreateModelDebrisEntity(s32 arg0, u8 arg1, s32 arg2, s32 arg3)
{
    struct Unk3A8C0Request request;
    struct Unk3A8C0Args args;

    request.handler = gModelDebrisEntityHandler;
    request.unk04 = 0;
    request.unk0C = 0;
    request.args = &args;
    args.arg0 = arg0;
    args.arg1 = arg1;
    args.arg2 = arg2;
    args.arg3 = arg3;
    gSpawnQueuedEntityAndGetId(&request);
}
