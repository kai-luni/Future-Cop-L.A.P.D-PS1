// Categories: Physics > Pair Processing
typedef struct Object Object;
typedef struct Node Node;
struct Object {
    int unused;
    int (*callback)(int, Object *, Object *, int);
    char pad[0x1c];
    int flags;
};
struct Node {
    Object *object;
    Node *next;
};
extern void func_800620D8(Object *, Object *, int);

/**
 *  @brief Processes eligible object pairs across two nonempty lists.
 *
 *  Every object in the first list is paired with every object in the second.
 *  Combined flags and optional callbacks determine whether a pair is processed.
 *  @param first Head of the nonempty first object list.
 *  @param second Head of the nonempty second object list.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Nested loops traverse the Cartesian product; combined flags must include
 *  0x2200, and objects with flag 0x800000 must approve in callback order.
 *  @par Calls
 *  Object callbacks receive event 10 and the pair; func_800620D8 processes
 *  pairs that pass both callback checks.
 *  @par Side effects
 *  Invokes callbacks and the pair helper, whose effects are not shown here.
 *  @par Callers
 *  Confirmed: func_80062614.
 */
void func_80062504(Node *first, Node *second)
{
    Node *cur;
    Object *a, *b;
    int flags, aflags;
    register int mask __asm__("$22") = 0x800000;
    __asm__ volatile ("" : : : "memory");
    cur = second;
    do {
        a = first->object;
        first = first->next;
        aflags = a->flags;
        do {
            b = cur->object;
            cur = cur->next;
            flags = aflags | b->flags;
            if (flags & 0x2200) {
                if (!(aflags & mask) || a->callback(10, a, b, 0)) {
                    if (!(b->flags & mask) || b->callback(10, b, a, 0)) {
                        func_800620D8(a, b, flags);
                    }
                }
            }
        } while (cur);
        cur = second;
    } while (first);
}
