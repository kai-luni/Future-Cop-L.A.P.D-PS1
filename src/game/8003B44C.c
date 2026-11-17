// Categories: Game Engine > List Management
#include "game/types.h"

struct KindData {
    u8 pad[4];
    u8 kind;
};

struct Node {
    struct Node *next;
    u8 pad[0x1c];
    struct KindData *data;
};

extern struct Node *gListHeads[7] __asm__("D_800A1070");

/**
 *  @brief Prepends a node to a list selected by its data kind or index.
 *
 *  Seven recognized kind values override the supplied index; other kinds
 *  use the index unchanged to select a global list head.
 *  @param node Node to insert, with a valid data pointer.
 *  @param index Fallback list index for an unrecognized kind.
 *  @par Globals
 *  D_800A1070 contains the seven list heads and receives the new head.
 *  @par Control flow
 *  A switch maps kinds 0x5D, 0x59, 0x5C, 0x5E, 0x5A, 0x57, and 0x58
 *  to indices 0 through 6 respectively; the fallback index is not checked.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Replaces node->next and the selected list head.
 *  @par Callers
 *  Confirmed: func_8003F570.
 */
void func_8003B44C(struct Node *node, s32 index)
{
    node->next = 0;
    switch (node->data->kind) {
    case 0x5d: index = 0; break;
    case 0x59: index = 1; break;
    case 0x5c: index = 2; break;
    case 0x5e: index = 3; break;
    case 0x5a: index = 4; break;
    case 0x57: index = 5; break;
    case 0x58: index = 6; break;
    }
    node->next = gListHeads[index];
    gListHeads[index] = node;
}
