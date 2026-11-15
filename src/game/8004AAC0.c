// Categories: Utilities > Data Structures > Linked List
struct LinkNode {
    unsigned char pad_00[0x48];
    struct LinkNode *back;
    unsigned char pad_4C[0x38];
    struct LinkNode *forward;
};

/**
 * @brief Inserts a node into a doubly linked list after a given node
 *        (forward-only repair of the back link).
 *
 * @param left Insertion point; +0x84 the forward pointer.
 * @param right Node to insert; +0x48 the back pointer.
 * @return Nothing.
 *
 * @par Confirmed
 * right->back = left->forward; left->forward = right. The old successor's
 * back pointer is not updated here.
 *
 * @par Likely role
 * The fragment-list insert of the polygon fracture func_8004B5F8: the
 * shard chain is walked forwards, so only the new node's back link matters.
 *
 * @par Callers
 * func_8004B5F8.
 */
void func_8004AAC0(struct LinkNode *left, struct LinkNode *right)
{
    right->back = left->forward;
    left->forward = right;
}
