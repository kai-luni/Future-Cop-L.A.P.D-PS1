typedef struct Object Object;
typedef struct Node Node;
struct Object {
    int unused;
    int (*callback)(int, Object *, Object *, int);
    char pad[0x1c];
    unsigned int flags;
};
struct Node { Object *object; Node *next; };
extern void func_800620D8(Object *, Object *, unsigned int);
void func_800623FC(Node *list)
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
                            func_800620D8(a, b, combined);
                        }
                    }
                }
            } while (other);
        }
    } while (list);
}
