// Categories: Physics > Pair Processing
typedef struct Object Object;
typedef struct Node Node;
struct Object {
    int unused;
    int (*callback)(int, Object *, Object *, int);
    char pad[0x1c];
    unsigned int flags;
};
struct Node { Object *object; Node *next; };
extern void gTestEntityPairCollision(Object *, Object *, unsigned int) __asm__("func_800620D8");
void gProcessCellCollisionPairs(Node *list) __asm__("func_800623FC");


/**
 *  @brief Processes eligible unordered pairs from a nonempty object list.
 *
 *  Each pair is filtered by combined flags and optional per-object callbacks
 *  before being passed to the pair-processing helper.
 *  @param list Head of a nonempty list of objects.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Nested loops visit each pair once; combined flags must include 0x2200.
 *  Objects with flag 0x800000 must approve through their callback, in order.
 *  @par Calls
 *  Object callbacks receive event 10 and the pair; func_800620D8 processes
 *  pairs that pass both callback checks.
 *  @par Side effects
 *  Invokes callbacks and the pair helper, whose effects are not shown here.
 *  @par Callers
 *  Confirmed: func_80062614.
 */
void gProcessCellCollisionPairs(Node *list)
{
    Object *a, *b;
    Node *other;
    unsigned int flags, combined;
    do {
        a = list->object;
        list = list->next;
        flags = a->flags;
        other = list;
        if (other) {
            do {
                b = other->object;
                other = other->next;
                combined = flags | b->flags;
                if (combined & 0x2200) {
                    if (!(flags & 0x800000) || a->callback(10, a, b, 0)) {
                        if (!(b->flags & 0x800000) || b->callback(10, b, a, 0)) {
                            gTestEntityPairCollision(a, b, combined);
                        }
                    }
                }
            } while (other);
        }
    } while (list);
}
