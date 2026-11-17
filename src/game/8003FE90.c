// Categories: Game Engine > Callback Lists
typedef unsigned char u8;
struct Target { u8 pad[0x1c]; u8 flags; };
struct Owner { u8 pad[0x44]; struct Target *target; };
struct Node {
    struct Node *next;
    u8 pad[0x18];
    struct Owner *owner;
    u8 pad20[4];
    void (*callback)(int, struct Node *, int, int);
};
extern struct Node *gListHeads[7] __asm__("D_800A1070");

/**
 *  @brief Removes and notifies all list nodes associated with a target.
 *
 *  Searches seven linked lists for nodes whose owner references the target.
 *  Each removed node receives callback event 10 after it is unlinked.
 *  @param arg Target whose associated nodes are removed.
 *  @par Globals
 *  D_800A1070 contains the seven list heads.
 *  @par Control flow
 *  Traverses every list, preserving the next pointer before invoking a callback.
 *  Matching nodes are unlinked; other nodes remain in their original order.
 *  @par Calls
 *  Each removed node's callback is invoked with event 10 and two zero arguments.
 *  @par Side effects
 *  Changes list links, clears removed nodes' next pointers, and invokes callbacks.
 *  Clears bit zero of the target's flags after processing the lists.
 *  @par Callers
 *  Confirmed: func_8003FF7C.
 */
void func_8003FE90(struct Target *arg)
{
    register struct Node **cursor __asm__("$18");
    struct Node **base;
    register struct Node **head __asm__("$19");
    register struct Target *target __asm__("$20");
    struct Node *node, *next, *prev;
    int zero;
    target = arg;
    base = gListHeads;
    cursor = base;
    do {
        node = *cursor;
        prev = 0;
        if (node != 0) {
            head = cursor;
            do {
                next = node->next;
                if (node->owner->target == target) {
                    if (prev != 0) prev->next = next;
                    else *head = next;
                    zero = 0;
                    node->next = 0;
                    node->callback(10, node, zero, zero);
                } else {
                    prev = node;
                }
                node = next;
            } while (node != 0);
        }
        cursor++;
        __asm__("" : "+r"(base));
    } while ((int)cursor < (int)(base + 7));
    target->flags &= 0xfe;
}
