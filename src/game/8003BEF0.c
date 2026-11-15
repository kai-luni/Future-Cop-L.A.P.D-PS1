// Categories: Graphics > Scene Graph > Object List | Utilities
struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

/**
 * @brief Pops the last child of a node's child list (LIFO, doubly linked).
 *
 * @param node Parent node: @c previous, @c next and @c tail (the list is
 *             double-linked and keeps a tail pointer).
 * @return The removed node, or NULL when the child list is empty.
 *
 * @par Confirmed
 * tail = node->tail; if (tail) { node->tail = tail->previous;
 * tail->previous = node->next; node->next = tail; } - i.e. the popped node is
 * re-linked in front of the parent's sibling chain, which is the shape of an
 * "unlink and re-insert" step rather than a plain pop.
 *
 * @par Callers
 * The scene walkers: func_8005BDE4, func_8005CBCC, func_8006F0E8,
 * func_80075A84 and the unit at 0x80075698.
 *
 * @par Unknown
 * Whether the re-linking into node->next is a deliberate ordering or an
 * artefact of a doubly linked list with an implicit sentinel.
 */
struct ListNode *func_8003BEF0(struct ListNode *node)
{
    struct ListNode *tail = node->tail;

    if (tail != 0) {
        node->tail = tail->previous;
        tail->previous = node->next;
        node->next = tail;
    }
    return tail;
}
