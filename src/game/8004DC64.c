// Categories: Gameplay > Entity Dispatch
#include "game/types.h"

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    u8 unk0C[0x08];
    u16 unk14;
};

extern struct Entity *gUnitListHead __asm__("D_8009FA40");

extern void func_80031BEC(s32 id, s32 kind, s32 mode);
extern void func_8003FD6C();
extern void gSetScriptTimerCount(s32 index, s32 kind, s32 value) __asm__("func_8004953C");
extern u8 func_8004DA84(struct Entity *entity, s32 kind, s32 value, s32 retry);

/**
 *  @brief Dispatches a kind and value to a special handler or matching entities.
 *
 *  Three kind values bypass the entity list. Other kinds are applied to every
 *  listed entity whose unk14 matches arg0 and whose id is nonzero.
 *  @param arg0 Identifier or index used by the selected dispatch path.
 *  @param kind Kind selecting the handler or passed to matching entities.
 *  @param value Value passed to the selected handler.
 *  @par Globals
 *  D_8009FA40 is the head of the entity list traversed for ordinary kinds.
 *  @par Control flow
 *  Kinds 0x7C, 0x7E, and 0x69 dispatch directly and return; otherwise a loop
 *  visits the entire list and processes each matching entity.
 *  @par Calls
 *  func_80031BEC, func_8004953C, and func_8003FD6C handle special kinds;
 *  func_8004DA84 handles each matching entity with retry set to 1.
 *  @par Side effects
 *  Invokes handlers that may modify game state; their effects are not shown here.
 *  @par Callers
 *  Confirmed: func_80069D68.
 */
void func_8004DC64(s32 arg0, s32 kind, s32 value)
{
    struct Entity *entity;

    switch (kind) {
    case 0x7C:
        func_80031BEC(arg0, 0x7C, value);
        return;
    case 0x7E:
        gSetScriptTimerCount(arg0, 0x7E, value);
        return;
    case 0x69:
        func_8003FD6C(arg0, 0x69, value);
        return;
    }

    for (entity = gUnitListHead; entity != 0; entity = entity->next) {
        if (entity->unk14 == arg0 && entity->id != 0) {
            func_8004DA84(entity, kind, value, 1);
        }
    }
}
