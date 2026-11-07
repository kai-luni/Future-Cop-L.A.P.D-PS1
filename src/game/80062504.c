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
