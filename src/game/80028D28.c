// Categories: Game Engine > Lists
#include "game/types.h"

struct Node {
    struct Node *next;
    s32 unk04;
    s32 unk08;
    s32 count;
};

struct State {
    u8 pad[0x14];
    struct Node *head;
    struct Node *other;
};

extern struct State gStreamBufferList __asm__("D_800A0B70");
extern void gRecycleModuleReadBuffer(struct Node *node) __asm__("func_800288BC");

void gRetireParsedModuleBuffer(void) __asm__("func_80028D28");

/**
 * @brief Removes the first parsed buffer and moves it to the retained or reusable list.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Reads the node at state +0x14, advances that head and clears the node link and parse offset.
 * A positive reference count links it at state +0x18 or that list's tail. Other counts go
 * through gRecycleModuleReadBuffer.
 *
 * @par Globals
 * Updates the parsed and retained lists in gStreamBufferList.
 *
 * @par Callers
 * Direct calls from func_8002902C.
 *
 * @par Role and path
 * After finishing a buffer, the chunk parser retains it only while chunk references remain.
 *
 * @par Unknown
 * The parser supplies a nonnull source head.
 */
void gRetireParsedModuleBuffer(void)
{
    struct Node *node;
    struct Node *cur;

    node = gStreamBufferList.head;
    gStreamBufferList.head = node->next;
    node->next = 0;
    node->unk08 = 0;
    if (node->count > 0) {
        if (gStreamBufferList.other == 0) {
            gStreamBufferList.other = node;
        } else {
            cur = gStreamBufferList.other;
            while (cur->next != 0) {
                cur = cur->next;
            }
            cur->next = node;
        }
    } else {
        gRecycleModuleReadBuffer(node);
    }
}
