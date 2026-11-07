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
extern struct Node *D_800A1070[7];
void func_8003FE90(struct Target *arg)
{
    register struct Node **cursor __asm__("$18");
    struct Node **base;
    register struct Node **head __asm__("$19");
    register struct Target *target __asm__("$20");
    struct Node *node, *next, *prev;
    int zero;
    target = arg;
    base = D_800A1070;
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
