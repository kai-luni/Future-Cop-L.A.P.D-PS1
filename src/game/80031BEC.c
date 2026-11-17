// Categories: Game Engine > Object State
#include "game/types.h"

struct Unk31Node {
    s16 x;
    s16 unk02;
    s16 z;
    s16 unk06;
    s32 id;
    s16 extentX;
    s16 unk0E;
    s16 extentZ;
    s16 unk12;
    u8 flags;
    u8 unk15;
    s16 unk16;
    u8 unk18[0x0C];
    struct Unk31Node *next;
};

extern struct Unk31Node *gMapZoneList __asm__("D_8009F6F8");

/**
 *  @brief Updates flags on the first list node matching an identifier.
 *
 *  Only kind 0x7C changes the matching node; the mode selects a flag
 *  update or sets a separate byte field.
 *  @param id Identifier to find in the node list.
 *  @param kind Kind selector; only 0x7C enables an update.
 *  @param mode Update selector: 1 clears, 0 sets, and other values set unk15.
 *  @par Globals
 *  D_8009F6F8 is the head of the linked node list.
 *  @par Control flow
 *  Traverses until the first matching id, handles its kind and mode, then returns.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  May change flags, unk15, or unk16 on the matching node.
 *  @par Callers
 *  Confirmed: func_8004DC64.
 */
void func_80031BEC(s32 id, s32 kind, s32 mode)
{
    struct Unk31Node *node;

    for (node = gMapZoneList; node != 0; node = node->next) {
        if (node->id == id) {
            if (kind == 0x7C) {
                if (mode == 1) {
                    node->unk16 = 0;
                    node->flags &= ~0x10;
                } else if (mode == 0) {
                    node->flags |= 0x10;
                } else {
                    node->unk15 = 1;
                }
            }
            return;
        }
    }
}
